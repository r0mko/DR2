#include "dgshape.h"

DGShape::DGShape(QObject *parent) :
    QObject(parent),
    m_path(QPainterPath()),
    m_pen(0),
    m_brush(0),
    lastElementCount(0),
    lastSubpathCount(0)
{

}

DGShape::DGShape(QPainterPath path, DGPen *pen, DGBrush *brush)
    : m_path(path),
      m_pen(pen),
      m_brush(brush)
{
    m_path = path;
    updatePolygons();
}

QPainterPath DGShape::path() const
{
    return m_path;
}

void DGShape::setPath(const QPainterPath &path)
{
    qDebug() << "DGShape: Path updated";
    m_path = path;
    notifyPathUpdate();
}

bool DGShape::polygonCache() const
{
    return m_polygonCache;
}

int DGShape::subpathCount() const
{
    return m_pathPolygons.size();
}

QRectF DGShape::boundingRect() const
{
    return m_boundingRect;
}

void DGShape::updatePolygons()
{
    m_pathPolygons = m_path.toSubpathPolygons();
    m_fillPolygons = m_path.toFillPolygons();
    if (lastSubpathCount != m_pathPolygons.size()) {
        lastSubpathCount = m_pathPolygons.size();
        emit subpathCountChanged(lastSubpathCount);
    }
}

void DGShape::notifyPathUpdate()
{
    if (!batch) {
        if (lastElementCount != m_path.elementCount()) {
            lastElementCount = m_path.elementCount();
            emit elementCountChanged(lastElementCount);
        }
        QRectF rect = m_path.boundingRect();
        if (rect != m_boundingRect) {
            m_boundingRect = rect;
            emit boundingRectChanged(m_boundingRect);
        }
        emit pathChanged();
    }
    else
        pathDirty = true;
}

//QList<QPolygonF> DGShape::pathPolygons() const
//{
//    return m_pathPolygons;
//}

DGPen *DGShape::pen() const
{
    return m_pen;
}

DGBrush *DGShape::brush() const
{
    return m_brush;
}

int DGShape::elementCount() const
{
    return m_path.elementCount();
}

void DGShape::setPolygonCache(bool arg)
{
    if (m_polygonCache != arg) {
        m_polygonCache = arg;
        emit polygonCacheChanged(arg);
    }
}

bool DGShape::hasFill() const
{
    return m_brush ? m_brush->visible() : false;
}

bool DGShape::hasStroke() const
{
    return m_pen ? m_pen->visible() : false;
}

void DGShape::componentComplete()
{
    if (!m_pen)
        m_pen = new DGPen(this);
    if (!m_brush)
        m_brush = new DGBrush(this);
}

void DGShape::begin()
{
    pathDirty = false;
    batch = true;
}

void DGShape::end()
{
    batch = false;
    if (pathDirty) {
        notifyPathUpdate();
        pathDirty = false;
    }
}

void DGShape::close()
{
    m_path.closeSubpath();
    notifyPathUpdate();
}

void DGShape::moveTo(qreal x, qreal y)
{
    m_path.moveTo(x, y);
    notifyPathUpdate();

}

void DGShape::lineTo(qreal x, qreal y)
{
    m_path.lineTo(x, y);
    notifyPathUpdate();
}

void DGShape::cubicTo(qreal cp1x, qreal cp1y, qreal cp2x, qreal cp2y, qreal x, qreal y)
{
    m_path.cubicTo(cp1x, cp1y, cp2x, cp2y, x, y);
    notifyPathUpdate();

}

void DGShape::scale(qreal scaleX, qreal scaleY, QPointF origin)
{
    QPainterPath p = m_path.translated(-origin);
    for (int i = 0; i < p.elementCount(); ++i) {
        QPainterPath::Element e = p.elementAt(i);
        m_path.setElementPositionAt(i, e.x * scaleX + origin.x(), e.y * scaleY + origin.y());
    }
    notifyPathUpdate();
}

void DGShape::translate(qreal dx, qreal dy)
{
    m_path.translate(dx, dy);
    notifyPathUpdate();
}

