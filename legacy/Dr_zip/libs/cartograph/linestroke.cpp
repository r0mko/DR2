#include "linestroke.h"

#include <QSGFlatColorMaterial>


qreal LineStroke::width() const
{
    return _df()->triangulator.width();
}

qreal LineStroke::offset() const
{
    return _df()->triangulator.offset();
}

QColor LineStroke::color() const
{
    return _df()->color;
}

void LineStroke::setWidth(qreal width)
{
    _df()->triangulator.setWidth(width);
}

void LineStroke::setOffset(qreal offset)
{
    _df()->triangulator.setOffset(offset);
}

void LineStroke::setColor(QColor color)
{
    _df()->color = color;
}

void LineStroke::initBuffer(const VertexBuffer &vb)
{
    for (QPolygonF poly : vb.generatePolygons()) {
        _df()->triangulator.setVertexBuffer(&m_buffer);
        _df()->triangulator.triangulate(poly);
    }
    StyleNode::initBuffer(m_buffer);
}

QSGNode *LineStroke::updateStyleNode(QSGNode *node, const VertexBuffer &)
{
    QSGGeometryNode *n = getGeometryNode(node);
    QSGFlatColorMaterial *m = getMaterial<QSGFlatColorMaterial>(n);
    if (m->color() != _df()->color)
        m->setColor(_df()->color);
    if (!geometry())
        setGeometry(m_buffer.createFillGeometry());
    else
        m_buffer.updateFillGeometry(geometry());
    if (n->geometry() != geometry())
        n->setGeometry(geometry());
    return n;
}
