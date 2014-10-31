#ifndef SMPPSERVER_H
#define SMPPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QHash>

class ISmsGateway;

class SmppServer : public QObject
{
    Q_OBJECT
public:
    explicit SmppServer(const QHostAddress &address = QHostAddress::Any,
                        quint16 port = 0,
                        ISmsGateway *smsGateway = 0,
                        QObject *parent = 0);
    void addAuthenticationInfo(const QString &userName, const QString &userPassword);

signals:

public slots:

private slots:
    void onNewConnection();

private:
    QTcpServer *m_server;
    QList<QPair<QString, QString> > m_authentication;
    ISmsGateway *m_smsGateway;

};

#endif // SMPPSERVER_H
