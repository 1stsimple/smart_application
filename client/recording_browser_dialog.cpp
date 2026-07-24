#include "recording_browser_dialog.h"

#include <QComboBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProgressDialog>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QUrl>
#include <QVBoxLayout>

RecordingBrowserDialog::RecordingBrowserDialog(
    ProtocolClient* client, const QVector<CameraDeviceDto>& cameras, QWidget* parent)
    : QDialog(parent), client_(client), cameras_(cameras),
      network_(new QNetworkAccessManager(this)), reply_(0), output_(0),
      progress_(0), hash_(0), downloadedBytes_(0), cancelled_(false) {
    setWindowTitle(tr("服务器录像"));
    resize(1120, 680);
    setAttribute(Qt::WA_DeleteOnClose, true);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(12);
    QLabel* title = new QLabel(tr("服务器录像列表"));
    title->setProperty("role", "appTitle");
    root->addWidget(title);
    QLabel* subtitle = new QLabel(tr("按设备、通道和日期时间查询真实 TS 分片，可播放或流式下载。"));
    subtitle->setProperty("role", "subtitle");
    root->addWidget(subtitle);

    camera_ = new QComboBox;
    for (int i = 0; i < cameras_.size(); ++i)
        camera_->addItem(cameras_[i].serialNumber, cameras_[i].id);
    channel_ = new QComboBox;
    begin_ = new QDateTimeEdit(QDateTime::currentDateTime().addDays(-1));
    end_ = new QDateTimeEdit(QDateTime::currentDateTime());
    begin_->setCalendarPopup(true);
    end_->setCalendarPopup(true);
    begin_->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    end_->setDisplayFormat(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    QPushButton* queryButton = new QPushButton(tr("查询录像"));
    queryButton->setProperty("role", "primary");
    QHBoxLayout* filters = new QHBoxLayout;
    filters->addWidget(new QLabel(tr("设备")));
    filters->addWidget(camera_);
    filters->addWidget(new QLabel(tr("通道")));
    filters->addWidget(channel_);
    filters->addWidget(new QLabel(tr("开始")));
    filters->addWidget(begin_);
    filters->addWidget(new QLabel(tr("结束")));
    filters->addWidget(end_);
    filters->addWidget(queryButton);
    root->addLayout(filters);

    table_ = new QTableWidget;
    table_->setColumnCount(7);
    table_->setHorizontalHeaderLabels(QStringList()
        << tr("文件名") << tr("设备") << tr("通道") << tr("开始时间")
        << tr("时长") << tr("文件大小") << tr("状态"));
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(true);
    table_->verticalHeader()->hide();
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    for (int i = 1; i < 7; ++i)
        table_->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    root->addWidget(table_, 1);

    play_ = new QPushButton(tr("播放"));
    download_ = new QPushButton(tr("下载"));
    download_->setProperty("role", "primary");
    information_ = new QPushButton(tr("查看信息"));
    QPushButton* close = new QPushButton(tr("关闭"));
    play_->setEnabled(false);
    download_->setEnabled(false);
    information_->setEnabled(false);
    QHBoxLayout* actions = new QHBoxLayout;
    actions->addWidget(play_);
    actions->addWidget(download_);
    actions->addWidget(information_);
    actions->addStretch();
    actions->addWidget(close);
    root->addLayout(actions);

    connect(camera_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RecordingBrowserDialog::updateChannels);
    connect(queryButton, &QPushButton::clicked, this, &RecordingBrowserDialog::query);
    connect(table_, &QTableWidget::itemSelectionChanged, this, [this] {
        const bool selected = selectedSegmentIndex() >= 0;
        play_->setEnabled(selected);
        download_->setEnabled(selected && !reply_);
        information_->setEnabled(selected);
    });
    connect(table_, &QTableWidget::cellDoubleClicked, this,
            [this](int, int) { playSelected(); });
    connect(play_, &QPushButton::clicked, this, &RecordingBrowserDialog::playSelected);
    connect(download_, &QPushButton::clicked, this, &RecordingBrowserDialog::downloadSelected);
    connect(information_, &QPushButton::clicked,
            this, &RecordingBrowserDialog::showSelectedInformation);
    connect(close, &QPushButton::clicked, this, &QDialog::close);
    connect(client_, &ProtocolClient::recordingSegmentsReceived,
            this, &RecordingBrowserDialog::populate);
    connect(client_, &ProtocolClient::protocolError, this, [this](const QString& error) {
        if (isVisible()) QMessageBox::warning(this, tr("录像请求失败"), error);
    });
    updateChannels();
}

RecordingBrowserDialog::~RecordingBrowserDialog() {
    cancelled_ = true;
    if (reply_) {
        disconnect(reply_, 0, this, 0);
        reply_->abort();
    }
    if (output_) {
        output_->close();
        QFile::remove(partPath_);
    }
}

void RecordingBrowserDialog::updateChannels() {
    channel_->clear();
    const int index = camera_->currentIndex();
    if (index < 0 || index >= cameras_.size()) return;
    for (quint32 channel = 0; channel < cameras_[index].channels; ++channel)
        channel_->addItem(tr("通道 %1").arg(channel), channel);
}

void RecordingBrowserDialog::query() {
    if (camera_->currentIndex() < 0) {
        QMessageBox::information(this, tr("没有设备"), tr("服务器尚未返回可查询的设备。"));
        return;
    }
    const qint64 begin = begin_->dateTime().toMSecsSinceEpoch();
    const qint64 end = end_->dateTime().toMSecsSinceEpoch();
    if (end <= begin) {
        QMessageBox::warning(this, tr("时间范围无效"), tr("结束时间必须晚于开始时间。"));
        return;
    }
    table_->setRowCount(0);
    client_->requestRecordingSegments(camera_->currentData().toUInt(),
                                      channel_->currentData().toUInt(), begin, end);
}

void RecordingBrowserDialog::populate(const QVector<RecordingSegmentDto>& segments) {
    segments_ = segments;
    table_->setRowCount(segments_.size());
    for (int row = 0; row < segments_.size(); ++row) {
        const RecordingSegmentDto& segment = segments_[row];
        QString deviceName = QString::number(segment.cameraId);
        for (int i = 0; i < cameras_.size(); ++i)
            if (cameras_[i].id == segment.cameraId) deviceName = cameras_[i].serialNumber;
        const QStringList values = QStringList()
            << segment.fileName << deviceName << QString::number(segment.channel)
            << QDateTime::fromMSecsSinceEpoch(segment.startMs).toString("yyyy-MM-dd HH:mm:ss")
            << QString::number(qMax<qint64>(0, segment.endMs - segment.startMs) / 1000.0, 'f', 1) + tr(" 秒")
            << humanSize(segment.sizeBytes)
            << (segment.discontinuity ? tr("已完成（断流续录）") : tr("已完成"));
        for (int column = 0; column < values.size(); ++column)
            table_->setItem(row, column, new QTableWidgetItem(values[column]));
    }
    if (segments_.isEmpty())
        QMessageBox::information(this, tr("查询完成"), tr("所选时间范围内没有录像分片。"));
}

int RecordingBrowserDialog::selectedSegmentIndex() const {
    const QList<QTableWidgetSelectionRange> ranges = table_->selectedRanges();
    if (ranges.isEmpty()) return -1;
    const int row = ranges.first().topRow();
    return row >= 0 && row < segments_.size() ? row : -1;
}

void RecordingBrowserDialog::playSelected() {
    const int index = selectedSegmentIndex();
    if (index < 0) return;
    const RecordingSegmentDto& segment = segments_[index];
    client_->requestPlayback(segment.cameraId, segment.channel,
                             segment.startMs, segment.endMs);
}

void RecordingBrowserDialog::downloadSelected() {
    if (reply_) {
        QMessageBox::information(this, tr("下载进行中"), tr("请等待当前下载完成或取消。"));
        return;
    }
    const int index = selectedSegmentIndex();
    if (index < 0) return;
    const RecordingSegmentDto segment = segments_[index];
    const QString path = QFileDialog::getSaveFileName(
        this, tr("保存服务器录像"), segment.fileName, tr("MPEG-TS 视频 (*.ts)"));
    if (path.isEmpty()) return;
    if (QFile::exists(path) && QMessageBox::question(
            this, tr("覆盖文件"), tr("目标文件已存在，是否覆盖？")) != QMessageBox::Yes)
        return;
    startDownload(segment, path);
}

void RecordingBrowserDialog::startDownload(
    const RecordingSegmentDto& segment, const QString& path) {
    destination_ = path;
    partPath_ = path + QStringLiteral(".part");
    downloading_ = segment;
    cancelled_ = false;
    downloadFailure_.clear();
    downloadedBytes_ = 0;
    QFile::remove(partPath_);
    output_ = new QFile(partPath_, this);
    if (!output_->open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("无法下载"), tr("无法创建临时文件：%1").arg(partPath_));
        delete output_;
        output_ = 0;
        return;
    }
    hash_ = new QCryptographicHash(QCryptographicHash::Md5);
    progress_ = new QProgressDialog(tr("正在连接服务器…"), tr("取消"), 0, 1000, this);
    progress_->setWindowTitle(tr("下载录像"));
    progress_->setWindowModality(Qt::WindowModal);
    progress_->setMinimumDuration(0);
    download_->setEnabled(false);
    downloadTimer_.restart();

    QNetworkRequest request(segment.downloadUrl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    reply_ = network_->get(request);
    connect(progress_, &QProgressDialog::canceled, this, [this] {
        cancelled_ = true;
        if (reply_) reply_->abort();
    });
    connect(reply_, &QNetworkReply::readyRead, this, [this] {
        const QByteArray chunk = reply_->readAll();
        if (chunk.isEmpty()) return;
        const qint64 written = output_->write(chunk);
        if (written != chunk.size()) {
            downloadFailure_ = tr("写入临时文件失败，请检查磁盘空间和目录权限");
            reply_->abort();
            return;
        }
        hash_->addData(chunk);
        downloadedBytes_ += written;
    });
    connect(reply_, &QNetworkReply::downloadProgress, this,
            [this](qint64 received, qint64 total) {
        const qint64 elapsed = qMax<qint64>(1, downloadTimer_.elapsed());
        const double bytesPerSecond = received * 1000.0 / elapsed;
        if (total > 0) progress_->setValue(int(received * 1000 / total));
        progress_->setLabelText(tr("%1 / %2 · %3/s")
            .arg(humanSize(quint64(qMax<qint64>(0, received))))
            .arg(total > 0 ? humanSize(quint64(total)) : tr("未知大小"))
            .arg(humanSize(quint64(bytesPerSecond))));
    });
    connect(reply_, &QNetworkReply::finished, this, &RecordingBrowserDialog::finishDownload);
}

void RecordingBrowserDialog::finishDownload() {
    if (!reply_) return;
    const QNetworkReply::NetworkError error = reply_->error();
    const QString errorText = reply_->errorString();
    output_->flush();
    output_->close();
    const QByteArray actualHash = hash_->result().toHex();
    const bool sizeMatches = downloading_.sizeBytes == 0 ||
        quint64(downloadedBytes_) == downloading_.sizeBytes;
    const bool hashMatches = downloading_.checksum.isEmpty() ||
        QString::fromLatin1(actualHash).compare(downloading_.checksum, Qt::CaseInsensitive) == 0;
    reply_->deleteLater();
    reply_ = 0;
    delete hash_;
    hash_ = 0;
    output_->deleteLater();
    output_ = 0;
    progress_->setValue(1000);
    progress_->deleteLater();
    progress_ = 0;
    download_->setEnabled(selectedSegmentIndex() >= 0);

    if (error != QNetworkReply::NoError || !downloadFailure_.isEmpty() ||
        !sizeMatches || !hashMatches) {
        QFile::remove(partPath_);
        const QString reason = !downloadFailure_.isEmpty() ? downloadFailure_ :
            (error != QNetworkReply::NoError ? errorText :
             (!sizeMatches ? tr("文件大小校验失败") : tr("MD5 校验失败")));
        if (!cancelled_ && QMessageBox::question(
                this, tr("下载失败"), tr("%1\n临时文件已清理。是否重试？").arg(reason),
                QMessageBox::Retry | QMessageBox::Cancel) == QMessageBox::Retry)
            startDownload(downloading_, destination_);
        return;
    }
    QFile::remove(destination_);
    if (!QFile::rename(partPath_, destination_)) {
        QMessageBox::warning(this, tr("下载完成但重命名失败"),
                             tr("完整文件保留在：%1").arg(partPath_));
        return;
    }
    if (QMessageBox::question(this, tr("下载完成"),
            tr("录像已保存到：\n%1\n\n是否打开所在文件夹？").arg(destination_)) == QMessageBox::Yes)
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(destination_).absolutePath()));
}

