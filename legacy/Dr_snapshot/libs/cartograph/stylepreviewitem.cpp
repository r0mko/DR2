#include "stylenode.h"
#include "stylepreviewitem.h"
#include "triangulator.h"

StylePreviewItem::StylePreviewItem(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlags(QQuickItem::ItemHasContents);
    createGeometry();
}


int StylePreviewItem::shapeSides() const
{
    return m_shapeSides;
}

void StylePreviewItem::setShapeSides(int shapeSides)
{
    if (m_shapeSides == shapeSides)
        return;

    m_shapeSides = shapeSides;
    emit shapeSidesChanged(shapeSides);
    createGeometry();
    update();
}

Style StylePreviewItem::style() const
{
    return m_style;
}

void StylePreviewItem::setStyle(Style style)
{
//    if (style == m_style) {
//        update();
//        return;
//    }
//    m_style.setDeleted(true);
    m_style = style;
    m_style.init(geometry);
    update();
}


QSGNode *StylePreviewItem::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
//    qDebug() << Q_FUNC_INFO << "update paint node" << node;
    if (!geometry.vertexCount())
        createGeometry();
//    qDebug() << "update paint node" << boundingRect() << geometry.vertexCount() << geometry.lineIndexCount() << m_style.isValid();
    QSGNode *updatedStyle = m_style.updateNode(node, geometry);
//    qDebug() << Q_FUNC_INFO << "result paint node" << updatedStyle;

    return updatedStyle;
}

void StylePreviewItem::geometryChanged(const QRectF &ng, const QRectF &og)
{
    QQuickItem::geometryChanged(ng, og);
    createGeometry();
}

void StylePreviewItem::createGeometry()
{
    geometry.clear();
    QPainterPath shape;
    QPainterPath guide;
    QRectF rect = boundingRect();
    if (rect.isEmpty()) {
//        qDebug() << "empty rect!";
        return;
    }
    qreal margin = qMax(rect.width(), rect.height()) / 20;
    rect = rect.marginsRemoved(QMarginsF(margin, margin, margin, margin));
    for (int i = 0; i < m_shapeSides; ++i) {
        guide.arcMoveTo(rect, (360 / m_shapeSides) * i);
        if (i == 0)
            shape.moveTo(guide.currentPosition());
        else
            shape.lineTo(guide.currentPosition());
    }

    shape.closeSubpath();
    geometry = triangulate(shape);
    geometry.addPolyLines(shape.toSubpathPolygons());
}
