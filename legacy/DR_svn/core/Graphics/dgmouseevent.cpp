#include "dgmouseevent.h"

DGMouseEvent::DGMouseEvent(QMouseEvent *event, QObject *parent, const QPointF &overridePos) :
    QObject(parent),
    m_modifiers(event->modifiers()),
    m_buttons(event->buttons()),
    m_button(event->button()),
    m_pos(event->pos()),
    m_eventType(event->type())
{
    if (!overridePos.isNull())
        m_pos = overridePos;
}

Qt::KeyboardModifiers DGMouseEvent::modifiers() const
{
    return m_modifiers;
}

Qt::MouseButtons DGMouseEvent::buttons() const
{
    return m_buttons;
}

int DGMouseEvent::button() const
{
    return m_button;
}

QPointF DGMouseEvent::pos() const
{
    return m_pos;
}

DGMouseEvent::Type DGMouseEvent::type() const
{
    return m_type;
}

QEvent::Type DGMouseEvent::eventType() const
{
    return m_eventType;
}

qreal DGMouseEvent::x() const
{
    return m_pos.x();
}

qreal DGMouseEvent::y() const
{
    return m_pos.y();
}

