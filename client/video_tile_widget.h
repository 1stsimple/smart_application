#ifndef SMART_HOME_CLIENT_VIDEO_TILE_WIDGET_H
#define SMART_HOME_CLIENT_VIDEO_TILE_WIDGET_H

#include "protocol_client.h"

#include <QFrame>
#include <QImage>
#include <QThread>

class QLabel;
class QPushButton;
class QResizeEvent;
class VideoDecoder;

class VideoTileWidget : public QFrame {
    Q_OBJECT
public:
    VideoTileWidget(const CameraDeviceDto& camera, quint32 channel,
                    const QString& serverHost, quint16 serverPort,
                    const QString& username, const QString& password,
                    QWidget* parent = 0);
    ~VideoTileWidget();

    quint32 cameraId() const { return camera_.id; }
    quint32 channel() const { return channel_; }
    CameraDeviceDto camera() const { return camera_; }
    QString key() const;
    bool isActive() const { return active_; }
    void setActive(bool active);
    void startPreview();
    void stopPreview();
    void setRecordingResult(bool active, bool success, const QString& resultMessage);

signals:
    void activated(VideoTileWidget* tile);
    void closeRequested(VideoTileWidget* tile);
    void recordingRequested(VideoTileWidget* tile, bool start);
    void fullScreenRequested(VideoTileWidget* tile);
    void message(const QString& text);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void resizeEvent(QResizeEvent* event);

private:
    void setPreviewState(const QString& text, const char* tone);
    void updatePixmap();
    QString streamUrl() const;

    CameraDeviceDto camera_;
    quint32 channel_;
    QString host_, username_, password_;
    quint16 port_;
    ProtocolClient* client_;
    VideoDecoder* decoder_;
    QThread decoderThread_;
    QLabel *title_, *status_, *surface_;
    QPushButton *recordButton_, *fullScreenButton_, *closeButton_;
    QImage lastFrame_;
    bool active_;
    bool stopping_;
    bool recording_;
};

#endif
