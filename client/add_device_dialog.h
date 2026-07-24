#ifndef SMART_HOME_CLIENT_ADD_DEVICE_DIALOG_H
#define SMART_HOME_CLIENT_ADD_DEVICE_DIALOG_H

#include "protocol_client.h"

#include <QDialog>

class QComboBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTcpSocket;

class AddDeviceDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddDeviceDialog(QWidget* parent = 0);
    CameraDeviceDto device() const;

private slots:
    void testConnection();
    void validateAndAccept();
    void updateStreamPreview();

private:
    QString authenticatedUrl(const QString& url) const;

    QLineEdit *id_, *name_, *address_, *port_, *username_, *password_;
    QLineEdit *primaryStream_, *secondaryStream_;
    QComboBox *protocol_, *deviceType_;
    QSpinBox* channels_;
    QLabel *testStatus_, *streamPreview_;
    QPushButton* testButton_;
    QDialogButtonBox* buttons_;
    QTcpSocket* testSocket_;
};

#endif
