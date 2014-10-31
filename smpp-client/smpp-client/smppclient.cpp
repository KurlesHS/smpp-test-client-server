#include "smppclient.h"
#include "qtopia/qgsmcodec.h"
#include <QMetaType>
#include <QTimer>
#include <QUuid>

QAtomicInt SmppClient::m_sequenceNumber = QAtomicInt(0x01);

Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
Q_DECLARE_METATYPE(SmppClient::SmsStates)
Q_DECLARE_METATYPE(SmppClient::DisconnectReason)

#define EnquireLinkInterval 120000

class SmppHeader : public Smpp::Request {
public:
    SmppHeader() : Smpp::Request(Smpp::CommandLength(0x10),
                                Smpp::CommandId(0x00),
                                Smpp::SequenceNumber(0x01)) {

    }
    void decode(const Smpp::Uint8* b) { Header::decode(b); }
};

SmppClient::SmppClient(const QString &smppUserName, const QString &smppPassword, QObject *parent) :
    QObject(parent),
    m_transport(new QTcpSocket(this)),
    m_timer(new QTimer(this)),
    m_timerForEnquireLink(new QTimer(this)),
    m_smppUserName(smppUserName),
    m_smppPassword(smppPassword)
{
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
        qRegisterMetaType<SmppClient::SmsStates>("SmppClient::SmsStates");
        qRegisterMetaType<SmppClient::DisconnectReason>("SmppClient::DisconnectReason");
    }
    m_timerForEnquireLink->setInterval(EnquireLinkInterval);
    connect(m_transport, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));
    connect(m_transport, SIGNAL(connected()),
            this, SLOT(onConnected()));
    connect(m_transport, SIGNAL(disconnected()),
            this, SLOT(onDisconnected()));
    connect(m_transport, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));

    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));

    connect(m_timerForEnquireLink, SIGNAL(timeout()),
            this, SLOT(onEnquireLinkTimeout()));
}

void SmppClient::connectToHost(const QString &hostName, quint16 port)
{
    if (m_transport) {
        m_transport->connectToHost(hostName, port);
        m_needInformAbountDisconnect = true;
    }
}

void SmppClient::disconnectFromHost()
{
    if (m_transport) {
        m_transport->disconnectFromHost();
    }
}

bool SmppClient::isOpen() const
{
    return m_transport && m_transport->isOpen();
}

void SmppClient::sendSms(const QString &messageId, const QString &recepient, const QString &sender, const QString &smsText)
{
    if (isOpen()) {
        SmppSmsMessage smsMessage;
        smsMessage.setMessageId(messageId);
        smsMessage.setMessage(smsText);
        smsMessage.setSenderAddr(sender);
        smsMessage.setRecepient(recepient);
        smsMessage.setSequenceNumber(nextSequenceNumber());
        smsMessage.setSendToServerTime(QDateTime::currentDateTime());
        m_transport->write(smsMessage.getSubmitSmPacket());
        m_waitingResponseMessages.append(smsMessage);
    } else {
        emit smsStatusChanged(messageId, Rejected);
    }
}

QString SmppClient::sendSms(const QString &recepient, const QString &sender, const QString &smsText)
{
    QString messageId = createNewMessageId();
    sendSms(messageId, recepient, sender, smsText);
    return messageId;
}

QString SmppClient::createNewMessageId()
{
    return QUuid::createUuid().toString().remove("-").remove("{").remove("}");
}

void SmppClient::onDisconnectHelper()
{
    for (const SmppSmsMessage &smsMessage : m_waitingResponseMessages) {
        emit smsStatusChanged(smsMessage.messageId(), Rejected);
    }
    m_waitingResponseMessages.clear();
    m_pendingDeliveryRequestMessages.clear();
}

