#include "xvfbevent.h"

XvfbEvent::XvfbEvent(QObject *parent) : QObject(parent)
{
    m_xvfb = Xvfb::instance();
}

void XvfbEvent::keyPress(const int key, const int modifiers, const bool pressed)
{
    m_xvfb->sendKeyEvent(key, modifiers, pressed);
}

void XvfbEvent::mouseMove(const int x, const int y, const int width, const int height)
{
    m_xvfb->sendMoveMouseEvent(x ,y, width, height);
}

void XvfbEvent::mouseClick(const int button, const int pressed)
{
    if (pressed) {
        m_buttonPressed = button;
        m_xvfb->sendMouseClick(button, true);
    } else if (button) {
        m_xvfb->sendMouseClick(m_buttonPressed, false);
        m_buttonPressed = button;
    } else
        m_xvfb->sendMouseClick(m_buttonPressed, false);
}

