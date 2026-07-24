#ifndef SMART_HOME_CLIENT_PTZ_GEAR_CONTROL_H
#define SMART_HOME_CLIENT_PTZ_GEAR_CONTROL_H

#include <QElapsedTimer>
#include <QWidget>

class QFocusEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QSoundEffect;

class PtzGearControl : public QWidget {
    Q_OBJECT
public:
    explicit PtzGearControl(QWidget* parent = 0);
    void setSoundEnabled(bool enabled);

signals:
    void commandStarted(const QString& command, int speed);
    void stopRequested();
    void speedSuggested(int speed);

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void leaveEvent(QEvent* event);
    void focusOutEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    void updateControl(const QPoint& position, bool force);
    void stopControl();
    QString directionFor(const QPointF& delta) const;
    void playTick(int notch);
    QString createTickSound();

    QSoundEffect* sound_;
    QElapsedTimer commandThrottle_;
    QElapsedTimer soundThrottle_;
    QString activeCommand_;
    int activeSpeed_;
    int lastNotch_;
    bool dragging_;
    bool soundEnabled_;
};

#endif
