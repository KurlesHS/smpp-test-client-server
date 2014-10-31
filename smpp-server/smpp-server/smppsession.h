#ifndef SMPPSESSION_H
#define SMPPSESSION_H
#include "smppcxx/smpp.hpp"
#include "interfaces/ismsgateway.h"

#include <QObject>
#include <QTcpSocket>
#include <QAtomicInt>
#include <QHash>
#include <QDateTime>

class QTimer;
class ISmsGateway;

struct SmsInfo {
    QDateTime sendTime;
    QString recepient;
    QString textMessage;
    bool registerDelivery;
};

class SmppSession : public QObject
{
    Q_OBJECT
public:
    explicit SmppSession(QTcpSocket *clientSocket,
                         QList<QPair<QString, QString> > *authInfo, ISmsGateway *smsGateway,

                         QObject *parent = 0);
    ~SmppSession();
    void handleIncomingData();

signals:

private:
    static int nextSequenceNumber();
    void handleBindTransceiver(const QByteArray &pdu);
    void handleSubmitSm(const QByteArray &pdu);
    void handleEnquireLink(const QByteArray &pdu);
    void handleUnbind(const QByteArray &pdu);

    void handleDeliverSmResp(const QByteArray &pdu);

    void sendDeliveryReport(const QString &messageId, ISmsGateway::SmsStatus status);
    void sendIncomingMessage(const QString &from, const QString &to,
                             const QString &smsText);

    void startTimeoutTimer();
    void stopTimeoutTimer();

    void printErrorAndDisconnect(const Smpp::Error &e);
    void disconnectFromClient();

    void logMsg(const QString &msg);

    template<typename T>
    void writeSmppPacket(T &packet) {
        QByteArray outgoingPdu((const char *)packet.encode(), packet.command_length());
        m_clientSocket->write(outgoingPdu);
    }

    bool checkAuthentication();

    static QString createNewMessageId();

private slots:
   void onClientDisconnected();
   void onReadyRead();
   void onTimerTimeout();

   void onIncomingSms(const QString &from ,const QString &to,
                    const QString &smsText);

   void onSmsStatusChanged(const QString &messageId, int smsStatus);

private:
    QTcpSocket * const m_clientSocket;
    QList<QPair<QString, QString> > *m_authInfo;
    ISmsGateway *m_smsGateway;
    QByteArray m_buffer;
    static QAtomicInt m_sequenceNumber;
    QTimer *m_timeoutTimer;
    QString m_logStringTemplage;
    QString m_clientStrAddr;
    QHash<QString, SmsInfo> m_pendingSms;

    bool m_isAuthenticated;

};

#endif // SMPPSESSION_H
