#include "video_tile_widget.h"
#include "video_decoder.h"

#include <QHBoxLayout>
#include <QDateTime>
#include <QFileDialog>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QStyle>
#include <QSlider>
#include <QVBoxLayout>

VideoTileWidget::VideoTileWidget(const CameraDeviceDto& camera, quint32 channel,
                                 const QString& serverHost, quint16 serverPort,
                                 const QString& username, const QString& password,
                                 QWidget* parent)
    : QFrame(parent), camera_(camera), channel_(channel), host_(serverHost),
      username_(username), password_(password), port_(serverPort),
      client_(new ProtocolClient(this)), decoder_(new VideoDecoder),
      active_(false), stopping_(false), recording_(false) {
    setObjectName("videoTile");
    setProperty("active", false);
    setMinimumSize(260, 180);
    setFrameShape(QFrame::StyledPanel);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(1, 1, 1, 1);
    root->setSpacing(0);
    QFrame* header = new QFrame;
    header->setObjectName("videoTileHeader");
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(10, 6, 8, 6);
    title_ = new QLabel(tr("%1 / 通道 %2").arg(camera_.serialNumber).arg(channel_));
    title_->setProperty("role", "videoTitle");
    status_ = new QLabel(tr("等待连接"));
    status_->setProperty("role", "videoStatus");
    status_->setProperty("tone", "neutral");
    headerLayout->addWidget(title_, 1);
    headerLayout->addWidget(status_);
    root->addWidget(header);

    surface_ = new QLabel(tr("正在准备预览…"));
    surface_->setObjectName("videoTileSurface");
    surface_->setAlignment(Qt::AlignCenter);
    surface_->setProperty("state", "connecting");
    root->addWidget(surface_, 1);

    QFrame* footer = new QFrame;
    footer->setObjectName("videoTileFooter");
    QHBoxLayout* footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(8, 5, 8, 5);
    QPushButton* muteButton = new QPushButton(tr("🔇"));
    muteButton->setEnabled(false);
    muteButton->setToolTip(tr("当前实时协议只转发视频 AVPacket，音量控制等待音频协议扩展"));
    QSlider* volume = new QSlider(Qt::Horizontal);
    volume->setRange(0, 100);
    volume->setValue(70);
    volume->setMaximumWidth(90);
    volume->setEnabled(false);
    volume->setToolTip(muteButton->toolTip());
    QPushButton* snapshotButton = new QPushButton(tr("截图"));
    recordButton_ = new QPushButton(tr("开始录像"));
    fullScreenButton_ = new QPushButton(tr("全屏"));
    closeButton_ = new QPushButton(tr("关闭"));
    closeButton_->setProperty("role", "danger");
    footerLayout->addWidget(muteButton);
    footerLayout->addWidget(volume);
    footerLayout->addStretch();
    footerLayout->addWidget(snapshotButton);
    footerLayout->addWidget(recordButton_);
    footerLayout->addWidget(fullScreenButton_);
    footerLayout->addWidget(closeButton_);
    root->addWidget(footer);

    decoder_->moveToThread(&decoderThread_);
    connect(&decoderThread_, &QThread::finished, decoder_, &QObject::deleteLater);
    connect(client_, &ProtocolClient::videoMetadata, decoder_, &VideoDecoder::configure);
    connect(client_, &ProtocolClient::compressedVideoPacket, decoder_, &VideoDecoder::decode);
    connect(decoder_, &VideoDecoder::frameReady, this, [this](const QImage& image) {
        lastFrame_ = image;
        setPreviewState(tr("播放中"), "success");
        surface_->setText(QString());
        updatePixmap();
    });
    connect(decoder_, &VideoDecoder::decoderError, this, [this](const QString& error) {
        setPreviewState(tr("解码失败"), "warning");
        surface_->setText(tr("视频解码失败\n%1").arg(error));
        emit message(tr("%1 / 通道 %2：%3").arg(camera_.serialNumber).arg(channel_).arg(error));
    });
    connect(client_, &ProtocolClient::connected, this, [this] {
        setPreviewState(tr("正在认证"), "info");
        client_->login(username_, password_);
    });
    connect(client_, &ProtocolClient::authenticated, this, [this] {
        setPreviewState(tr("正在拉流"), "info");
        client_->startStream(camera_.id, streamUrl());
    });
    connect(client_, &ProtocolClient::protocolError, this, [this](const QString& error) {
        if (stopping_) return;
        setPreviewState(tr("播放失败"), "warning");
        surface_->setText(tr("无法播放此通道\n%1").arg(error));
        emit message(tr("%1 / 通道 %2：%3").arg(camera_.serialNumber).arg(channel_).arg(error));
    });
    connect(client_, &ProtocolClient::disconnected, this, [this] {
        if (!stopping_) {
            setPreviewState(tr("连接断开"), "warning");
            surface_->setText(tr("服务器连接已断开"));
        }
    });
    connect(recordButton_, &QPushButton::clicked, this, [this] {
        recordButton_->setEnabled(false);
        recordButton_->setText(recording_ ? tr("正在停止…") : tr("正在请求…"));
        emit recordingRequested(this, !recording_);
    });
    connect(snapshotButton, &QPushButton::clicked, this, [this] {
        if (lastFrame_.isNull()) {
            emit message(tr("%1 / 通道 %2：当前没有可截图的画面")
                             .arg(camera_.serialNumber).arg(channel_));
            return;
        }
        const QString suggested = QStringLiteral("%1_channel_%2_%3.png")
            .arg(camera_.serialNumber).arg(channel_)
            .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss")));
        const QString path = QFileDialog::getSaveFileName(
            this, tr("保存截图"), suggested, tr("PNG 图片 (*.png);;JPEG 图片 (*.jpg *.jpeg)"));
        if (path.isEmpty()) return;
        if (lastFrame_.save(path))
            emit message(tr("截图已保存：%1").arg(path));
        else
            emit message(tr("截图保存失败：%1").arg(path));
    });
    connect(fullScreenButton_, &QPushButton::clicked, this,
            [this] { emit fullScreenRequested(this); });
    connect(closeButton_, &QPushButton::clicked, this,
            [this] { emit closeRequested(this); });
    decoderThread_.start();
}

