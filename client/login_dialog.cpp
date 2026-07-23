#include "login_dialog.h"

#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QSettings>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

namespace {
QString loginSettingsPath() {
    QString directory = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (directory.isEmpty()) directory = QCoreApplication::applicationDirPath();
    QDir().mkpath(directory);
    return QDir(directory).filePath("client.ini");
}

QLabel* makeLoginLabel(const QString& text, const char* role) {
    QLabel* label = new QLabel(text);
    label->setProperty("role", role);
    return label;
}

class DragTitleBar : public QWidget {
public:
    explicit DragTitleBar(QWidget* parent = 0) : QWidget(parent), dragging_(false) {}
protected:
    void mousePressEvent(QMouseEvent* event) {
        if (event->button() == Qt::LeftButton) {
            dragging_ = true;
            offset_ = event->globalPos() - window()->frameGeometry().topLeft();
            event->accept();
            return;
        }
        QWidget::mousePressEvent(event);
    }
    void mouseMoveEvent(QMouseEvent* event) {
        if (dragging_ && (event->buttons() & Qt::LeftButton)) {
            window()->move(event->globalPos() - offset_);
            event->accept();
            return;
        }
        QWidget::mouseMoveEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent* event) {
        dragging_ = false;
        QWidget::mouseReleaseEvent(event);
    }
private:
    bool dragging_;
    QPoint offset_;
};
}

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent), client_(new ProtocolClient(this)), autoLoginTimer_(new QTimer(this)),
      pendingAction_(NoAction),
      connected_(false), busy_(false), authenticated_(false) {
    setWindowTitle(tr("登录智能家居监控系统"));
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    resize(520, 740);
    setMinimumSize(500, 700);

    setStyleSheet(QStringLiteral(R"QSS(
QDialog { background:transparent; }
QWidget { font-family:"Microsoft YaHei UI"; font-size:17px; color:#F4F7FB; }
QFrame#loginShell {
    background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #130C19,stop:0.48 #24122D,stop:1 #10233A);
    border:1px solid #3D2E48; border-radius:18px;
}
QWidget#loginBase, QWidget#loginPage, QWidget#registrationPage { background:transparent; }
QWidget#serverPanel { background:#211A2B; border:1px solid #493957; border-radius:10px; }
QLabel { background:transparent; border:none; }
QLabel[role="brand"] { color:#7DD3FC; font-size:17px; font-weight:700; letter-spacing:2px; }
QLabel[role="pageTitle"] { color:#FFFFFF; font-size:27px; font-weight:700; }
QLabel[role="pageSubtitle"] { color:#AFA4B8; font-size:17px; }
QLabel[role="status"] { color:#9FB1C1; font-size:17px; }
QLabel[role="status"][tone="success"] { color:#6EE7B7; }
QLabel[role="divider"] { color:#55445E; font-size:17px; }
QLabel[role="failureTitle"] { color:#FFFFFF; font-size:23px; font-weight:700; }
QLabel[role="failureMessage"] { color:#E6E8EC; font-size:17px; }
QLineEdit {
    min-height:40px; background:#17131D; color:#FFFFFF; border:1px solid #4A3B54;
    border-radius:9px; padding:8px 13px; selection-background-color:#147DE0;
}
QLineEdit:hover { border-color:#71607C; }
QLineEdit:focus { border:1px solid #198AE8; background:#131A25; }
QLineEdit:disabled { color:#716A78; background:#151219; border-color:#342C3B; }
QCheckBox { color:#B8AEC0; spacing:9px; }
QCheckBox::indicator { width:20px; height:20px; border:1px solid #66566F; border-radius:10px; background:#19141F; }
QCheckBox::indicator:hover { border-color:#349BF0; }
QCheckBox::indicator:checked { background:#147DE0; border-color:#147DE0; }
QPushButton {
    min-height:38px; border-radius:9px; padding:8px 16px; font-size:17px; font-weight:600;
    background:#25202B; color:#E9E4ED; border:1px solid #55495E;
}
QPushButton:hover { background:#302838; border-color:#75637F; }
QPushButton:pressed { background:#1A1720; padding-top:10px; padding-bottom:6px; }
QPushButton:disabled { background:#201B25; color:#6E6674; border-color:#392F40; }
QPushButton[role="primary"] {
    min-height:46px; background:#0877DB; color:#FFFFFF; border:1px solid #0877DB;
    border-radius:11px; font-size:20px; font-weight:700;
}
QPushButton[role="primary"]:hover { background:#1588EA; border-color:#1588EA; }
QPushButton[role="primary"]:pressed { background:#0568C1; border-color:#0568C1; }
QPushButton[role="link"] { min-height:28px; background:transparent; border:none; color:#2B98F4; padding:4px 7px; font-weight:500; }
QPushButton[role="link"]:hover { color:#73BEFA; text-decoration:underline; }
QPushButton[role="topAction"] { min-width:34px; min-height:32px; background:transparent; border:none; color:#A89EAE; padding:2px; font-size:22px; }
QPushButton[role="topAction"]:hover { background:#34263B; color:#FFFFFF; }
QPushButton[role="closeAction"]:hover { background:#B53D50; color:#FFFFFF; }
QWidget#failureOverlay { background:rgba(5, 7, 12, 205); border-radius:17px; }
QFrame#failureCard { background:#2B2B2E; border:1px solid #4D4D52; border-radius:13px; }
)QSS"));

    QVBoxLayout* outer = new QVBoxLayout(this);
    outer->setContentsMargins(10, 10, 10, 10);
    QFrame* shell = new QFrame;
    shell->setObjectName("loginShell");
    outer->addWidget(shell);

    QStackedLayout* layers = new QStackedLayout(shell);
    layers->setContentsMargins(0, 0, 0, 0);
    layers->setStackingMode(QStackedLayout::StackAll);

    QWidget* base = new QWidget;
    base->setObjectName("loginBase");
    QVBoxLayout* baseLayout = new QVBoxLayout(base);
    baseLayout->setContentsMargins(30, 16, 30, 30);
    baseLayout->setSpacing(13);

    DragTitleBar* titleBar = new DragTitleBar;
    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* brand = makeLoginLabel(tr("SMART HOME"), "brand");
    QPushButton* settingsButton = new QPushButton(tr("☰"));
    settingsButton->setProperty("role", "topAction");
    settingsButton->setToolTip(tr("服务器设置"));
    QPushButton* closeButton = new QPushButton(tr("×"));
    closeButton->setProperty("role", "topAction");
    closeButton->setProperty("class", "closeAction");
    closeButton->setStyleSheet(QStringLiteral(
        "QPushButton:hover{background:#B53D50;color:#FFFFFF;}"));
    closeButton->setToolTip(tr("退出"));
    titleLayout->addWidget(brand);
    titleLayout->addStretch();
    titleLayout->addWidget(settingsButton);
    titleLayout->addWidget(closeButton);
    baseLayout->addWidget(titleBar);

    QLabel* logo = new QLabel;
    logo->setFixedSize(112, 112);
    logo->setAlignment(Qt::AlignCenter);
    logo->setPixmap(QPixmap(QStringLiteral(":/branding/app_icon.png")).scaled(
        108, 108, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    baseLayout->addWidget(logo, 0, Qt::AlignHCenter);

    pageTitle_ = makeLoginLabel(tr("欢迎回来"), "pageTitle");
    pageTitle_->setAlignment(Qt::AlignCenter);
    pageSubtitle_ = makeLoginLabel(tr("登录后进入智能家居监控面板"), "pageSubtitle");
    pageSubtitle_->setAlignment(Qt::AlignCenter);
    baseLayout->addWidget(pageTitle_);
    baseLayout->addWidget(pageSubtitle_);

    serverPanel_ = new QWidget;
    serverPanel_->setObjectName("serverPanel");
    QFormLayout* serverLayout = new QFormLayout(serverPanel_);
    serverLayout->setContentsMargins(14, 12, 14, 12);
    serverLayout->setHorizontalSpacing(10);
    serverLayout->setVerticalSpacing(9);
    host_ = new QLineEdit;
    host_->setPlaceholderText(tr("服务器地址"));
    port_ = new QLineEdit;
    port_->setPlaceholderText(tr("端口"));
    serverLayout->addRow(tr("服务器"), host_);
    serverLayout->addRow(tr("端口"), port_);
    serverPanel_->hide();
    baseLayout->addWidget(serverPanel_);

    forms_ = new QStackedWidget;
    forms_->setObjectName("loginForms");
    forms_->setStyleSheet(QStringLiteral("QStackedWidget#loginForms{background:transparent;border:none;}"));

    QWidget* loginPage = new QWidget;
    loginPage->setObjectName("loginPage");
    QVBoxLayout* loginLayout = new QVBoxLayout(loginPage);
    loginLayout->setContentsMargins(4, 5, 4, 0);
    loginLayout->setSpacing(12);
    username_ = new QLineEdit;
    username_->setPlaceholderText(tr("账号"));
    username_->setClearButtonEnabled(true);
    password_ = new QLineEdit;
    password_->setPlaceholderText(tr("密码"));
    password_->setEchoMode(QLineEdit::Password);
    autoLogin_ = new QCheckBox(tr("自动登录"));
    cancelAutoLoginButton_ = new QPushButton(tr("取消自动登录"));
    cancelAutoLoginButton_->setProperty("role", "link");
    cancelAutoLoginButton_->hide();
    loginButton_ = new QPushButton(tr("登录"));
    loginButton_->setProperty("role", "primary");
    loginLayout->addWidget(username_);
    loginLayout->addWidget(password_);
    QHBoxLayout* autoLoginLayout = new QHBoxLayout;
    autoLoginLayout->setContentsMargins(0, 0, 0, 0);
    autoLoginLayout->addWidget(autoLogin_);
    autoLoginLayout->addStretch();
    autoLoginLayout->addWidget(cancelAutoLoginButton_);
    loginLayout->addLayout(autoLoginLayout);
    loginLayout->addSpacing(4);
    loginLayout->addWidget(loginButton_);
    loginLayout->addStretch();
    QHBoxLayout* loginLinks = new QHBoxLayout;
    QPushButton* recoveryButton = new QPushButton(tr("找回密码"));
    recoveryButton->setProperty("role", "link");
    QPushButton* showRegistrationButton = new QPushButton(tr("注册账号"));
    showRegistrationButton->setProperty("role", "link");
    loginLinks->addStretch();
    loginLinks->addWidget(recoveryButton);
    loginLinks->addWidget(makeLoginLabel(QStringLiteral("|"), "divider"));
    loginLinks->addWidget(showRegistrationButton);
    loginLinks->addStretch();
    loginLayout->addLayout(loginLinks);
    forms_->addWidget(loginPage);

    QWidget* registrationPage = new QWidget;
    registrationPage->setObjectName("registrationPage");
    QVBoxLayout* registrationLayout = new QVBoxLayout(registrationPage);
    registrationLayout->setContentsMargins(4, 5, 4, 0);
    registrationLayout->setSpacing(12);
    registerUsername_ = new QLineEdit;
    registerUsername_->setPlaceholderText(tr("设置账号"));
    registerUsername_->setClearButtonEnabled(true);
    registerPassword_ = new QLineEdit;
    registerPassword_->setPlaceholderText(tr("设置密码"));
    registerPassword_->setEchoMode(QLineEdit::Password);
    registerConfirmation_ = new QLineEdit;
    registerConfirmation_->setPlaceholderText(tr("再次输入密码"));
    registerConfirmation_->setEchoMode(QLineEdit::Password);
    registrationButton_ = new QPushButton(tr("注册并返回登录"));
    registrationButton_->setProperty("role", "primary");
    QPushButton* backToLoginButton = new QPushButton(tr("已有账号，返回登录"));
    backToLoginButton->setProperty("role", "link");
    registrationLayout->addWidget(registerUsername_);
    registrationLayout->addWidget(registerPassword_);
    registrationLayout->addWidget(registerConfirmation_);
    registrationLayout->addSpacing(4);
    registrationLayout->addWidget(registrationButton_);
    registrationLayout->addStretch();
    registrationLayout->addWidget(backToLoginButton, 0, Qt::AlignHCenter);
    forms_->addWidget(registrationPage);
    baseLayout->addWidget(forms_, 1);

    statusLabel_ = makeLoginLabel(tr("请输入账号和密码"), "status");
    statusLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setWordWrap(true);
    baseLayout->addWidget(statusLabel_);
    layers->addWidget(base);

    failureOverlay_ = new QWidget;
    failureOverlay_->setObjectName("failureOverlay");
    QVBoxLayout* failureOverlayLayout = new QVBoxLayout(failureOverlay_);
    failureOverlayLayout->setContentsMargins(28, 28, 28, 28);
    failureOverlayLayout->addStretch();
    QFrame* failureCard = new QFrame;
    failureCard->setObjectName("failureCard");
    failureCard->setMaximumWidth(430);
    QVBoxLayout* failureCardLayout = new QVBoxLayout(failureCard);
    failureCardLayout->setContentsMargins(28, 24, 28, 24);
    failureCardLayout->setSpacing(16);
    QHBoxLayout* failureHeader = new QHBoxLayout;
    failureTitle_ = makeLoginLabel(tr("登录失败"), "failureTitle");
    QPushButton* dismissFailure = new QPushButton(tr("×"));
    dismissFailure->setProperty("role", "topAction");
    failureHeader->addWidget(failureTitle_);
    failureHeader->addStretch();
    failureHeader->addWidget(dismissFailure);
    failureCardLayout->addLayout(failureHeader);
    failureMessage_ = makeLoginLabel(tr("账号或密码错误，请重新输入。"), "failureMessage");
    failureMessage_->setWordWrap(true);
    failureCardLayout->addWidget(failureMessage_);
    QHBoxLayout* failureButtons = new QHBoxLayout;
    QPushButton* failureRecovery = new QPushButton(tr("忘记密码"));
    failureRecovery->setProperty("role", "primary");
    QPushButton* retryButton = new QPushButton(tr("重新登录"));
    failureButtons->addStretch();
    failureButtons->addWidget(failureRecovery);
    failureButtons->addWidget(retryButton);
    failureCardLayout->addLayout(failureButtons);
    failureOverlayLayout->addWidget(failureCard, 0, Qt::AlignCenter);
    failureOverlayLayout->addStretch();
    layers->addWidget(failureOverlay_);
    failureOverlay_->hide();

    loadSettings();

    connect(closeButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(settingsButton, &QPushButton::clicked, this, &LoginDialog::toggleServerSettings);
    connect(loginButton_, &QPushButton::clicked, this, &LoginDialog::beginLogin);
    connect(password_, &QLineEdit::returnPressed, this, &LoginDialog::beginLogin);
    connect(cancelAutoLoginButton_, &QPushButton::clicked, this, [this] {
        if (autoLogin_->isChecked())
            autoLogin_->setChecked(false);
        else
            cancelPendingAutoLogin();
    });
    connect(autoLogin_, &QCheckBox::toggled, this, [this](bool checked) {
        if (!checked) {
            cancelPendingAutoLogin();
            saveSettings();
        }
    });
    connect(username_, &QLineEdit::textEdited, this, [this] {
        if (autoLoginTimer_->isActive()) autoLogin_->setChecked(false);
    });
    connect(password_, &QLineEdit::textEdited, this, [this] {
        if (autoLoginTimer_->isActive()) autoLogin_->setChecked(false);
    });
    connect(registrationButton_, &QPushButton::clicked, this, &LoginDialog::beginRegistration);
    connect(registerConfirmation_, &QLineEdit::returnPressed, this, &LoginDialog::beginRegistration);
    connect(showRegistrationButton, &QPushButton::clicked, this, &LoginDialog::showRegistrationPage);
    connect(backToLoginButton, &QPushButton::clicked, this, &LoginDialog::showLoginPage);
    connect(recoveryButton, &QPushButton::clicked, this, &LoginDialog::showRecoveryInformation);
    connect(failureRecovery, &QPushButton::clicked, this, &LoginDialog::showRecoveryInformation);
    connect(dismissFailure, &QPushButton::clicked, this, &LoginDialog::hideFailure);
    connect(retryButton, &QPushButton::clicked, this, [this] {
        hideFailure();
        setBusy(false, tr("请重新输入账号和密码"));
        password_->clear();
        password_->setFocus();
    });

    connect(client_, &ProtocolClient::connected, this, [this] {
        connected_ = true;
        resumePendingAction();
    });
    connect(client_, &ProtocolClient::disconnected, this, [this] {
        connected_ = false;
        if (busy_) handleAuthenticationError(tr("服务器连接已断开，请检查地址和网络后重试。"));
    });
    connect(client_, &ProtocolClient::protocolError,
            this, &LoginDialog::handleAuthenticationError);
    connect(client_, &ProtocolClient::registrationFinished,
            this, &LoginDialog::handleRegistrationFinished);
    connect(client_, &ProtocolClient::authenticated,
            this, &LoginDialog::handleAuthenticated);

    autoLoginTimer_->setSingleShot(true);
    autoLoginTimer_->setInterval(1200);
    connect(autoLoginTimer_, &QTimer::timeout, this, [this] {
        cancelAutoLoginButton_->hide();
        autoLogin_->setText(tr("自动登录"));
        if (autoLogin_->isChecked() && !username_->text().isEmpty() && !password_->text().isEmpty()) {
            statusLabel_->setText(tr("正在自动登录…"));
            beginLogin();
        } else {
            username_->setFocus();
        }
    });
    if (autoLogin_->isChecked() && !username_->text().isEmpty() && !password_->text().isEmpty()) {
        autoLogin_->setText(tr("自动登录（约 1 秒后）"));
        cancelAutoLoginButton_->show();
        statusLabel_->setText(tr("即将自动登录，可取消后切换账号"));
        QTimer::singleShot(0, this, [this] { autoLoginTimer_->start(); });
    } else {
        username_->setFocus();
    }
}

ProtocolClient* LoginDialog::takeAuthenticatedClient() {
    if (!authenticated_ || !client_) return 0;
    client_->setParent(0);
    ProtocolClient* result = client_;
    client_ = 0;
    return result;
}

void LoginDialog::loadSettings() {
    QSettings settings(loginSettingsPath(), QSettings::IniFormat);
    host_->setText(settings.value("connection/host", "127.0.0.1").toString());
    port_->setText(settings.value("connection/port", "8000").toString());
    username_->setText(settings.value("authentication/username").toString());
    password_->setText(settings.value("authentication/password").toString());
    autoLogin_->setChecked(settings.value("connection/autoLogin", false).toBool());
}

void LoginDialog::saveSettings() const {
    QSettings settings(loginSettingsPath(), QSettings::IniFormat);
    settings.setValue("connection/host", host_->text().trimmed());
    settings.setValue("connection/port", port_->text().trimmed());
    settings.setValue("authentication/username", username_->text().trimmed());
    settings.setValue("authentication/password",
                      autoLogin_->isChecked() ? password_->text() : QString());
    settings.setValue("connection/autoLogin", autoLogin_->isChecked());
    settings.sync();
}

bool LoginDialog::validateServer() {
    bool portOk = false;
    const uint port = port_->text().trimmed().toUInt(&portOk);
    if (host_->text().trimmed().isEmpty() || !portOk || port == 0 || port > 65535) {
        showFailure(tr("连接设置错误"), tr("请输入有效的服务器地址和 1～65535 之间的端口。"));
        return false;
    }
    return true;
}

void LoginDialog::beginLogin() {
    if (busy_) return;
    if (autoLoginTimer_->isActive()) {
        autoLoginTimer_->stop();
        cancelAutoLoginButton_->hide();
        autoLogin_->setText(tr("自动登录"));
    }
    hideFailure();
    if (!validateServer()) return;
    if (username_->text().trimmed().isEmpty() || password_->text().isEmpty()) {
        showFailure(tr("无法登录"), tr("账号和密码不能为空，请完整输入后重试。"));
        return;
    }
    pendingAction_ = LoginAction;
    setBusy(true, connected_ ? tr("正在验证账号…") : tr("正在连接服务器…"));
    if (connected_) resumePendingAction();
    else client_->connectToServer(host_->text().trimmed(), quint16(port_->text().toUShort()));
}

void LoginDialog::beginRegistration() {
    if (busy_) return;
    hideFailure();
    if (!validateServer()) return;
    if (registerUsername_->text().trimmed().isEmpty() || registerPassword_->text().isEmpty()) {
        showFailure(tr("无法注册"), tr("注册账号和密码不能为空。"));
        return;
    }
    if (registerPassword_->text() != registerConfirmation_->text()) {
        showFailure(tr("无法注册"), tr("两次输入的密码不一致，请重新确认。"));
        return;
    }
    pendingAction_ = RegistrationAction;
    setBusy(true, connected_ ? tr("正在注册账号…") : tr("正在连接服务器…"));
    if (connected_) resumePendingAction();
    else client_->connectToServer(host_->text().trimmed(), quint16(port_->text().toUShort()));
}

void LoginDialog::resumePendingAction() {
    if (!client_ || !connected_) return;
    if (pendingAction_ == LoginAction) {
        statusLabel_->setText(tr("正在验证账号…"));
        client_->login(username_->text().trimmed(), password_->text());
    } else if (pendingAction_ == RegistrationAction) {
        statusLabel_->setText(tr("正在创建账号…"));
        client_->registerUser(registerUsername_->text().trimmed(), registerPassword_->text());
    }
}

void LoginDialog::handleAuthenticationError(const QString& message) {
    if (pendingAction_ == NoAction && !busy_) return;
    const bool registration = pendingAction_ == RegistrationAction;
    pendingAction_ = NoAction;
    setBusy(false);
    showFailure(registration ? tr("注册失败") : tr("登录失败"),
                message.trimmed().isEmpty()
                    ? tr("账号或密码错误，请重新输入或找回密码。")
                    : message.trimmed());
}

void LoginDialog::handleRegistrationFinished() {
    pendingAction_ = NoAction;
    setBusy(false);
    username_->setText(registerUsername_->text().trimmed());
    password_->setText(registerPassword_->text());
    registerPassword_->clear();
    registerConfirmation_->clear();
    showLoginPage();
    statusLabel_->setProperty("tone", "success");
    statusLabel_->setText(tr("注册成功，请使用新账号登录"));
    statusLabel_->style()->unpolish(statusLabel_);
    statusLabel_->style()->polish(statusLabel_);
    password_->setFocus();
}

void LoginDialog::handleAuthenticated() {
    pendingAction_ = NoAction;
    setBusy(false, tr("登录成功，正在进入监控面板…"));
    saveSettings();
    authenticated_ = true;
    accept();
}

void LoginDialog::showLoginPage() {
    hideFailure();
    forms_->setCurrentIndex(0);
    pageTitle_->setText(tr("欢迎回来"));
    pageSubtitle_->setText(tr("登录后进入智能家居监控面板"));
    statusLabel_->setProperty("tone", "neutral");
    statusLabel_->setText(tr("请输入账号和密码"));
    statusLabel_->style()->unpolish(statusLabel_);
    statusLabel_->style()->polish(statusLabel_);
    username_->setFocus();
}

void LoginDialog::showRegistrationPage() {
    if (autoLoginTimer_->isActive()) {
        autoLoginTimer_->stop();
        cancelAutoLoginButton_->hide();
        autoLogin_->setText(tr("自动登录"));
    }
    hideFailure();
    forms_->setCurrentIndex(1);
    pageTitle_->setText(tr("创建账号"));
    pageSubtitle_->setText(tr("注册完成后返回登录"));
    statusLabel_->setProperty("tone", "neutral");
    statusLabel_->setText(tr("请设置账号和密码"));
    statusLabel_->style()->unpolish(statusLabel_);
    statusLabel_->style()->polish(statusLabel_);
    registerUsername_->setFocus();
}

void LoginDialog::toggleServerSettings() {
    serverPanel_->setVisible(!serverPanel_->isVisible());
    if (serverPanel_->isVisible()) host_->setFocus();
}

void LoginDialog::showRecoveryInformation() {
    QMessageBox::information(this, tr("找回密码"),
        tr("当前服务端未提供在线密码找回接口。\n请联系系统管理员重置账号密码。"));
}

void LoginDialog::cancelPendingAutoLogin() {
    const bool wasPending = autoLoginTimer_->isActive() || cancelAutoLoginButton_->isVisible();
    autoLoginTimer_->stop();
    cancelAutoLoginButton_->hide();
    autoLogin_->setText(tr("自动登录"));
    if (wasPending) {
        statusLabel_->setProperty("tone", "neutral");
        statusLabel_->setText(tr("已取消自动登录，可修改账号后手动登录"));
        statusLabel_->style()->unpolish(statusLabel_);
        statusLabel_->style()->polish(statusLabel_);
        username_->setFocus();
    }
}

void LoginDialog::setBusy(bool busy, const QString& message) {
    busy_ = busy;
    loginButton_->setDisabled(busy);
    registrationButton_->setDisabled(busy);
    host_->setDisabled(busy);
    port_->setDisabled(busy);
    username_->setDisabled(busy);
    password_->setDisabled(busy);
    registerUsername_->setDisabled(busy);
    registerPassword_->setDisabled(busy);
    registerConfirmation_->setDisabled(busy);
    if (!message.isEmpty()) statusLabel_->setText(message);
}

void LoginDialog::showFailure(const QString& title, const QString& message) {
    failureTitle_->setText(title);
    failureMessage_->setText(message);
    failureOverlay_->show();
    failureOverlay_->raise();
}

void LoginDialog::hideFailure() {
    failureOverlay_->hide();
}