void DGShape::setElementPosition(int elementIndex, qreal x, qreal y)
{
    m_path.setElementPositionAt(elementIndex, x, y);
    notifyPathUpdate();

}

DGShape::ElementType DGShape::getElementType(int elementIndex) const
{
    return static_cast<DGShape::ElementType>(m_path.elementAt(elementIndex).type);
}

QPointF DGShape::getElementPos(int elementIndex) const
{
    const QPainterPath::Element &el = m_path.elementAt(elementIndex);
    return QPointF(el.x, el.y);
}

void DGShape::setPen(DGPen *arg)
{
    if (m_pen != arg) {
        m_pen = arg;
        emit penChanged(arg);
        notifyPathUpdate();
    }
}

void DGShape::setBrush(DGBrush *arg)
{
    if (m_brush != arg) {
        m_brush = arg;
        emit brushChanged(arg);
        notifyPathUpdate();
    }
}

//QList<QPolygonF> DGShape::fillPolygons() const
//{
//    return m_fillPolygons;
//}

qreal DGPen::width() const
{
    return m_pen.widthF();
}

QColor DGPen::color() const
{
    return m_pen.color();
}

bool DGPen::cosmetic() const
{
    return m_pen.isCosmetic();
}

Qt::PenStyle DGPen::strokeStyle() const
{
    return m_pen.style();
}

Qt::PenJoinStyle DGPen::joinStyle() const
{
    return m_pen.joinStyle();
}

Qt::PenCapStyle DGPen::capStyle() const
{
    return m_pen.capStyle();
}

QPen DGPen::pen() const
{
    return m_pen;
}

void DGPen::setWidth(qreal arg)
{
    if (m_pen.widthF() != arg) {
        m_pen.setWidthF(arg);
        emit widthChanged(arg);
    }
}

void DGPen::setColor(QColor arg)
{
    if (m_pen.color()!= arg) {
        m_pen.setColor(arg);
        emit colorChanged(arg);
    }
}

void DGPen::setCosmetic(bool arg)
{
    if (m_pen.isCosmetic() != arg) {
        m_pen.setCosmetic(arg);
        emit cosmeticChanged(arg);
    }
}

void DGPen::setStrokeStyle(Qt::PenStyle arg)
{
    if (m_pen.style() != arg) {
        m_pen.setStyle(arg);
        emit strokeStyleChanged(arg);
        if (m_visible != (arg != Qt::NoPen)) {
            m_visible = (arg != Qt::NoPen);
            emit visibleChanged(m_visible);
        }
    }
}

void DGPen::setJoinStyle(Qt::PenJoinStyle arg)
{
    if (m_pen.joinStyle() != arg) {
        m_pen.setJoinStyle(arg);
        emit joinStyleChanged(arg);
    }
}

void DGPen::setCapStyle(Qt::PenCapStyle arg)
{
    if (m_pen.capStyle() != arg) {
        m_pen.setCapStyle(arg);
        emit capStyleChanged(arg);
    }
}

qreal DGPen::dashOffset() const
{
    return m_pen.dashOffset();
}

void DGPen::setDashOffset(qreal arg)
{
    if (m_pen.dashOffset() != arg) {
        m_pen.setDashOffset(arg);
        emit dashOffsetChanged(arg);
    }
}

bool DGPen::visible() const
{
    return m_visible;
}


QBrush DGBrush::brush() const
{
    return m_brush;
}

QColor DGBrush::color() const
{
    return m_brush.color();
}

Qt::BrushStyle DGBrush::style() const
{
    return m_brush.style();
}

void DGBrush::setColor(QColor arg)
{
    if (m_brush.color() != arg) {
        m_brush.setColor(arg);
        emit colorChanged(arg);
    }
}

void DGBrush::setStyle(Qt::BrushStyle arg)
{
    if (m_brush.style() != arg) {
        m_brush.setStyle(arg);
        emit styleChanged(arg);
        if (m_visible != (arg != Qt::NoBrush)) {
            m_visible = arg != Qt::NoBrush;
            emit visibleChanged(m_visible);
        }
    }
}

bool DGBrush::visible() const
{
    return m_visible;
}
