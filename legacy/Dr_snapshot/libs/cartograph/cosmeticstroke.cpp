#include "cosmeticstroke.h"

#include <QSGFlatColorMaterial>




qreal CosmeticStroke::width() const
{
    return _df()->width;
}

QColor CosmeticStroke::color() const
{
    return _df()->color;
}

void CosmeticStroke::setWidth(qreal width)
{
    _df()->width = width;
}

void CosmeticStroke::setColor(QColor color)
{
    _df()->color = color;
}

QSGNode *CosmeticStroke::updateStyleNode(QSGNode *node, const VertexBuffer &buffer)
{
    QSGGeometryNode *n = getGeometryNode(node);
    QSGFlatColorMaterial *m = getMaterial<QSGFlatColorMaterial>(n);
    if (_df()->color != m->color())
        m->setColor(_df()->color);

    if (!geometry())
        setGeometry(buffer.createLineGeometry());
    else
        buffer.updateLineGeometry(geometry());

    if (n->geometry() != geometry())
        n->setGeometry(geometry());

    if (geometry()->lineWidth() != _df()->width)
        geometry()->setLineWidth(_df()->width);

    return n;
}


