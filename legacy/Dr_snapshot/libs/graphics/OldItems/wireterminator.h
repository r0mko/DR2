#ifndef WIRETERMINATOR_H
#define WIRETERMINATOR_H

#include "wiringelement.h"
#include "smoothwire.h"

class WireTerminator : public WiringElement
{
    Q_OBJECT

    Q_PROPERTY(SmoothWire* first READ first WRITE setFirst NOTIFY firstChanged)
    Q_PROPERTY(SmoothWire* second READ second WRITE setSecond NOTIFY secondChanged)
    Q_PROPERTY(qreal span READ span WRITE setSpan NOTIFY spanChanged)
    Q_PROPERTY(qreal length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)

    QPolygonF polygon1;
    QPolygonF polygon2;

public:
    explicit WireTerminator(SceneItem *parent = 0);
    ~WireTerminator();
    QPainterPath shape() const;
    SmoothWire* first() const;
    qreal span() const;
    SmoothWire* second() const;
    qreal length() const;
    qreal width() const;

    void setFirst(SmoothWire* arg);
    void setSpan(qreal arg);
    void setSecond(SmoothWire* arg);
    void setLength(qreal arg);
    void setWidth(qreal arg);

public slots:

signals:
    void firstChanged(SmoothWire* arg);
    void spanChanged(qreal arg);
    void secondChanged(SmoothWire* arg);
    void lengthChanged();

    void widthChanged(qreal arg);

protected:
    void build();
    void paint(QPainter *painter);
    QRectF getBoundingRect() const;

private:
    void setConnections();
    SmoothWire* m_first;
    qreal m_span;
    SmoothWire* m_second;
    qreal m_width;
    qreal m_length;
};



#endif // WIRETERMINATOR_H
