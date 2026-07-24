#ifndef SMART_HOME_CLIENT_MAIN_WINDOW_H
#define SMART_HOME_CLIENT_MAIN_WINDOW_H

#include "protocol_client.h"

#include <QMainWindow>
#include <QMediaPlayer>
#include <QList>

class QLabel;
class QAction;
class QCheckBox;
class QGridLayout;
class QKeyEvent;
class QLineEdit;
class QPlainTextEdit;
class QPoint;
class QProcess;
class QPushButton;
class QSlider;
class QSpinBox;
class QStackedWidget;
class QSplitter;
class QTreeWidget;
class QVideoWidget;
class QWidget;
class VideoGridWidget;
class VideoTileWidget;
class PtzGearControl;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(ProtocolClient* authenticatedClient = 0, QWidget* parent = 0);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent* event);
private slots:
    void showCameras(const QVector<CameraDeviceDto>& cameras);
    void playLive();
    void stopLive();
    void saveCamera();
    void startRecording();
    void stopRecording();
    void log(const QString& text);
    void applyTheme(bool dark);
    void restoreDefaultMonitorLayout();
    void togglePreviewMaximized();
private:
    bool selection(CameraDeviceDto& camera, quint32& channel) const;
    bool activeSelection(CameraDeviceDto& camera, quint32& channel) const;
    QString selectedStream(const CameraDeviceDto& camera) const;
    QString selectedStream(const CameraDeviceDto& camera, quint32 channel) const;
    void sendPtz(const QString& command);
    void loadSettings();
    void saveSettings() const;
    void adjustPreviewWidth(int direction);
    void updatePreviewLayoutButtons();
    void addDevice();
    void showDeviceContextMenu(const QPoint& position);
    void enterVideoFullScreen(VideoTileWidget* tile);
    void exitVideoFullScreen();
    void startWebcamPublisher();
    void stopWebcamPublisher();
    QString ffmpegExecutable() const;

    ProtocolClient* protocol_;
    QVector<CameraDeviceDto> cameras_;
    QMediaPlayer player_;
    QTreeWidget* tree_;
    QStackedWidget* deviceStack_;
    QSplitter* monitorSplitter_;
    QWidget *devicesPanel_, *controlPanel_, *videoHeader_;
    VideoGridWidget* videoGrid_;
    VideoTileWidget* fullScreenTile_;
    QLabel* connectionStatus_;
    QLabel* deviceCount_;
    QLabel* videoStatus_;
    QLabel* publisherStatus_;
    QVideoWidget* playbackView_;
    QStackedWidget* videoStack_;
    QPlainTextEdit* logView_;
    QProcess* publisher_;
    QLineEdit *host_, *port_, *username_, *password_;
    QLineEdit *cameraId_, *serial_, *cameraIp_, *rtsp_, *rtmp_;
    QLineEdit *webcamDevice_, *publishUrl_;
    QSpinBox *cameraType_, *channels_;
    QSlider* speed_;
    QCheckBox *autoLogin_, *autoPublish_;
    QCheckBox* soundEffects_;
    PtzGearControl* ptzGear_;
    QPushButton *publisherStart_, *publisherStop_;
    QPushButton *previewShrinkButton_, *previewExpandButton_, *previewMaximizeButton_;
    QAction *connectionInfoAction_, *themeAction_;
    bool shuttingDown_;
    bool layoutMaximized_;
    bool authenticatedSession_;
    Qt::WindowStates preVideoFullScreenState_;
    QList<int> preMaximizeSizes_;
};

#endif
