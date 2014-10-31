#include "smsgateway.h"

SmsGateway::SmsGateway(QObject *parent) :
    ISmsGateway(parent)
{
}

void SmsGateway::sendSms(
        const QString &messageId,
        const QString &preferedSender,
        const QString &recepinet,
        const QString &smsText,
        const quint16 &priority)
{
    Q_UNUSED(priority)
    emit smsSended(messageId, preferedSender, recepinet, smsText);
}

void SmsGateway::fakeIncomingSms(const QString &from, const QString &to, const QString &smsText)
{
    emit incomingSms(from, to, smsText);
}

void SmsGateway::fakeSmsStatusChanged(const QString &messageId, const int smsStatus)
{
    emit smsStatusChanged(messageId, smsStatus);
}