void RecordingBrowserDialog::showSelectedInformation() {
    const int index = selectedSegmentIndex();
    if (index < 0) return;
    const RecordingSegmentDto& segment = segments_[index];
    QMessageBox::information(this, tr("录像信息"),
        tr("文件：%1\n分片 ID：%2\n设备：%3\n通道：%4\n开始：%5\n结束：%6\n大小：%7\nMD5：%8\n断流续录：%9")
            .arg(segment.fileName).arg(segment.id).arg(segment.cameraId).arg(segment.channel)
            .arg(QDateTime::fromMSecsSinceEpoch(segment.startMs).toString("yyyy-MM-dd HH:mm:ss"))
            .arg(QDateTime::fromMSecsSinceEpoch(segment.endMs).toString("yyyy-MM-dd HH:mm:ss"))
            .arg(humanSize(segment.sizeBytes)).arg(segment.checksum)
            .arg(segment.discontinuity ? tr("是") : tr("否")));
}

QString RecordingBrowserDialog::humanSize(quint64 bytes) {
    static const char* units[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    double value = double(bytes);
    int unit = 0;
    while (value >= 1024.0 && unit < 4) { value /= 1024.0; ++unit; }
    return QString::number(value, 'f', unit == 0 ? 0 : 1) + QLatin1Char(' ') +
           QString::fromLatin1(units[unit]);
}
