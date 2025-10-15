#ifndef XVFBEVENT_H
#define XVFBEVENT_H

#include <QObject>
#include "xvfb.h"

#undef Bool

class XvfbEvent : public QObject
{
    Q_OBJECT
public:
    explicit XvfbEvent(QObject *parent = 0);
    Q_INVOKABLE void keyPress(const int key, const int modifiers, const bool pressed);
    Q_INVOKABLE void mouseMove(const int x, const int y, const int width, const int height);
    Q_INVOKABLE void mouseClick(const int button, const int pressed);

signals:

public slots:
private:
    Xvfb     *m_xvfb;
    int      m_buttonPressed = 0;
};

#endif // XVFBEVENT_H
