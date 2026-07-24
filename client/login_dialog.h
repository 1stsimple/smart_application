#ifndef SMART_HOME_CLIENT_LOGIN_DIALOG_H
#define SMART_HOME_CLIENT_LOGIN_DIALOG_H

#include "protocol_client.h"

#include <QDialog>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTimer;
class QWidget;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = 0);
    ProtocolClient* takeAuthenticatedClient();
    QString authenticatedHost() const;
    quint16 authenticatedPort() const;
    QString authenticatedUsername() const;
    QString authenticatedPassword() const;

private slots:
    void beginLogin();
    void beginRegistration();
    void showLoginPage();
    void showRegistrationPage();
    void toggleServerSettings();
    void resumePendingAction();
    void handleAuthenticationError(const QString& message);
    void handleRegistrationFinished();
    void handleAuthenticated();
    void showRecoveryInformation();
    void cancelPendingAutoLogin();

private:
    enum PendingAction { NoAction, LoginAction, RegistrationAction };

    void loadSettings();
    void saveSettings() const;
    void setBusy(bool busy, const QString& message = QString());
    void showFailure(const QString& title, const QString& message);
    void hideFailure();
    bool validateServer();

    ProtocolClient* client_;
    QTimer* autoLoginTimer_;
    QLineEdit *host_, *port_;
    QLineEdit *username_, *password_;
    QLineEdit *registerUsername_, *registerPassword_, *registerConfirmation_;
    QCheckBox* autoLogin_;
    QPushButton *loginButton_, *registrationButton_, *cancelAutoLoginButton_;
    QLabel *pageTitle_, *pageSubtitle_, *statusLabel_;
    QLabel *failureTitle_, *failureMessage_;
    QStackedWidget* forms_;
    QWidget *serverPanel_, *failureOverlay_;
    PendingAction pendingAction_;
    bool connected_;
    bool busy_;
    bool authenticated_;
};

#endif
