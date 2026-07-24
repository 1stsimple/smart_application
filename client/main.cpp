#include "login_dialog.h"
#include "main_window.h"

#include <QApplication>
#include <QIcon>
#include <QStyleFactory>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    application.setStyle(QStyleFactory::create("Fusion"));
    application.setWindowIcon(QIcon(QStringLiteral(":/branding/app_icon.png")));
    QCoreApplication::setOrganizationName("SmartHomeMonitor");
    QCoreApplication::setApplicationName("SmartHomeMonitorClient");
    application.setQuitOnLastWindowClosed(false);
    LoginDialog login;
    if (login.exec() != QDialog::Accepted) return 0;
    const QString host = login.authenticatedHost();
    const quint16 port = login.authenticatedPort();
    const QString username = login.authenticatedUsername();
    const QString password = login.authenticatedPassword();
    ProtocolClient* authenticatedClient = login.takeAuthenticatedClient();
    if (!authenticatedClient) return 1;
    MainWindow window(authenticatedClient, host, port, username, password);
    application.setQuitOnLastWindowClosed(true);
    window.show();
    return application.exec();
}
