#ifndef TESTSMPPCLIENTMAINWINDOW_H
#define TESTSMPPCLIENTMAINWINDOW_H

#include <QMainWindow>
#include "smpp-client/smppclient.h"

namespace Ui {
class TestSmppClientMainWindow;
}

class TestSmppClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestSmppClientMainWindow(QWidget *parent = 0);
    ~TestSmppClientMainWindow();

private slots:
    void onSmsStatusChanged(const QString &messageId, SmppClient::SmsStates newStatus);
    void onConnected();
    void onAuthenticated();
    void onDisconnected(SmppClient::DisconnectReason disconnectReason);
    void onIncomingSms(const QString &from, const QString &to, const QString &smsText);

    void on_pushButtonSend_clicked();

private:
    void addLogMsg(const QString &msg);

private:
    Ui::TestSmppClientMainWindow *ui;
    SmppClient *m_smppClient;
};

#endif // TESTSMPPCLIENTMAINWINDOW_H
