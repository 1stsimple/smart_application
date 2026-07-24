#include "video_grid_widget.h"
#include "video_tile_widget.h"

#include <QGridLayout>
#include <QLabel>

VideoGridWidget::VideoGridWidget(QWidget* parent)
    : QWidget(parent), grid_(new QGridLayout(this)), active_(0) {
    setObjectName("videoGrid");
    grid_->setContentsMargins(0, 0, 0, 0);
    grid_->setHorizontalSpacing(7);
    grid_->setVerticalSpacing(7);
    emptyState_ = new QLabel(tr("◉\n\n尚未开始预览\n双击左侧设备通道，最多可同时打开 9 路"));
    emptyState_->setObjectName("videoGridEmptyState");
    emptyState_->setAlignment(Qt::AlignCenter);
    grid_->addWidget(emptyState_, 0, 0);
}

QString VideoGridWidget::key(quint32 cameraId, quint32 channel) const {
    return QString::number(cameraId) + QLatin1Char(':') + QString::number(channel);
}

VideoTileWidget* VideoGridWidget::tile(quint32 cameraId, quint32 channel) const {
    return tiles_.value(key(cameraId, channel), 0);
}

bool VideoGridWidget::addPreview(const CameraDeviceDto& camera, quint32 channel,
                                 const QString& host, quint16 port,
                                 const QString& username, const QString& password) {
    const QString tileKey = key(camera.id, channel);
    if (tiles_.contains(tileKey)) {
        setActiveTile(tiles_.value(tileKey));
        emit message(tr("%1 / 通道 %2 已经在预览").arg(camera.serialNumber).arg(channel));
        return false;
    }
    if (tiles_.size() >= 9) {
        emit message(tr("最多同时预览 9 路视频，请先关闭一路"));
        return false;
    }
    VideoTileWidget* tile = new VideoTileWidget(
        camera, channel, host, port, username, password, this);
    tiles_.insert(tileKey, tile);
    order_.append(tile);
    connect(tile, &VideoTileWidget::activated, this, &VideoGridWidget::setActiveTile);
    connect(tile, &VideoTileWidget::closeRequested, this,
            QOverload<VideoTileWidget*>::of(&VideoGridWidget::removePreview));
    connect(tile, &VideoTileWidget::recordingRequested,
            this, &VideoGridWidget::recordingRequested);
    connect(tile, &VideoTileWidget::fullScreenRequested,
            this, &VideoGridWidget::fullScreenRequested);
    connect(tile, &VideoTileWidget::message, this, &VideoGridWidget::message);
    emptyState_->hide();
    relayout();
    setActiveTile(tile);
    emit previewCountChanged(tiles_.size());
    emit previewStateChanged(camera.id, channel, true);
    tile->startPreview();
    return true;
}

void VideoGridWidget::removePreview(VideoTileWidget* tile) {
    if (!tile || !tiles_.contains(tile->key())) return;
    const quint32 cameraId = tile->cameraId();
    const quint32 channel = tile->channel();
    emit tileAboutToRemove(tile);
    const bool wasActive = active_ == tile;
    tiles_.remove(tile->key());
    order_.removeAll(tile);
    grid_->removeWidget(tile);
    tile->stopPreview();
    tile->deleteLater();
    if (wasActive) {
        active_ = 0;
        if (!order_.isEmpty()) setActiveTile(order_.first());
        else emit activeTileChanged(0);
    }
    relayout();
    emptyState_->setVisible(tiles_.isEmpty());
    emit previewCountChanged(tiles_.size());
    emit previewStateChanged(cameraId, channel, false);
}

void VideoGridWidget::removePreview(quint32 cameraId, quint32 channel) {
    removePreview(tiles_.value(key(cameraId, channel), 0));
}

void VideoGridWidget::removeCamera(quint32 cameraId) {
    const QList<VideoTileWidget*> values = order_;
    for (int i = 0; i < values.size(); ++i)
        if (values[i]->cameraId() == cameraId) removePreview(values[i]);
}

void VideoGridWidget::stopAll() {
    const QList<VideoTileWidget*> values = order_;
    tiles_.clear();
    order_.clear();
    active_ = 0;
    for (int i = 0; i < values.size(); ++i) {
        grid_->removeWidget(values[i]);
        values[i]->stopPreview();
        delete values[i];
    }
    emptyState_->show();
    grid_->addWidget(emptyState_, 0, 0);
    emit activeTileChanged(0);
    emit previewCountChanged(0);
}

void VideoGridWidget::showOnly(VideoTileWidget* tile) {
    const QList<VideoTileWidget*> values = order_;
    for (int i = 0; i < values.size(); ++i)
        values[i]->setVisible(values[i] == tile);
    if (tile) {
        grid_->removeWidget(tile);
        grid_->addWidget(tile, 0, 0);
        setActiveTile(tile);
    }
}

void VideoGridWidget::showAll() {
    const QList<VideoTileWidget*> values = order_;
    for (int i = 0; i < values.size(); ++i) values[i]->show();
    relayout();
}

void VideoGridWidget::setActiveTile(VideoTileWidget* tile) {
    if (active_ == tile) return;
    if (active_) active_->setActive(false);
    active_ = tile;
    if (active_) active_->setActive(true);
    emit activeTileChanged(active_);
}

void VideoGridWidget::relayout() {
    const QList<VideoTileWidget*> values = order_;
    for (int i = 0; i < values.size(); ++i) grid_->removeWidget(values[i]);
    const int count = values.size();
    int columns = 1;
    if (count == 2) columns = 2;
    else if (count <= 4) columns = 2;
    else if (count <= 9) columns = 3;
    for (int i = 0; i < values.size(); ++i)
        grid_->addWidget(values[i], i / columns, i % columns);
    const int rows = count ? ((count + columns - 1) / columns) : 1;
    for (int row = 0; row < 3; ++row) grid_->setRowStretch(row, row < rows ? 1 : 0);
    for (int column = 0; column < 3; ++column)
        grid_->setColumnStretch(column, column < columns ? 1 : 0);
}
