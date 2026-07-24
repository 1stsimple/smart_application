#include "add_device_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTcpSocket>
#include <QUrl>
#include <QVBoxLayout>

AddDeviceDialog::AddDeviceDialog(QWidget* parent)
    : QDialog(parent), testSocket_(new QTcpSocket(this)) {
    setWindowTitle(tr("添加摄像头设备"));
    setModal(true);
    resize(620, 560);

    id_ = new QLineEdit;
    id_->setPlaceholderText(tr("服务端唯一数字 ID"));
    name_ = new QLineEdit;
    name_->setPlaceholderText(tr("例如：客厅摄像头"));
    address_ = new QLineEdit;
    address_->setPlaceholderText(tr("IP 地址或域名"));
    port_ = new QLineEdit(QStringLiteral("554"));
    username_ = new QLineEdit;
    password_ = new QLineEdit;
    password_->setEchoMode(QLineEdit::Password);
    protocol_ = new QComboBox;
    protocol_->addItem(QStringLiteral("RTSP"), QStringLiteral("rtsp"));
    protocol_->addItem(QStringLiteral("RTMP"), QStringLiteral("rtmp"));
    deviceType_ = new QComboBox;
    deviceType_->addItem(tr("枪机（不支持云台）"), 0);
    deviceType_->addItem(tr("球机（支持云台）"), 1);
    channels_ = new QSpinBox;
    channels_->setRange(1, 128);
    primaryStream_ = new QLineEdit;
    primaryStream_->setPlaceholderText(tr("完整 URL 或路径；多通道可使用 {channel} 占位符"));
    secondaryStream_ = new QLineEdit;
    secondaryStream_->setPlaceholderText(tr("可选；当前服务端模型仅保存一种备用协议地址"));
    streamPreview_ = new QLabel;
    streamPreview_->setWordWrap(true);
    streamPreview_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    testStatus_ = new QLabel(tr("尚未测试"));
    testStatus_->setProperty("tone", "neutral");
    testButton_ = new QPushButton(tr("测试网络连接"));
    buttons_ = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    buttons_->button(QDialogButtonBox::Save)->setText(tr("保存到服务器"));
    buttons_->button(QDialogButtonBox::Save)->setProperty("role", "primary");

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(22, 20, 22, 20);
    root->setSpacing(12);
    QLabel* title = new QLabel(tr("添加摄像头"));
    title->setProperty("role", "appTitle");
    root->addWidget(title);
    QLabel* hint = new QLabel(tr("设备由服务器统一保存。账号和密码只用于构造媒体 URL，不会写入运行日志。"));
    hint->setProperty("role", "subtitle");
    hint->setWordWrap(true);
    root->addWidget(hint);

    QFormLayout* form = new QFormLayout;
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    form->setHorizontalSpacing(14);
    form->setVerticalSpacing(9);
    form->addRow(tr("设备 ID"), id_);
    form->addRow(tr("设备名称"), name_);
    form->addRow(tr("设备类型"), deviceType_);
    form->addRow(tr("通道数量"), channels_);
    form->addRow(tr("设备地址"), address_);
    form->addRow(tr("媒体端口"), port_);
    form->addRow(tr("设备用户名"), username_);
    form->addRow(tr("设备密码"), password_);
    form->addRow(tr("视频协议"), protocol_);
    form->addRow(tr("主码流"), primaryStream_);
    form->addRow(tr("备用码流"), secondaryStream_);
    form->addRow(tr("最终主码流"), streamPreview_);
    root->addLayout(form);

    QHBoxLayout* testRow = new QHBoxLayout;
    testRow->addWidget(testButton_);
    testRow->addWidget(testStatus_, 1);
    root->addLayout(testRow);
    root->addWidget(buttons_);

    connect(buttons_, &QDialogButtonBox::accepted, this, &AddDeviceDialog::validateAndAccept);
    connect(buttons_, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(testButton_, &QPushButton::clicked, this, &AddDeviceDialog::testConnection);
    connect(testSocket_, &QTcpSocket::connected, this, [this] {
        testStatus_->setText(tr("网络连接成功"));
        testStatus_->setProperty("tone", "success");
        testStatus_->style()->unpolish(testStatus_);
        testStatus_->style()->polish(testStatus_);
        testButton_->setEnabled(true);
        testSocket_->disconnectFromHost();
    });
    connect(testSocket_, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, [this](QAbstractSocket::SocketError) {
        testStatus_->setText(tr("连接失败：%1").arg(testSocket_->errorString()));
        testStatus_->setProperty("tone", "warning");
        testStatus_->style()->unpolish(testStatus_);
        testStatus_->style()->polish(testStatus_);
        testButton_->setEnabled(true);
    });
    connect(address_, &QLineEdit::textChanged, this, &AddDeviceDialog::updateStreamPreview);
    connect(port_, &QLineEdit::textChanged, this, &AddDeviceDialog::updateStreamPreview);
    connect(username_, &QLineEdit::textChanged, this, &AddDeviceDialog::updateStreamPreview);
    connect(password_, &QLineEdit::textChanged, this, &AddDeviceDialog::updateStreamPreview);
    connect(primaryStream_, &QLineEdit::textChanged, this, &AddDeviceDialog::updateStreamPreview);
    connect(protocol_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddDeviceDialog::updateStreamPreview);
    updateStreamPreview();
}

QString AddDeviceDialog::authenticatedUrl(const QString& raw) const {
    const QString text = raw.trimmed();
    if (text.isEmpty()) return QString();
    QUrl url(text);
    if (url.scheme().isEmpty()) {
        QString path = text;
        if (!path.startsWith('/')) path.prepend('/');
        url.setScheme(protocol_->currentData().toString());
        url.setHost(address_->text().trimmed());
        url.setPort(port_->text().toInt());
        url.setPath(path);
    }
    if (!username_->text().isEmpty()) url.setUserName(username_->text());
    if (!password_->text().isEmpty()) url.setPassword(password_->text());
    return url.toString(QUrl::FullyEncoded);
}

void AddDeviceDialog::updateStreamPreview() {
    const QString value = authenticatedUrl(primaryStream_->text());
    if (value.isEmpty()) {
        streamPreview_->setText(tr("请输入主码流 URL 或路径"));
        return;
    }
    QUrl display(value);
    if (!display.password().isEmpty()) display.setPassword(QStringLiteral("******"));
    streamPreview_->setText(display.toString(QUrl::FullyDecoded));
}

void AddDeviceDialog::testConnection() {
    const QString host = address_->text().trimmed();
    bool validPort = false;
    const int port = port_->text().toInt(&validPort);
    if (host.isEmpty() || !validPort || port < 1 || port > 65535) {
        QMessageBox::warning(this, tr("参数不完整"), tr("请填写有效的设备地址和端口。"));
        return;
    }
    testButton_->setEnabled(false);
    testStatus_->setText(tr("正在连接 %1:%2…").arg(host).arg(port));
    testSocket_->abort();
    testSocket_->connectToHost(host, quint16(port));
}

void AddDeviceDialog::validateAndAccept() {
    bool validId = false;
    const uint id = id_->text().toUInt(&validId);
    const QString url = authenticatedUrl(primaryStream_->text());
    if (!validId || id == 0 || name_->text().trimmed().isEmpty() ||
        address_->text().trimmed().isEmpty() || url.isEmpty()) {
        QMessageBox::warning(this, tr("无法保存"),
            tr("设备 ID、设备名称、设备地址和主码流均为必填项。"));
        return;
    }
    const QUrl parsed(url);
    if (!parsed.isValid() || (parsed.scheme() != QStringLiteral("rtsp") &&
                              parsed.scheme() != QStringLiteral("rtmp"))) {
        QMessageBox::warning(this, tr("无法保存"), tr("主码流必须是有效的 RTSP 或 RTMP 地址。"));
        return;
    }
    accept();
}

CameraDeviceDto AddDeviceDialog::device() const {
    CameraDeviceDto result;
    result.id = id_->text().toUInt();
    result.type = deviceType_->currentData().toUInt();
    result.channels = quint32(channels_->value());
    result.serialNumber = name_->text().trimmed();
    result.ip = address_->text().trimmed();
    const QString primary = authenticatedUrl(primaryStream_->text());
    const QString secondary = authenticatedUrl(secondaryStream_->text());
    if (protocol_->currentData().toString() == QStringLiteral("rtsp")) {
        result.rtspUrl = primary;
        result.rtmpUrl = secondary;
    } else {
        result.rtmpUrl = primary;
        result.rtspUrl = secondary;
    }
    return result;
}
