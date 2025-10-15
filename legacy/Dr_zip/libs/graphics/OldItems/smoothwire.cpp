#include "smoothwire.h"
#include "pen.h"
#include "scene.h"
#include <QPainter>
#include <QDebug>

SmoothWire::SmoothWire(SceneItem *parent) :
    WiringElement(parent),
    m_pen(nullptr),
    m_offset(0),
    m_radius(1),
    m_precision(4),
    m_guide(nullptr),
    m_beginMargin(0),
    m_endMargin(0),
    m_result(new Groove(&m_offsetPath, this))

{

}

SmoothWire::~SmoothWire()
{
//    qDebug() << "SmoothWire element destroyed" << this;
}

Pen *SmoothWire::pen() const
{
    return m_pen;
}

qreal SmoothWire::offset() const
{
    return m_offset;
}

qreal SmoothWire::radius() const
{
    return m_radius;
}

qreal SmoothWire::precision() const
{
    return m_precision;
}

qreal SmoothWire::beginMargin() const
{
    return m_beginMargin;
}

qreal SmoothWire::endMargin() const
{
    return m_endMargin;
}

Groove *SmoothWire::guide() const
{
    return m_guide;
}



void SmoothWire::setPen(Pen *arg)
{
    if (m_pen == arg)
        return;
    if (m_pen)
        m_pen->disconnect(0, this, 0);
    m_pen = arg;
    connect(m_pen, SIGNAL(penChanged()), this, SLOT(rebuild()));
    emit penChanged();
    build();
}

void SmoothWire::setOffset(qreal arg)
{
    if (m_offset == arg)
        return;

    m_offset = arg;
    build();
    emit offsetChanged();
}

void SmoothWire::setRadius(qreal arg)
{
    if (m_radius == arg)
        return;

    m_radius = arg;
    build();
    emit radiusChanged();
}

void SmoothWire::setPrecision(qreal arg)
{
    if (m_precision == arg)
        return;

    m_precision = arg;
    build();
    emit precisionChanged();
}

void SmoothWire::setBeginMargin(qreal arg)
{
    if (m_beginMargin == arg)
        return;

    m_beginMargin = arg;
    build();
    emit beginMarginChanged(arg);
}

void SmoothWire::setEndMargin(qreal arg)
{
    if (m_endMargin == arg)
        return;
    m_endMargin = arg;
    build();
    emit endMarginChanged(arg);
}

void SmoothWire::setGuide(Groove *arg)
{
    if (m_guide == arg)
        return;
    if (m_guide) {
        m_guide->disconnect(0, this, 0);
    }

    m_guide = arg;

    connect(m_guide, SIGNAL(polygonChanged()), this, SLOT(rebuild()));
    rebuild();
    emit guideChanged();
}

Groove *SmoothWire::result() const
{
    return m_result;
}




void SmoothWire::appendRib(Groove *guide, qreal inPosition, qreal outPosition, QVariant data)
{
//    qDebug()<< "adding rib" << guide->length() << inPosition << outPosition << data;
    if (!m_guide)
        setGuide(new Groove(this));

    m_guide->vectorPath().addVectorPath(guide->vectorPath().getPart(inPosition, outPosition), data);
}

SmoothWire *SmoothWire::split(qreal pos)
{
    Groove *rest = m_guide->split(pos);
    SmoothWire *wire = new SmoothWire(parentItem());
    wire->setGuide(rest);
    wire->setPen(pen());
    wire->setRadius(radius());
    wire->setOffset(offset());
    wire->setPrecision(precision());
    wire->setScene(scene());
    return wire;
}
const VectorPath & SmoothWire::roundedPath() const
{
    return m_roundedPath;
}


const VectorPath &SmoothWire::offsetPath() const
{
    return m_offsetPath;
}


void SmoothWire::build()
{
    if (!m_guide || !scene() || !m_pen || m_guide->vectorPath().isNull())
        return;
    effectiveRadius = m_radius * qSqrt(zoomFactor());
    if (m_radius > 0)
        m_roundedPath = m_guide->vectorPath().buildRounding(effectiveRadius, 50 * zoomFactor() / m_precision, m_beginMargin, qMin(m_guide->length() - m_endMargin, m_guide->length()));
    else
        m_roundedPath = m_guide->vectorPath().getPart(m_beginMargin, m_guide->length() - m_endMargin);
    if (m_offset > 0)
        m_offsetPath = m_roundedPath.buildOffset(m_pen->width() * m_offset * zoomFactor());
    else
        m_offsetPath = m_roundedPath;
    polygon = m_offsetPath.toPolygon();
    qDebug() << "Generate smoothWire" << m_guide->length() << "polygon size" << polygon.size();
    item()->setBoundingRectDirty();
    emit resultChanged();
    update();
}

QPainterPath SmoothWire::shape() const
{
    QPainterPath p;
    qreal z = 0.5 / scene()->viewportScale();
    VectorPath lp = m_offsetPath.buildOffset(outlineMargin() * z);
    VectorPath rp = m_offsetPath.buildOffset(-outlineMargin() * z);
    QPolygonF pol1 = lp.toPolygon();
    QPolygonF pol2 = rp.reversed().toPolygon();
    QPolygonF pol = pol1 + pol2;

    if( pol1.isEmpty() )
        qDebug() << "!!!! somehow pol1 is empty";
    else
        pol.append(pol1.first());

    p.addPolygon(pol);

    return p;
    //    return buildOutline(p, m_pen->pen());
}

void SmoothWire::paint(QPainter *painter)
{
    if (!m_pen)
        return;
    painter->setPen(m_pen->pen());
    painter->drawPolyline(polygon);
    if (debugNodes())
        drawPolygonNodes(painter, polygon, m_pen->pen());

}

QRectF SmoothWire::getBoundingRect() const
{
    return polygon.boundingRect();
}



