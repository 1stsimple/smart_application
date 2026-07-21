#include "main_window.h"
#include "video_decoder.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMediaContent>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPixmap>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>

namespace {
QString clientSettingsPath() {
    QString directory = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (directory.isEmpty()) directory = QCoreApplication::applicationDirPath();
    QDir().mkpath(directory);
    return QDir(directory).filePath("client.ini");
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), decoder_(new VideoDecoder), player_(this) {
    setWindowTitle(tr("智能家居监控客户端")); resize(1280, 820);
    QWidget* central = new QWidget(this); QVBoxLayout* root = new QVBoxLayout(central);

    QHBoxLayout* server = new QHBoxLayout;
    host_ = new QLineEdit("127.0.0.1"); port_ = new QLineEdit("8000");
    username_ = new QLineEdit; password_ = new QLineEdit; password_->setEchoMode(QLineEdit::Password);
    loadSettings();
    QPushButton* connectButton = new QPushButton(tr("连接"));
    QPushButton* registerButton = new QPushButton(tr("注册"));
    QPushButton* loginButton = new QPushButton(tr("登录"));
    server->addWidget(new QLabel(tr("服务器"))); server->addWidget(host_); server->addWidget(port_);
    server->addWidget(connectButton); server->addWidget(new QLabel(tr("用户")));
    server->addWidget(username_); server->addWidget(password_);
    server->addWidget(registerButton); server->addWidget(loginButton); root->addLayout(server);

    QSplitter* splitter = new QSplitter; QWidget* left = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(left); tree_ = new QTreeWidget;
    tree_->setHeaderLabels(QStringList() << tr("设备/通道") << tr("地址"));
    QPushButton* refresh = new QPushButton(tr("刷新设备"));
    leftLayout->addWidget(tree_); leftLayout->addWidget(refresh);
    QGroupBox* device = new QGroupBox(tr("保存摄像头")); QFormLayout* form = new QFormLayout(device);
    cameraId_ = new QLineEdit; cameraType_ = new QSpinBox; cameraType_->setRange(0, 1);
    channels_ = new QSpinBox; channels_->setRange(1, 128); serial_ = new QLineEdit;
    cameraIp_ = new QLineEdit; rtsp_ = new QLineEdit; rtmp_ = new QLineEdit;
    QPushButton* save = new QPushButton(tr("保存/更新"));
    form->addRow(tr("ID"), cameraId_); form->addRow(tr("类型(0枪机/1球机)"), cameraType_);
    form->addRow(tr("通道数"), channels_); form->addRow(tr("序列号"), serial_);
    form->addRow(tr("IP"), cameraIp_); form->addRow("RTSP", rtsp_); form->addRow("RTMP", rtmp_);
    form->addRow(save); leftLayout->addWidget(device); splitter->addWidget(left);

    QWidget* right = new QWidget; QVBoxLayout* rightLayout = new QVBoxLayout(right);
    videoStack_ = new QStackedWidget; liveView_ = new QLabel(tr("等待实时视频"));
    liveView_->setAlignment(Qt::AlignCenter); liveView_->setMinimumSize(640, 360);
    liveView_->setStyleSheet("background:#111;color:#ddd;"); playbackView_ = new QVideoWidget;
    videoStack_->addWidget(liveView_); videoStack_->addWidget(playbackView_); rightLayout->addWidget(videoStack_, 1);
    QHBoxLayout* actions = new QHBoxLayout;
    QPushButton* live = new QPushButton(tr("播放实时")); QPushButton* stop = new QPushButton(tr("停止"));
    QPushButton* record = new QPushButton(tr("开始录像")); QPushButton* stopRecord = new QPushButton(tr("停止录像"));
    actions->addWidget(live); actions->addWidget(stop); actions->addWidget(record); actions->addWidget(stopRecord);
    rightLayout->addLayout(actions);

    QGroupBox* ptz = new QGroupBox(tr("云台控制")); QGridLayout* ptzGrid = new QGridLayout(ptz);
    const QString commands[9] = {"left_up", "up", "right_up", "left", "stop", "right",
                                 "left_down", "down", "right_down"};
    for (int i = 0; i < 9; ++i) { const QString command = commands[i];
        QPushButton* button = new QPushButton(command);
        ptzGrid->addWidget(button, i / 3, i % 3); connect(button, &QPushButton::clicked, this,
            [this, command] { sendPtz(command); }); }
    QPushButton* zoomIn = new QPushButton("zoom_in"), *zoomOut = new QPushButton("zoom_out");
    speed_ = new QSpinBox; speed_->setRange(0, 100); speed_->setValue(50);
    ptzGrid->addWidget(zoomIn, 0, 3); ptzGrid->addWidget(zoomOut, 1, 3); ptzGrid->addWidget(speed_, 2, 3);
    connect(zoomIn, &QPushButton::clicked, this, [this] { sendPtz("zoom_in"); });
    connect(zoomOut, &QPushButton::clicked, this, [this] { sendPtz("zoom_out"); });
    rightLayout->addWidget(ptz);

    QHBoxLayout* playback = new QHBoxLayout; beginMs_ = new QLineEdit; endMs_ = new QLineEdit;
    const qint64 now = QDateTime::currentMSecsSinceEpoch(); beginMs_->setText(QString::number(now - 3600000));
    endMs_->setText(QString::number(now)); QPushButton* playbackButton = new QPushButton(tr("HLS 回放"));
    playback->addWidget(new QLabel(tr("开始ms"))); playback->addWidget(beginMs_);
    playback->addWidget(new QLabel(tr("结束ms"))); playback->addWidget(endMs_);
    playback->addWidget(playbackButton); rightLayout->addLayout(playback); splitter->addWidget(right);
    splitter->setStretchFactor(1, 1); root->addWidget(splitter, 1);
    logView_ = new QPlainTextEdit; logView_->setReadOnly(true); logView_->setMaximumBlockCount(500);
    logView_->setMaximumHeight(130); root->addWidget(logView_); setCentralWidget(central);
    log(tr("客户端配置：%1").arg(QDir::toNativeSeparators(clientSettingsPath())));

    decoder_->moveToThread(&decoderThread_); connect(&decoderThread_, &QThread::finished, decoder_, &QObject::deleteLater);
    connect(&protocol_, &ProtocolClient::videoMetadata, decoder_, &VideoDecoder::configure);
    connect(&protocol_, &ProtocolClient::compressedVideoPacket, decoder_, &VideoDecoder::decode);
    connect(decoder_, &VideoDecoder::frameReady, this, &MainWindow::showFrame);
    connect(decoder_, &VideoDecoder::decoderError, this, &MainWindow::log); decoderThread_.start();
    player_.setVideoOutput(playbackView_);

    connect(connectButton, &QPushButton::clicked, this, [this] {
        saveSettings();
        protocol_.connectToServer(host_->text(), quint16(port_->text().toUShort())); });
    connect(registerButton, &QPushButton::clicked, this, [this] {
        saveSettings();
        protocol_.registerUser(username_->text(), password_->text()); });
    connect(loginButton, &QPushButton::clicked, this, [this] {
        saveSettings();
        protocol_.login(username_->text(), password_->text()); });
    connect(refresh, &QPushButton::clicked, &protocol_, &ProtocolClient::requestCameras);
    connect(save, &QPushButton::clicked, this, &MainWindow::saveCamera);
    connect(live, &QPushButton::clicked, this, &MainWindow::playLive);
    connect(stop, &QPushButton::clicked, this, &MainWindow::stopLive);
    connect(record, &QPushButton::clicked, this, &MainWindow::startRecording);
    connect(stopRecord, &QPushButton::clicked, this, &MainWindow::stopRecording);
    connect(playbackButton, &QPushButton::clicked, this, &MainWindow::playRecording);
    connect(&protocol_, &ProtocolClient::camerasReceived, this, &MainWindow::showCameras);
    connect(&protocol_, &ProtocolClient::message, this, &MainWindow::log);
    connect(&protocol_, &ProtocolClient::protocolError, this, &MainWindow::log);
    connect(&protocol_, &ProtocolClient::connected, this, [this] { log(tr("服务器已连接")); });
    connect(&protocol_, &ProtocolClient::disconnected, this, [this] { log(tr("服务器已断开")); });
    connect(&protocol_, &ProtocolClient::playbackUrlReceived, this, [this](const QUrl& url) {
        videoStack_->setCurrentWidget(playbackView_); player_.setMedia(QMediaContent(url)); player_.play();
        log(tr("开始回放 %1").arg(url.toString())); });
}

MainWindow::~MainWindow() {
    saveSettings();
    protocol_.stopStream(); decoderThread_.quit(); decoderThread_.wait();
}

void MainWindow::loadSettings() {
    QSettings settings(clientSettingsPath(), QSettings::IniFormat);
    host_->setText(settings.value("connection/host", host_->text()).toString());
    port_->setText(settings.value("connection/port", port_->text()).toString());
    username_->setText(settings.value("authentication/username").toString());
    password_->setText(settings.value("authentication/password").toString());
}

void MainWindow::saveSettings() const {
    QSettings settings(clientSettingsPath(), QSettings::IniFormat);
    settings.setValue("connection/host", host_->text());
    settings.setValue("connection/port", port_->text());
    settings.setValue("authentication/username", username_->text());
    settings.setValue("authentication/password", password_->text());
    settings.sync();
}

void MainWindow::showCameras(const QVector<CameraDeviceDto>& cameras) {
    cameras_ = cameras; tree_->clear();
    for (int i = 0; i < cameras.size(); ++i) {
        QTreeWidgetItem* root = new QTreeWidgetItem(tree_, QStringList() <<
            QString("%1 [%2]").arg(cameras[i].serialNumber).arg(cameras[i].id) << cameras[i].ip);
        root->setData(0, Qt::UserRole, cameras[i].id);
        for (quint32 channel = 0; channel < cameras[i].channels; ++channel) {
            QTreeWidgetItem* item = new QTreeWidgetItem(root, QStringList() <<
                tr("通道 %1").arg(channel) << selectedStream(cameras[i]));
            item->setData(0, Qt::UserRole, cameras[i].id); item->setData(0, Qt::UserRole + 1, channel);
        }
        root->setExpanded(true);
    }
    log(tr("已加载 %1 台摄像头").arg(cameras.size()));
}
bool MainWindow::selection(CameraDeviceDto& camera, quint32& channel) const {
    QTreeWidgetItem* item = tree_->currentItem();
    if (!item || !item->parent()) return false;
    const quint32 id = item->data(0, Qt::UserRole).toUInt(); channel = item->data(0, Qt::UserRole + 1).toUInt();
    for (int i = 0; i < cameras_.size(); ++i) if (cameras_[i].id == id) { camera = cameras_[i]; return true; }
    return false;
}
QString MainWindow::selectedStream(const CameraDeviceDto& camera) const {
    return !camera.rtspUrl.isEmpty() ? camera.rtspUrl : camera.rtmpUrl;
}
void MainWindow::showFrame(const QImage& image) {
    videoStack_->setCurrentWidget(liveView_);
    liveView_->setPixmap(QPixmap::fromImage(image).scaled(liveView_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void MainWindow::playLive() { CameraDeviceDto camera; quint32 channel;
    if (!selection(camera, channel)) { log(tr("请先选择一个通道")); return; }
    player_.stop(); videoStack_->setCurrentWidget(liveView_); protocol_.startStream(camera.id, selectedStream(camera)); }
void MainWindow::stopLive() { protocol_.stopStream(); QMetaObject::invokeMethod(decoder_, "reset", Qt::QueuedConnection); }
void MainWindow::saveCamera() { CameraDeviceDto camera; camera.id = cameraId_->text().toUInt();
    camera.type = cameraType_->value(); camera.channels = channels_->value(); camera.serialNumber = serial_->text();
    camera.ip = cameraIp_->text(); camera.rtspUrl = rtsp_->text(); camera.rtmpUrl = rtmp_->text(); protocol_.saveCamera(camera); }
void MainWindow::startRecording() { CameraDeviceDto camera; quint32 channel;
    if (!selection(camera, channel)) { log(tr("请先选择一个通道")); return; }
    protocol_.startRecording(camera.id, channel, selectedStream(camera)); }
void MainWindow::stopRecording() { CameraDeviceDto camera; quint32 channel;
    if (!selection(camera, channel)) { log(tr("请先选择一个通道")); return; }
    protocol_.stopRecording(camera.id, channel); }
void MainWindow::playRecording() { CameraDeviceDto camera; quint32 channel;
    if (!selection(camera, channel)) { log(tr("请先选择一个通道")); return; }
    protocol_.requestPlayback(camera.id, channel, beginMs_->text().toLongLong(), endMs_->text().toLongLong()); }
void MainWindow::sendPtz(const QString& command) { CameraDeviceDto camera; quint32 channel;
    if (!selection(camera, channel)) { log(tr("请先选择一个通道")); return; } protocol_.ptz(channel, command, speed_->value()); }
void MainWindow::log(const QString& text) {
    logView_->appendPlainText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ") + text);
}