void SmppClient::handleSubmitSmResp(const QByteArray &pdu)
{
    Smpp::SubmitSmResp submitSmResp;
    try {
        submitSmResp.decode((const Smpp::Uint8 *) pdu.data());
    } catch (Smpp::Error e) {
        printErrorAndDisconnect(e);
        return;
    }
    int sequenceNumber = submitSmResp.sequence_number();
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QList<int> indexesToDelete;
    for (int i = 0; i < m_waitingResponseMessages.count(); ++i) {
        SmppSmsMessage smppMessage = m_waitingResponseMessages.at(i);
        if (smppMessage.sequenceNumber() == sequenceNumber) {
            indexesToDelete.append(i);
            smppMessage.setCurrentMessageState(Accepted);
            emit smsStatusChanged(smppMessage.messageId(), Accepted);
            QString smppMessageId = QString::fromStdString(submitSmResp.message_id());
            smppMessage.setSmppServerMessageId(smppMessageId);
            m_pendingDeliveryRequestMessages[smppMessageId] = smppMessage;
        } else if (smppMessage.sendToServerTime().secsTo(currentDateTime) > 60000) {
            // больше минуты нет отклика об принятии смс - беда
            indexesToDelete.append(i);
            emit smsStatusChanged(smppMessage.messageId(), Rejected);
        }
    }
    qSort(indexesToDelete.begin(), indexesToDelete.end(),
          [](const int a, const int b){
        return a > b;
    });
    for (const int &idx : indexesToDelete) {
        m_waitingResponseMessages.removeAt(idx);
    }
    logMsg(QString("number of waiting response messages: %0").arg(m_waitingResponseMessages.count()));
}

void SmppClient::handleDeliverSm(const QByteArray &pdu)
{
    Smpp::DeliverSm deliverSm;
    try {
        deliverSm.decode((const Smpp::Uint8 *) pdu.data());
    } catch (Smpp::Error e) {
        printErrorAndDisconnect(e);
        return;
    }
    int esmClass = deliverSm.esm_class();
    if (esmClass & 0x04) {
        // delivery receipt
        const Smpp::Tlv *tlv = deliverSm.find_tlv(Smpp::Tlv::receipted_message_id);
        if (tlv) {
            QString smppMessageId = QString::fromLatin1((const char*)tlv->value());
            logMsg(QString("delivery receipt received with message id '%0'").arg(smppMessageId));
            if (m_pendingDeliveryRequestMessages.contains(smppMessageId)) {
                tlv = deliverSm.find_tlv(Smpp::Tlv::message_state);
                if (tlv){
                    SmppSmsMessage smppMessage = m_pendingDeliveryRequestMessages.value(smppMessageId);
                    Smpp::Uint8 messageState = *tlv->value();
                    SmsStates state = Unknown;
                    SmsStates currentState = (SmsStates)smppMessage.currentMessageState();
                    switch (messageState) {
                    case 0x01: // ENROUTE
                        state = Sended;
                        break;
                    case 0x06: // ACCEPTED
                        state = Accepted;
                        break;
                    case 0x02: { // DELIVERED
                        state = Delivered;
                        m_pendingDeliveryRequestMessages.remove(smppMessageId);
                    }
                        break;
                    case 0x08: { // REJECTED
                        state = Rejected;
                        m_pendingDeliveryRequestMessages.remove(smppMessageId);
                    }
                        break;
                    default: {
                        m_pendingDeliveryRequestMessages.remove(smppMessageId);
                    }
                        break;
                    }
                    if (currentState != state){
                        emit smsStatusChanged(smppMessage.messageId(), state);
                        if (m_pendingDeliveryRequestMessages.contains(smppMessageId)) {
                            smppMessage.setCurrentMessageState(state);
                            m_pendingDeliveryRequestMessages[smppMessageId] = smppMessage;
                        }
                    }
                }
            }
        }
    } else {
        // incoming sms
        int smLength = deliverSm.sm_length();
        QString textSms;
        QByteArray shortMessage;
        if (smLength > 0) {
            std::vector<Smpp::Uint8> tmp = deliverSm.short_message();
            shortMessage = QByteArray((const char*)tmp.data(), deliverSm.short_message().length());
        } else {
            const Smpp::Tlv *tlv = deliverSm.find_tlv(Smpp::Tlv::message_payload);
            if (tlv) {
                shortMessage = QByteArray((const char*)tlv->value(), tlv->length());
            }
        }
        int dataCoding = deliverSm.data_coding();
        QGsmCodec gsmCodec;
        QTextCodec *codec = 0;
        if (dataCoding == 0x00) {
           codec = &gsmCodec;
        } else if (dataCoding == 0x08) {
            codec = QTextCodec::codecForName("UTF-16");
        }
        if (codec) {
            textSms = codec->toUnicode(shortMessage);
        }
        const Smpp::Address toAddr = deliverSm.destination_addr().address();
        QString to = QString::fromStdString(toAddr);
        const Smpp::Address fromAddr = deliverSm.source_addr().address();
        QString from = QString::fromStdString(fromAddr);
        if (!textSms.isEmpty()) {
            emit incomingSms(from, to, textSms);
            logMsg(QString("incoming sms received. Text: %0, from: %1, to: %2")
                   .arg(textSms, from, to));
        }
    }
    Smpp::DeliverSmResp deliverSmResp;
    deliverSmResp.sequence_number(deliverSm.sequence_number());
    writeSmppPacket<Smpp::DeliverSmResp>(deliverSmResp);
}

