#ifndef SMPPCLIENT_H
#define SMPPCLIENT_H

#include "smpp-client/smppsmsmessage.h"
#include "smpp-client/smppcxx/smpp.hpp"

#include <QObject>
#include <QTcpSocket>

class QTimer;

class SmppClient : public QObject
{
    Q_OBJECT
public:
    enum SmsStates {
        Queued,
        Sended,
        Delivered,
        Accepted,
        Rejected,
        Unknown
    };

    enum DisconnectReason {
        CannotConnectToHost,
        DisconnectedBySmppServer,
        AuthenticationProblem,
        UnknownError
    };

    explicit SmppClient(const QString &smppUserName,
                        const QString &smppPassword,
                        QObject *parent = 0);
    void connectToHost(const QString & hostName, quint16 port);
    void disconnectFromHost();
    bool isOpen() const;
    void sendSms(const QString &messageId, const QString &recepient, const QString &sender, const QString &smsText);
    QString sendSms(const QString &recepient, const QString &sender, const QString &smsText);

    QString smppUserName() const;
    QString smppPassword() const;

signals:
    void smsStatusChanged(const QString &messageId, SmppClient::SmsStates newStatus);
    void connected();
    void authenticated();
    void disconnected(SmppClient::DisconnectReason disconnectReason);
    void incomingSms(const QString &from, const QString &to, const QString &smsText);

private:
    void sendBindTransiver();

    static int nextSequenceNumber();
    static QString createNewMessageId();

    void onDisconnectHelper();

    void handleIncomingData();

    void handleSubmitSmResp(const QByteArray &pdu);
    void handleDeliverSm(const QByteArray &pdu);
    void handleBindTransiverResp(const QByteArray &pdu);
    void handleEnquireLinkResp(const QByteArray &pdu);

    void stopTimer();
    void startTimer();

    void logMsg(const QString &msg) const;

    void printErrorAndDisconnect(const Smpp::Error &e);

    template<typename T>
    void writeSmppPacket(T &packet) {
        if (isOpen()){
            QByteArray outgoingPdu((const char *)packet.encode(), packet.command_length());
            m_transport->write(outgoingPdu);
        }
    }

private slots:
    void onConnected();
    void onDisconnected();
    void onError (QAbstractSocket::SocketError socketError );
    void onReadyRead();
    void onTimeout();
    void onEnquireLinkTimeout();

public slots:

private:
    QTcpSocket *m_transport;
    QTimer *m_timer;
    QTimer *m_timerForEnquireLink;
    QByteArray m_buffer;
    const QString m_smppUserName;
    const QString m_smppPassword;
    QList<SmppSmsMessage> m_waitingResponseMessages;
    QHash<QString, SmppSmsMessage> m_pendingDeliveryRequestMessages;
    bool m_needInformAbountDisconnect;
    bool m_isConnected;
    static QAtomicInt m_sequenceNumber;
};

#endif // SMPPCLIENT_H
