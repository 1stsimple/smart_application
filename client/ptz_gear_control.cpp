#include "ptz_gear_control.h"

#include <QApplication>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QSoundEffect>
#include <QStandardPaths>
#include <QUrl>
#include <QtMath>

PtzGearControl::PtzGearControl(QWidget* parent)
    : QWidget(parent), sound_(new QSoundEffect(this)), activeSpeed_(0),
      lastNotch_(-1), dragging_(false), soundEnabled_(true) {
    setObjectName("ptzGearControl");
    setMinimumSize(210, 210);
    setMaximumHeight(280);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    setToolTip(tr("按住并向八个方向拖动；拖得越远速度越快，释放后立即停止"));
    sound_->setVolume(0.08);
    const QString soundPath = createTickSound();
    if (!soundPath.isEmpty()) sound_->setSource(QUrl::fromLocalFile(soundPath));
    commandThrottle_.start();
    soundThrottle_.start();
}

void PtzGearControl::setSoundEnabled(bool enabled) {
    soundEnabled_ = enabled;
    if (!enabled) sound_->stop();
}

QString PtzGearControl::createTickSound() {
    QString directory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (directory.isEmpty()) directory = QDir::tempPath();
    const QString path = QDir(directory).filePath(QStringLiteral("smart_home_ptz_gear_tick.wav"));
    if (QFile::exists(path)) return path;

    const int sampleRate = 22050;
    const int sampleCount = sampleRate * 55 / 1000;
    QByteArray samples;
    samples.reserve(sampleCount * 2);
    quint32 noise = 0x13572468U;
    for (int i = 0; i < sampleCount; ++i) {
        noise = noise * 1664525U + 1013904223U;
        const double decay = qExp(-7.0 * double(i) / sampleCount);
        const double metal = qSin(2.0 * M_PI * 1150.0 * i / sampleRate) * 0.72 +
                             qSin(2.0 * M_PI * 1730.0 * i / sampleRate) * 0.28;
        const double grain = (double((noise >> 16) & 0x7fff) / 16384.0 - 1.0) * 0.18;
        const qint16 value = qint16(qBound(-1.0, (metal + grain) * decay, 1.0) * 10000);
        samples.append(char(value & 0xff));
        samples.append(char((value >> 8) & 0xff));
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return QString();
    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    file.write("RIFF", 4);
    out << quint32(36 + samples.size());
    file.write("WAVEfmt ", 8);
    out << quint32(16) << quint16(1) << quint16(1) << quint32(sampleRate)
        << quint32(sampleRate * 2) << quint16(2) << quint16(16);
    file.write("data", 4);
    out << quint32(samples.size());
    file.write(samples);
    return path;
}

QString PtzGearControl::directionFor(const QPointF& delta) const {
    const double angle = qRadiansToDegrees(qAtan2(-delta.y(), delta.x()));
    const int sector = (qRound(angle / 45.0) + 8) % 8;
    static const char* directions[8] = {
        "right", "right_up", "up", "left_up",
        "left", "left_down", "down", "right_down"
    };
    return QString::fromLatin1(directions[sector]);
}

void PtzGearControl::updateControl(const QPoint& position, bool force) {
    const QPointF center(width() / 2.0, height() / 2.0);
    const QPointF delta = QPointF(position) - center;
    const double distance = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());
    const double maximum = qMax(1.0, qMin(width(), height()) * 0.43);
    const double deadZone = maximum * 0.20;
    if (distance <= deadZone) {
        if (!activeCommand_.isEmpty() && activeCommand_ != QStringLiteral("stop"))
            emit stopRequested();
        activeCommand_ = QStringLiteral("stop");
        activeSpeed_ = 0;
        lastNotch_ = -1;
        update();
        return;
    }
    const QString command = directionFor(delta);
    const int speed = qBound(1, qRound((distance - deadZone) / (maximum - deadZone) * 100.0), 100);
    const int notch = qBound(0, speed / 10, 10);
    const bool changed = command != activeCommand_ || qAbs(speed - activeSpeed_) >= 8;
    activeCommand_ = command;
    activeSpeed_ = speed;
    emit speedSuggested(speed);
    if (force || (changed && commandThrottle_.elapsed() >= 70)) {
        emit commandStarted(command, speed);
        commandThrottle_.restart();
    }
    playTick(notch);
    update();
}

