#include "wiretransition.h"
#include "scene.h"
#include "pen.h"
#include <QPainter>

WireTransition::WireTransition(SceneItem *parent) :
    WiringElement(parent),
    m_first(nullptr),
    m_second(nullptr),
    m_span(20),
    m_easing(QEasingCurve::InOutCirc)
{

}

WireTransition::~WireTransition()
{
    qDebug() << "WireTransition element destroyed" << this;
}

SmoothWire *WireTransition::first() const
{
    return m_first;
}

SmoothWire *WireTransition::second() const
{
    return m_second;
}

qreal WireTransition::span() const
{
    return m_span;
}

QEasingCurve::Type WireTransition::easing() const
{
    return m_easing;
}

void WireTransition::setFirst(SmoothWire *arg)
{
    if (m_first == arg)
        return;
    if (m_first)
        m_first->guide()->disconnect(0, this, 0);
    m_first = arg;
    connect(m_first->guide(), SIGNAL(polygonChanged()), this, SLOT(rebuild()));
    if (m_second)
        setConnections();
    emit firstChanged(arg);
}

void WireTransition::setSecond(SmoothWire *arg)
{
    if (m_second == arg)
        return;

    if (m_second)
        m_second->guide()->disconnect(0, this, 0);
    m_second = arg;
    connect(m_second->guide(), SIGNAL(polygonChanged()), this, SLOT(rebuild()));
    if (m_first)
        setConnections();
    emit secondChanged(arg);
}

void WireTransition::setSpan(qreal arg)
{
    if (m_span == arg)
        return;

    m_span = arg;
    emit spanChanged(arg);
    build();
}

void WireTransition::setEasing(QEasingCurve::Type arg)
{
    if (m_easing == arg)
        return;

    m_easing = arg;
    emit easingChanged(arg);
    build();
}

QPainterPath WireTransition::shape() const
{
    QPainterPath p;
    p.addPolygon(polygon);
    return buildOutline(p, m_first->pen()->pen());
}

void WireTransition::build()
{
//    qDebug() << "Builld transition";
    if (!m_first || !m_second)
        return;
    if (m_first->guide()->vectorPath().isNull() || m_second->guide()->vectorPath().isNull())
        return;
    qreal range = m_span * zoomFactor();
//    qDebug() << "transition range"<< range;
    m_first->setEndMargin(range / 2);
    m_second->setBeginMargin(range / 2);

    VectorPath::link l = m_first->guide()->vectorPath().attachTail(m_second->guide()->vectorPath());
    VectorPath::Runner r = m_first->guide()->vectorPath().setRunner(m_first->guide()->length() - m_first->endMargin());
    qreal dist = 0;
    VectorPath ret;
    QEasingCurve curve(m_easing);
    qreal k = m_second->offset() - m_first->offset();
    qreal b = m_first->offset();
//    qreal rad = (m_first->radius() + m_second->radius()) / 2;
    qreal prec = m_first->precision() * 2;
    while (dist < range) {
        qreal x = dist / range;
        qreal y = curve.valueForProgress(x);
        qreal ov = (k * y + b) * m_first->pen()->width();
        ret.addPoint(r.getSmoothedPoint(m_first->effectiveRadius), ov * zoomFactor());
        dist += r.moveBy(range / prec);
//        qDebug() << "adding point at pos" << x << "value" << ov << r.atEnd();
        if (r.atEnd())
            break;
    }
    polygon = ret.buildOffset(1, true).toPolygon();
    if (!m_second->polygon.isEmpty())
        polygon.append(m_second->polygon.first());
    item()->setBoundingRectDirty();
    update();
}

void WireTransition::paint(QPainter *painter)
{
    if (!m_first || !m_second)
        return;
    if (!m_first->pen())
        return;
    painter->setPen(m_first->pen()->pen());
    painter->drawPolyline(polygon);
    if (debugNodes())
        drawPolygonNodes(painter, polygon, m_first->pen()->pen());
}

QRectF WireTransition::getBoundingRect() const
{
    return polygon.boundingRect();
}


void WireTransition::setConnections()
{
    connect(m_first, SIGNAL(offsetChanged()), this, SLOT(rebuild()));
    connect(m_second, SIGNAL(offsetChanged()), this, SLOT(rebuild()));
    connect(m_first, SIGNAL(radiusChanged()), this, SLOT(rebuild()));
    connect(m_second, SIGNAL(radiusChanged()), this, SLOT(rebuild()));
    connect(m_first, SIGNAL(penChanged()), this, SLOT(rebuild()));
    connect(m_second, SIGNAL(penChanged()), this, SLOT(rebuild()));
    connect(m_first, SIGNAL(precisionChanged()), this, SLOT(rebuild()));
    connect(m_second, SIGNAL(precisionChanged()), this, SLOT(rebuild()));
    build();
}
