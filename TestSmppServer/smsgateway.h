#ifndef SMSGATEWAY_H
#define SMSGATEWAY_H

#include <smpp-server/interfaces/ismsgateway.h>

class SmsGateway : public ISmsGateway
{
    Q_OBJECT
public:
    SmsGateway(QObject *parent);

    // ISmsGateway interface
public:
    virtual void sendSms(const QString &messageId,
            const QString &preferedSender,
            const QString &recepinet,
            const QString &smsText, const quint16 &priority);

    void fakeIncomingSms(const QString &from, const QString &to, const QString &smsText);
    void fakeSmsStatusChanged(const QString &messageId, const int smsStatus);

signals:
    void smsSended(
            const QString &messageId,
            const QString &preferedSender,
            const QString &recepinet,
            const QString &smsText);

};

#endif // SMSGATEWAY_H