void SmppClient::handleBindTransiverResp(const QByteArray &pdu)
{
    Smpp::BindTransceiverResp bindTransceiverResp;
    try {
        bindTransceiverResp.decode((const Smpp::Uint8 *) pdu.data());
    } catch (Smpp::Error e) {
        printErrorAndDisconnect(e);
        return;
    }
    if (bindTransceiverResp.command_status() != 0) {
        m_needInformAbountDisconnect = false;
        onDisconnectHelper();
        emit disconnected(AuthenticationProblem);
        logMsg("Authentication problem");
    } else {
        logMsg("Authentication on smpp server passed");
        emit authenticated();
        m_timerForEnquireLink->start();
    }
}

void SmppClient::handleEnquireLinkResp(const QByteArray &pdu)
{
    Q_UNUSED(pdu);
    logMsg("EnquireLink response received: connection with smpp server is ok");
}

void SmppClient::stopTimer()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

void SmppClient::startTimer()
{
    m_timer->start(5000);
}

void SmppClient::logMsg(const QString &msg) const
{
    qDebug(QString("[SmppClient] %0").arg(msg).toUtf8().data());
}

void SmppClient::printErrorAndDisconnect(const Smpp::Error &e)
{
    logMsg(QString("error: %e").arg(e.what()));
    m_needInformAbountDisconnect = false;
    emit disconnected(UnknownError);
    disconnectFromHost();
}

void SmppClient::onConnected()
{
    logMsg("connection estabilished");
    emit connected();
    m_isConnected = true;
    sendBindTransiver();
}

void SmppClient::onDisconnected()
{
    m_timerForEnquireLink->stop();
    m_isConnected = false;
    if (m_needInformAbountDisconnect){
        emit disconnected(DisconnectedBySmppServer);
    }
    onDisconnectHelper();
}

void SmppClient::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    if (!m_isConnected) {
        emit disconnected(CannotConnectToHost);
        onDisconnectHelper();
    }
}

void SmppClient::onReadyRead()
{
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(sender());
    if (socket) {
        stopTimer();
        m_buffer.append(socket->readAll());
        handleIncomingData();
    }
}

void SmppClient::onTimeout()
{

}

void SmppClient::onEnquireLinkTimeout()
{
   Smpp::EnquireLink enquireLink;
   enquireLink.sequence_number(nextSequenceNumber());
   writeSmppPacket<Smpp::EnquireLink>(enquireLink);
}
QString SmppClient::smppPassword() const
{
    return m_smppPassword;
}

void SmppClient::sendBindTransiver()
{
    Smpp::BindTransceiver bind;
    bind.sequence_number(nextSequenceNumber());
    bind.password(smppPassword().toLatin1().data());
    bind.system_id(smppUserName().toLatin1().data());
    writeSmppPacket<Smpp::BindTransceiver>(bind);
}

void SmppClient::handleIncomingData()
{
    if (m_buffer.length() < 16) {
        startTimer();
        return;
    }
    SmppHeader header;
    header.decode((const Smpp::Uint8 *)m_buffer.data());
    if ((int)header.command_length() < m_buffer.length()) {
        startTimer();
        return;
    }

    Smpp::Uint32 commandId = header.command_id();
    QByteArray pdu = m_buffer.mid(0, header.command_length());
    m_buffer.remove(0, header.command_length());

    switch (commandId) {
    case Smpp::CommandId::SubmitSmResp:
        handleSubmitSmResp(pdu);
        break;
    case Smpp::CommandId::BindTransceiverResp:
        handleBindTransiverResp(pdu);
        break;
    case Smpp::CommandId::DeliverSm:
        handleDeliverSm(pdu);
        break;
    case Smpp::CommandId::EnquireLinkResp:
        handleEnquireLinkResp(pdu);
        break;
    default:
        logMsg(QString("unsupported commandId: 0x%0")
               .arg(commandId, 0x08, 0x10, QChar('0')));
        break;
    }
    if (m_buffer.length()) {
        handleIncomingData();
    }
}

QString SmppClient::smppUserName() const
{
    return m_smppUserName;
}

int SmppClient::nextSequenceNumber()
{
    return m_sequenceNumber.fetchAndAddOrdered(0x01);
}