void PtzGearControl::playTick(int notch) {
    if (!soundEnabled_ || notch == lastNotch_ || soundThrottle_.elapsed() < 65) return;
    lastNotch_ = notch;
    if (sound_->status() == QSoundEffect::Ready) sound_->play();
    soundThrottle_.restart();
}

void PtzGearControl::stopControl() {
    if (!dragging_ && activeCommand_.isEmpty()) return;
    dragging_ = false;
    activeCommand_.clear();
    activeSpeed_ = 0;
    lastNotch_ = -1;
    emit stopRequested();
    update();
}

void PtzGearControl::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || !isEnabled()) return;
    setFocus(Qt::MouseFocusReason);
    dragging_ = true;
    updateControl(event->pos(), true);
    event->accept();
}

void PtzGearControl::mouseMoveEvent(QMouseEvent* event) {
    if (!dragging_) return;
    updateControl(event->pos(), false);
    event->accept();
}

void PtzGearControl::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && dragging_) {
        stopControl();
        event->accept();
    }
}

void PtzGearControl::leaveEvent(QEvent* event) {
    if (dragging_) stopControl();
    QWidget::leaveEvent(event);
}

void PtzGearControl::focusOutEvent(QFocusEvent* event) {
    if (dragging_) stopControl();
    QWidget::focusOutEvent(event);
}

void PtzGearControl::keyPressEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) return;
    QString command;
    if (event->key() == Qt::Key_Up) command = QStringLiteral("up");
    else if (event->key() == Qt::Key_Down) command = QStringLiteral("down");
    else if (event->key() == Qt::Key_Left) command = QStringLiteral("left");
    else if (event->key() == Qt::Key_Right) command = QStringLiteral("right");
    else if (event->key() == Qt::Key_Space) command = QStringLiteral("stop");
    if (!command.isEmpty()) {
        dragging_ = true;
        activeCommand_ = command;
        activeSpeed_ = 50;
        emit commandStarted(command, activeSpeed_);
        update();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void PtzGearControl::keyReleaseEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat() && dragging_) {
        stopControl();
        event->accept();
        return;
    }
    QWidget::keyReleaseEvent(event);
}

void PtzGearControl::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const QPointF center(width() / 2.0, height() / 2.0);
    const qreal radius = qMin(width(), height()) * 0.39;
    const QColor accent = palette().highlight().color();
    const QColor base = palette().window().color();
    const QColor text = palette().text().color();
    const QColor ring = activeCommand_.isEmpty() ? palette().mid().color() : accent;

    painter.setPen(QPen(ring, 3));
    painter.setBrush(base.lighter(108));
    painter.drawEllipse(center, radius, radius);
    painter.setPen(QPen(ring, 2));
    for (int i = 0; i < 24; ++i) {
        const double angle = 2.0 * M_PI * i / 24.0;
        const QPointF outer(center.x() + qCos(angle) * radius,
                            center.y() + qSin(angle) * radius);
        const QPointF inner(center.x() + qCos(angle) * (radius - (i % 3 == 0 ? 15 : 9)),
                            center.y() + qSin(angle) * (radius - (i % 3 == 0 ? 15 : 9)));
        painter.drawLine(inner, outer);
    }
    painter.setBrush(activeCommand_.isEmpty() ? base.darker(105) : accent);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, radius * 0.28, radius * 0.28);
    painter.setPen(activeCommand_.isEmpty() ? text : QColor(Qt::white));
    painter.drawText(QRectF(center.x() - radius * 0.45, center.y() - 18,
                            radius * 0.9, 36), Qt::AlignCenter,
                     activeCommand_.isEmpty() ? tr("拖动控制") : activeCommand_);
}
