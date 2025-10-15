#include "shapesceneitem.h"
#include <QPainter>
#include "pen.h"
#include "brush.h"
#include "scene.h"
#include "shapesceneitem.h"
#include <QDebug>
#include <QGraphicsSceneEvent>
#include <QtMath>
ShapeSceneItem::ShapeSceneItem(SceneItem *parent) :
    SceneItem(new GraphicsPathItem(this), parent)
{
}

Pen *ShapeSceneItem::pen() const
{
    return item()->pen();
}

Brush *ShapeSceneItem::brush() const
{
    return item()->brush();
}

qreal ShapeSceneItem::minimumWidth() const
{
    return item()->minimumWidth();
}

bool ShapeSceneItem::drawOutline() const
{
    return item()->drawOutline;
}

Pen *ShapeSceneItem::outlinePen() const
{
    return item()->outlinePen;
}

Brush *ShapeSceneItem::outlineBrush() const
{
    return item()->outlineBrush;
}


void ShapeSceneItem::setPen(Pen *arg)
{
    if (item()->pen() == arg)
        return;
    if (item()->pen()) {
        item()->pen()->disconnect(0, this, 0);
    }

    item()->setPen(arg);
    connect(item()->pen(), SIGNAL(penChanged()), this, SLOT(update()));
    emit penChanged(arg);
}

void ShapeSceneItem::setBrush(Brush *arg)
{
    if (item()->brush() == arg)
        return;

    item()->setBrush(arg);
    emit brushChanged(arg);
}

void ShapeSceneItem::setMinimumWidth(qreal arg)
{
    if (item()->minimumWidth() == arg)
        return;

    item()->setMinimumWidth(arg);
    emit minimumWidthChanged(arg);
}

void ShapeSceneItem::setDrawOutline(bool arg)
{
    if (item()->drawOutline == arg)
        return;

    item()->drawOutline = arg;
    emit drawOutlineChanged(arg);
}

void ShapeSceneItem::setOutlinePen(Pen *arg)
{
    if (item()->outlinePen == arg)
        return;

    item()->outlinePen = arg;
    emit outlinePenChanged(arg);
}

void ShapeSceneItem::setOutlineBrush(Brush *arg)
{
    if (item()->outlineBrush == arg)
        return;
    item()->outlineBrush = arg;
    emit outlineBrushChanged(arg);
}



void ShapeSceneItem::beforeDraw(QPainter *painter)
{
    Q_UNUSED(painter)
}

void ShapeSceneItem::afterDraw(QPainter *painter)
{
    Q_UNUSED(painter)
}

void ShapeSceneItem::setBoundingRectDirty()
{
    item()->setBoundingRectDirty();
}

void ShapeSceneItem::setPath(const QPainterPath &path)
{
    item()->setPath(path);
    item()->setBoundingRectDirty();
    item()->setShapeDirty();
}

QPainterPath &ShapeSceneItem::pathRef()
{
    return item()->pathRef();
}

const QPainterPath &ShapeSceneItem::pathRef() const
{
    return item()->pathRef();
}

ShapeSceneItem::GraphicsPathItem::GraphicsPathItem(SceneItem *item, QGraphicsItem *parent) :
    QuickGraphicsShapeItem(item, parent)
{

}

void ShapeSceneItem::GraphicsPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (drawOutline) {
        if (outlineBrush)
            painter->fillPath(shape(), outlineBrush->brush());
        if (outlinePen)
            painter->strokePath(shape(), outlinePen->pen());
    }
    sup->beforeDraw(painter);
    if (brush())
        painter->fillPath(path(), brush()->brush());
    if (pen())
        painter->strokePath(path(), pen()->pen());
    sup->afterDraw(painter);
}









