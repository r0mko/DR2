#include "wireterminator.h"
#include "scene.h"
#include "pen.h"
#include <QPainter>

WireTerminator::WireTerminator(SceneItem *parent) :
    WiringElement(parent),
    m_first(nullptr),
    m_second(nullptr),
    m_span(20),
    m_width(10)
{

}

WireTerminator::~WireTerminator()
{
    qDebug() << "WireTerminator element destroyed" << this;
}

QPainterPath WireTerminator::shape() const
{
    QPainterPath p;
    p.addPolygon(polygon1);
    p.addPolygon(polygon2);
    p.closeSubpath();
//    return p;
    return buildOutline(p, m_first->pen()->pen());
}

SmoothWire *WireTerminator::first() const
{
    return m_first;
}

qreal WireTerminator::span() const
{
    return m_span;
}

SmoothWire *WireTerminator::second() const
{
    return m_second;
}

qreal WireTerminator::length() const
{
    return m_length;
}

qreal WireTerminator::width() const
{
    return m_width;
}

void WireTerminator::setFirst(SmoothWire *arg)
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

void WireTerminator::setSpan(qreal arg)
{
    if (m_span == arg)
        return;

    m_span = arg;
    emit spanChanged(arg);
    build();
}

void WireTerminator::setSecond(SmoothWire *arg)
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

void WireTerminator::build()
{
    if (!m_first && !m_second)
        return;
    if (m_first->guide()->vectorPath().isNull() && m_second->guide()->vectorPath().isNull())
        return;

    qreal range = m_span * zoomFactor();

    m_first->setEndMargin(range / 2);
    m_second->setBeginMargin(range / 2);

    polygon1.clear();
    polygon2.clear();

    if( m_first != nullptr && !m_first->offsetPath().isNull() ){
        QPointF pt1 = m_first->offsetPath().tail()->endPoint();
        polygon1.append( pt1 + ( m_first->offsetPath().tail()->normal() * m_length * zoomFactor() ).toPointF() );
        polygon1.append( pt1 - ( m_first->offsetPath().tail()->normal() * m_length * zoomFactor() ).toPointF() );
    }
    if( m_second != nullptr && !m_second->offsetPath().isNull() ){
        QPointF pt2 = m_second->offsetPath().head()->point;
        polygon2.append( pt2 + ( m_second->offsetPath().head()->normal() * m_length * zoomFactor() ).toPointF() );
        polygon2.append( pt2 - ( m_second->offsetPath().head()->normal() * m_length * zoomFactor() ).toPointF() );
    }
    item()->setBoundingRectDirty();
    update();
}

void WireTerminator::paint(QPainter *painter)
{
    if (!m_first && !m_second)
        return;
//    if (!m_first->pen())
//        return;

    QPen firstPen( Qt::white );
    if( m_first && m_first->pen() ) firstPen = m_first->pen()->pen();
    QPen secondPen( Qt::white );
    if( m_second && m_second->pen() ) secondPen = m_second->pen()->pen();

    firstPen.setWidth(m_width);
    painter->setPen(firstPen);
    painter->drawPolyline(polygon1);

    secondPen.setWidth(m_width);
    painter->setPen(secondPen);
    painter->drawPolyline(polygon2);

//    painter->setBrush( QColor( 255, 255, 255, 0x40 ) );
//    painter->setPen( Qt::NoPen );
//    painter->drawRect( getBoundingRect() );

    if (debugNodes()) {
        drawPolygonNodes(painter, polygon1, m_first->pen()->pen());
        drawPolygonNodes(painter, polygon2, m_second->pen()->pen());
    }
}

QRectF WireTerminator::getBoundingRect() const
{
//    qDebug() <<  polygon1.united(polygon2).boundingRect();
    return polygon1.united(polygon2).boundingRect();
}

void WireTerminator::setConnections()
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



void WireTerminator::setLength(qreal arg)
{
    qDebug() << "setting new length" << arg;
    if (m_length == arg)
        return;

    m_length = arg;
    emit lengthChanged();
    build();
}

void WireTerminator::setWidth(qreal arg)
{
    qDebug() << "setting new width" << arg;
    if (m_width == arg)
        return;

    m_width = arg;
    emit widthChanged(arg);
    build();
}
