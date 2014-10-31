#include "testsmppservermainwindow.h"
#include "ui_testsmppservermainwindow.h"

TestSmppServerMainWindow::TestSmppServerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TestSmppServerMainWindow),
    m_smsGateway(new SmsGateway(this)),
    m_smppServer(new SmppServer(QHostAddress::Any, 8001, m_smsGateway, this))
{
    ui->setupUi(this);
    m_smppServer->addAuthenticationInfo("user", "pass");
    connect(m_smsGateway, SIGNAL(smsSended(QString,QString,QString,QString)),
            this, SLOT(onSendSmsRequested(QString,QString,QString,QString)));
}

TestSmppServerMainWindow::~TestSmppServerMainWindow()
{
    delete ui;
}

void TestSmppServerMainWindow::onSendSmsRequested(const QString &messageId, const QString &preferedSender, const QString &recepinet, const QString &smsText)
{
    addMsgToLog(QString("send sms requested: message id: %0, "
                        "pref sender: %1, recepinet: %2, smsText: %3")
                .arg(messageId, preferedSender, recepinet, smsText));
}


void TestSmppServerMainWindow::addMsgToLog(const QString &msg)
{
    ui->textEdit->append(msg);
}

void TestSmppServerMainWindow::on_pushButtonIncomingSms_clicked()
{
    m_smsGateway->fakeIncomingSms(ui->lineEditFrom->text(),
                                  ui->lineEditTo->text(),
                                  ui->lineEditSmsText->text());
}

void TestSmppServerMainWindow::on_pushButtonRefreshStatus_clicked()
{
    int smsStatus = ISmsGateway::Rejected;
    if (ui->radioButtonQueued->isChecked()) {
        smsStatus = ISmsGateway::Queued;
    } else if (ui->radioButtonErrorWhileSending->isChecked()) {
        smsStatus = ISmsGateway::ErrorWhileSending;
    } else if (ui->radioButtonDelivered->isChecked()) {
        smsStatus = ISmsGateway::Delivered;
    } else if (ui->radioButtonSended->isChecked()) {
        smsStatus = ISmsGateway::Sended;
    } else if (ui->radioButtonStartSending->isChecked()){
        smsStatus = ISmsGateway::StartSending;
    }
    m_smsGateway->fakeSmsStatusChanged(ui->lineEditMessageId->text(), smsStatus);
}
