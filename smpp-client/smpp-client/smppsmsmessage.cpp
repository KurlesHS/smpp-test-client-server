#include "smppsmsmessage.h"
#include "smpp-client/smppcxx/smpp.hpp"
#include "qtopia/qgsmcodec.h"

QByteArray SmppSmsMessage::getSubmitSmPacket() const
{
    Smpp::SubmitSm submitSm;
    QByteArray messagePayload;
    if (isAscii()) {
        submitSm.data_coding(0x00);
        QGsmCodec codec;
        messagePayload = codec.fromUnicode(message());
    } else {
        submitSm.data_coding(0x08);
        QTextCodec *codec = QTextCodec::codecForName("UTF-16");
        messagePayload = codec->fromUnicode(message());
    }
    submitSm.insert_array_tlv(Smpp::Tlv::message_payload, messagePayload.size(),
                              (const Smpp::Uint8*)messagePayload.data());
    Smpp::Address destination_addr(recepient().toStdString());
    submitSm.destination_addr(destination_addr);
    if (!senderAddr().isEmpty()) {
        Smpp::Address sourceAddr(senderAddr().toStdString());
        submitSm.source_addr(sourceAddr);
    }
    submitSm.registered_delivery(1);
    submitSm.sequence_number(sequenceNumber());
    QByteArray retVal((const char*)submitSm.encode(), submitSm.command_length());
    return retVal;
}

int SmppSmsMessage::sequenceNumber() const
{
    return m_sequenceNumber;
}

void SmppSmsMessage::setSequenceNumber(int SequenceNumber)
{
    m_sequenceNumber = SequenceNumber;
}
QString SmppSmsMessage::recepient() const
{
    return m_recepient;
}

void SmppSmsMessage::setRecepient(const QString &recepient)
{
    m_recepient = recepient;
}
QString SmppSmsMessage::message() const
{
    return m_message;
}

void SmppSmsMessage::setMessage(const QString &message)
{
    m_message = message;
}
QString SmppSmsMessage::messageId() const
{
    return m_messageId;
}

void SmppSmsMessage::setMessageId(const QString &messageId)
{
    m_messageId = messageId;
}

bool SmppSmsMessage::isAscii() const
{
    QByteArray utf8 = m_message.toUtf8();
    for (int i = 0; i < utf8.length(); ++i) {
        if ((unsigned char)utf8.at(i) >= 0x80) {
            return false;
        }
    }
    return true;
}
QString SmppSmsMessage::senderAddr() const
{
    return m_sender;
}

void SmppSmsMessage::setSenderAddr(const QString &sender)
{
    m_sender = sender;
}
QString SmppSmsMessage::smppServerMessageId() const
{
    return m_smppServerMessageId;
}

void SmppSmsMessage::setSmppServerMessageId(const QString &smppServerMessageId)
{
    m_smppServerMessageId = smppServerMessageId;
}
QDateTime SmppSmsMessage::sendToServerTime() const
{
    return m_sendToServerTime;
}

void SmppSmsMessage::setSendToServerTime(const QDateTime &sendToServerTime)
{
    m_sendToServerTime = sendToServerTime;
}
int SmppSmsMessage::currentMessageState() const
{
    return m_currentMessageState;
}

void SmppSmsMessage::setCurrentMessageState(int currentMessageState)
{
    m_currentMessageState = currentMessageState;
}








