#ifndef SMART_HOME_CLIENT_VIDEO_GRID_WIDGET_H
#define SMART_HOME_CLIENT_VIDEO_GRID_WIDGET_H

#include "protocol_client.h"

#include <QHash>
#include <QList>
#include <QWidget>

class QLabel;
class QGridLayout;
class VideoTileWidget;

class VideoGridWidget : public QWidget {
    Q_OBJECT
public:
    explicit VideoGridWidget(QWidget* parent = 0);
    bool addPreview(const CameraDeviceDto& camera, quint32 channel,
                    const QString& host, quint16 port,
                    const QString& username, const QString& password);
    void removePreview(VideoTileWidget* tile);
    void removePreview(quint32 cameraId, quint32 channel);
    void removeCamera(quint32 cameraId);
    void stopAll();
    void showOnly(VideoTileWidget* tile);
    void showAll();
    VideoTileWidget* activeTile() const { return active_; }
    VideoTileWidget* tile(quint32 cameraId, quint32 channel) const;
    int previewCount() const { return tiles_.size(); }

signals:
    void activeTileChanged(VideoTileWidget* tile);
    void recordingRequested(VideoTileWidget* tile, bool start);
    void fullScreenRequested(VideoTileWidget* tile);
    void tileAboutToRemove(VideoTileWidget* tile);
    void previewCountChanged(int count);
    void previewStateChanged(quint32 cameraId, quint32 channel, bool active);
    void message(const QString& text);

private:
    void setActiveTile(VideoTileWidget* tile);
    void relayout();
    QString key(quint32 cameraId, quint32 channel) const;

    QGridLayout* grid_;
    QLabel* emptyState_;
    QHash<QString, VideoTileWidget*> tiles_;
    QList<VideoTileWidget*> order_;
    VideoTileWidget* active_;
};

#endif
