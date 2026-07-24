#ifndef SMART_HOME_CLIENT_RECORDING_BROWSER_DIALOG_H
#define SMART_HOME_CLIENT_RECORDING_BROWSER_DIALOG_H

#include "protocol_client.h"

#include <QCryptographicHash>
#include <QDialog>
#include <QElapsedTimer>
#include <QVector>

class QComboBox;
class QDateTimeEdit;
class QFile;
class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;
class QPushButton;
class QTableWidget;

class RecordingBrowserDialog : public QDialog {
    Q_OBJECT
public:
    RecordingBrowserDialog(ProtocolClient* client,
                           const QVector<CameraDeviceDto>& cameras,
                           QWidget* parent = 0);
    ~RecordingBrowserDialog();

private slots:
    void updateChannels();
    void query();
    void populate(const QVector<RecordingSegmentDto>& segments);
    void playSelected();
    void downloadSelected();
    void showSelectedInformation();

private:
    int selectedSegmentIndex() const;
    void startDownload(const RecordingSegmentDto& segment, const QString& path);
    void finishDownload();
    static QString humanSize(quint64 bytes);

    ProtocolClient* client_;
    QVector<CameraDeviceDto> cameras_;
    QVector<RecordingSegmentDto> segments_;
    QComboBox *camera_, *channel_;
    QDateTimeEdit *begin_, *end_;
    QTableWidget* table_;
    QPushButton *play_, *download_, *information_;
    QNetworkAccessManager* network_;
    QNetworkReply* reply_;
    QFile* output_;
    QProgressDialog* progress_;
    QCryptographicHash* hash_;
    QString destination_, partPath_, downloadFailure_;
    RecordingSegmentDto downloading_;
    QElapsedTimer downloadTimer_;
    qint64 downloadedBytes_;
    bool cancelled_;
};

#endif
