#ifndef ISMSGATEWAY_H
#define ISMSGATEWAY_H

#include <QObject>

class ISmsGateway : public QObject
{
    Q_OBJECT
public:
    enum SmsStatus {
        Queued,
        StartSending,
        Sended,
        Delivered,
        ErrorWhileSending,
        Rejected
    };

    ISmsGateway(QObject *parent) : QObject(parent)  {}

    virtual void sendSms(const QString &messageId,
                         const QString &preferedSender,
                         const QString &recepinet,
                         const QString &smsText,
                         const quint16 &priority) = 0;

signals:
    void incomingSms(const QString &from ,const QString &to,
                     const QString &smsText);

    void smsStatusChanged(const QString &messageId, int smsStatus);
};


#endif // ISMSGATEWAY_H
