#ifndef TESTSMPPSERVERMAINWINDOW_H
#define TESTSMPPSERVERMAINWINDOW_H

#include <QMainWindow>
#include "smpp-server/smppserver.h"
#include "smsgateway.h"

namespace Ui {
class TestSmppServerMainWindow;
}

class TestSmppServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestSmppServerMainWindow(QWidget *parent = 0);
    ~TestSmppServerMainWindow();

public slots:
    void onSendSmsRequested(
            const QString &messageId,
            const QString &preferedSender,
            const QString &recepinet,
            const QString &smsText);

private slots:
    void on_pushButtonIncomingSms_clicked();

    void on_pushButtonRefreshStatus_clicked();

private:
    void addMsgToLog(const QString &msg);

private:
    Ui::TestSmppServerMainWindow *ui;
    SmsGateway *m_smsGateway;
    SmppServer *m_smppServer;
};

#endif // TESTSMPPSERVERMAINWINDOW_H
