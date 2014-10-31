#ifndef SMPPSMSMESSAGE_H
#define SMPPSMSMESSAGE_H

#include <QObject>
#include <QDateTime>

class SmppSmsMessage
{

public:
    QByteArray getSubmitSmPacket() const;

    int sequenceNumber() const;
    void setSequenceNumber(int sequenceNumber);

    QString recepient() const;
    void setRecepient(const QString &recepient);

    QString message() const;
    void setMessage(const QString &message);

    QString messageId() const;
    void setMessageId(const QString &messageId);

    bool isAscii() const;

    QString senderAddr() const;
    void setSenderAddr(const QString &sender);

    QString smppServerMessageId() const;
    void setSmppServerMessageId(const QString &smppServerMessageId);

    QDateTime sendToServerTime() const;
    void setSendToServerTime(const QDateTime &sendToServerTime);

    int currentMessageState() const;
    void setCurrentMessageState(int currentMessageState);

private:
    int m_sequenceNumber;
    QString m_recepient;
    QString m_sender;
    QString m_message;
    QString m_messageId;
    QString m_smppServerMessageId;
    QDateTime m_sendToServerTime;
    int m_currentMessageState;
};

#endif // SMPPSMSMESSAGE_H
