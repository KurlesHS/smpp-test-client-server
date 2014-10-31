#include "smppserver.h"
#include "smppsession.h"
#include "interfaces/ismsgateway.h"

SmppServer::SmppServer(const QHostAddress &address, quint16 port, ISmsGateway *smsGateway, QObject *parent) :
    QObject(parent),
    m_server(new QTcpServer(this)),
    m_smsGateway(smsGateway)
{
    connect(m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    m_server->listen(address, port);
}

void SmppServer::addAuthenticationInfo(const QString &userName, const QString &userPassword)
{
    m_authentication.append(QPair<QString, QString>(userName, userPassword));
}

void SmppServer::onNewConnection()
{
    QTcpServer *server = qobject_cast<QTcpServer*>(sender());
    if (server) {
        while (server->hasPendingConnections()) {
            QTcpSocket *socket = server->nextPendingConnection();
            if (socket) {
                new SmppSession(socket, &m_authentication, m_smsGateway);
            }
        }
    }
}