VideoTileWidget::~VideoTileWidget() {
    stopPreview();
    decoderThread_.quit();
    decoderThread_.wait();
}

QString VideoTileWidget::key() const {
    return QString::number(camera_.id) + QLatin1Char(':') + QString::number(channel_);
}

QString VideoTileWidget::streamUrl() const {
    QString url = !camera_.rtspUrl.isEmpty() ? camera_.rtspUrl : camera_.rtmpUrl;
    url.replace(QStringLiteral("{channel}"), QString::number(channel_), Qt::CaseInsensitive);
    url.replace(QStringLiteral("%CHANNEL%"), QString::number(channel_), Qt::CaseInsensitive);
    return url;
}

void VideoTileWidget::setActive(bool active) {
    if (active_ == active) return;
    active_ = active;
    setProperty("active", active);
    style()->unpolish(this);
    style()->polish(this);
}

void VideoTileWidget::startPreview() {
    if (client_->property("previewStarted").toBool()) return;
    client_->setProperty("previewStarted", true);
    stopping_ = false;
    setPreviewState(tr("正在连接"), "info");
    surface_->setText(tr("正在连接服务器…"));
    client_->connectToServer(host_, port_);
}

void VideoTileWidget::stopPreview() {
    if (stopping_) return;
    stopping_ = true;
    if (recording_) {
        recording_ = false;
        emit recordingRequested(this, false);
    }
    client_->stopStream();
    client_->disconnectFromServer();
    QMetaObject::invokeMethod(decoder_, "reset", Qt::QueuedConnection);
    lastFrame_ = QImage();
    surface_->clear();
    surface_->setText(tr("预览已停止"));
    setPreviewState(tr("已停止"), "neutral");
}

void VideoTileWidget::setRecordingResult(
    bool active, bool success, const QString& resultMessage) {
    if (success) recording_ = active;
    recordButton_->setEnabled(true);
    recordButton_->setText(recording_ ? tr("● 停止录像") : tr("开始录像"));
    recordButton_->setProperty("tone", recording_ ? "recording" : "normal");
    recordButton_->style()->unpolish(recordButton_);
    recordButton_->style()->polish(recordButton_);
    emit message(QStringLiteral("%1 / 通道 %2：%3")
        .arg(camera_.serialNumber).arg(channel_).arg(resultMessage));
}

void VideoTileWidget::setPreviewState(const QString& text, const char* tone) {
    status_->setText(text);
    status_->setProperty("tone", tone);
    status_->style()->unpolish(status_);
    status_->style()->polish(status_);
}

void VideoTileWidget::updatePixmap() {
    if (lastFrame_.isNull()) return;
    surface_->setPixmap(QPixmap::fromImage(lastFrame_).scaled(
        surface_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void VideoTileWidget::mousePressEvent(QMouseEvent* event) {
    emit activated(this);
    QFrame::mousePressEvent(event);
}

void VideoTileWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    emit activated(this);
    emit fullScreenRequested(this);
    QFrame::mouseDoubleClickEvent(event);
}

void VideoTileWidget::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    updatePixmap();
}
