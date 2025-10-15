#ifndef DGMOUSEEVENT_H
#define DGMOUSEEVENT_H
#include <QObject>
#include <QMouseEvent>



class DGMouseEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt::KeyboardModifiers modifiers READ modifiers)
    Q_PROPERTY(Qt::MouseButtons buttons READ buttons)
    Q_PROPERTY(int button READ button)
    Q_PROPERTY(QPointF pos READ pos)
    Q_PROPERTY(Type type READ type)
    Q_PROPERTY(qreal x READ x STORED false)
    Q_PROPERTY(qreal y READ y STORED false)

    Q_PROPERTY(QEvent::Type eventType READ eventType)
    Q_ENUMS(Type)
    Q_ENUMS(QEvent::Type)


public:
    enum Type {
        Move,
        Click,
        DoubleClick
    };

    DGMouseEvent(QMouseEvent *parentEvent, QObject *parent = 0, const QPointF &overridePos = QPointF());
    DGMouseEvent() {}
    virtual ~DGMouseEvent() {}
    Qt::KeyboardModifiers modifiers() const;
    Qt::MouseButtons buttons() const;
    int button() const;
    QPointF pos() const;
    Type type() const;
    void setPos(const QPointF &pos) { m_pos = pos; }
    void setButtons(Qt::MouseButtons buttons) { m_buttons = buttons; }
    void setButton(Qt::MouseButton button) { m_button = button; }
    void setModifiers(Qt::KeyboardModifiers modifiers) { m_modifiers = modifiers; }
    QEvent::Type eventType() const;

    qreal x() const;
    qreal y() const;

private:
    Qt::KeyboardModifiers m_modifiers;
    Qt::MouseButtons m_buttons;
    Qt::MouseButton m_button;
    Type m_type;
    QPointF m_pos;
    QEvent::Type m_eventType;
};



#endif // DGMOUSEEVENT_H
