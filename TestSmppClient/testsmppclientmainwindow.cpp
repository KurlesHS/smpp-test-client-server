#include "testsmppclientmainwindow.h"
#include "ui_testsmppclientmainwindow.h"

TestSmppClientMainWindow::TestSmppClientMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TestSmppClientMainWindow),
    m_smppClient(new SmppClient("user", "pass", this))
{
    connect(m_smppClient, SIGNAL(connected()),
            this, SLOT(onConnected()));
    connect(m_smppClient, SIGNAL(disconnected(SmppClient::DisconnectReason)),
            this, SLOT(onDisconnected(SmppClient::DisconnectReason)));
    connect(m_smppClient, SIGNAL(incomingSms(QString,QString,QString)),
            this, SLOT(onIncomingSms(QString,QString,QString)));
    connect(m_smppClient, SIGNAL(smsStatusChanged(QString,SmppClient::SmsStates)),
            this, SLOT(onSmsStatusChanged(QString,SmppClient::SmsStates)));
    connect(m_smppClient, SIGNAL(authenticated()),
            this, SLOT(onAuthenticated()));
    m_smppClient->connectToHost("localhost", 8001);
    ui->setupUi(this);
}

TestSmppClientMainWindow::~TestSmppClientMainWindow()
{
    delete ui;
}

void TestSmppClientMainWindow::onSmsStatusChanged(const QString &messageId, SmppClient::SmsStates newStatus)
{
    addLogMsg(QString("Sms status changed: msg id: %0, msg state: %1")
              .arg(messageId).arg((int)newStatus));
}

void TestSmppClientMainWindow::onConnected()
{
    addLogMsg(QString("Connected"));
}

void TestSmppClientMainWindow::onAuthenticated()
{
    addLogMsg(QString("Authenticated"));
}

void TestSmppClientMainWindow::onDisconnected(SmppClient::DisconnectReason disconnectReason)
{
    addLogMsg(QString("Disconnected, reason: %0").arg((int)disconnectReason));
}

void TestSmppClientMainWindow::onIncomingSms(const QString &from, const QString &to, const QString &smsText)
{
    addLogMsg(QString("Incoming sms from %0 to %1 with test: %2")
              .arg(from, to, smsText));
}

void TestSmppClientMainWindow::addLogMsg(const QString &msg)
{
    ui->textEdit->append(msg);
}

void TestSmppClientMainWindow::on_pushButtonSend_clicked()
{
    m_smppClient->sendSms(ui->lineEditMessageId->text(),
                          ui->lineEditRecepient->text(),
                          ui->lineEditPreferedSender->text(),
                          ui->lineEditSmsText->text());
}
