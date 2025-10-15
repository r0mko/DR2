#ifndef WIRETRANSITION_H
#define WIRETRANSITION_H

#include "wiringelement.h"
#include "smoothwire.h"
#include <QEasingCurve>

class WireTransition : public WiringElement
{
    Q_OBJECT
    Q_PROPERTY(SmoothWire* first READ first WRITE setFirst NOTIFY firstChanged)
    Q_PROPERTY(SmoothWire* second READ second WRITE setSecond NOTIFY secondChanged)
    Q_PROPERTY(qreal span READ span WRITE setSpan NOTIFY spanChanged)
    Q_PROPERTY(QEasingCurve::Type easing READ easing WRITE setEasing NOTIFY easingChanged)

    SmoothWire* m_first;
    SmoothWire* m_second;
    qreal m_span;
    QEasingCurve::Type m_easing;
    QPolygonF polygon;

public:
    WireTransition(SceneItem *parent = 0);
    ~WireTransition();
    SmoothWire* first() const;
    SmoothWire* second() const;
    qreal span() const;
    QEasingCurve::Type easing() const;
    void setFirst(SmoothWire* arg);
    void setSecond(SmoothWire* arg);
    void setSpan(qreal arg);
    void setEasing(QEasingCurve::Type arg);
    QPainterPath shape() const;
signals:

    void firstChanged(SmoothWire* arg);
    void secondChanged(SmoothWire* arg);
    void spanChanged(qreal arg);
    void easingChanged(QEasingCurve::Type arg);

    // WiringElement interface
protected:
    void build();
    void paint(QPainter *painter);
    QRectF getBoundingRect() const;
private:
    void setConnections();
};

#endif // WIRETRANSITION_H
