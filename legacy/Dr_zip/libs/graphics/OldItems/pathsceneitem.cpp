#include "pathsceneitem.h"
#include <QPainterPath>
#include <QPainter>
#include <QDebug>
#include <QPainterPathStroker>
#include "pen.h"
#include "brush.h"
#include <scene.h>

PathSceneItem::PathSceneItem(SceneItem *parent) :
    SceneItem(new PathGraphicsItem(this), parent)
{

}

GraphicsPath *PathSceneItem::path() const
{
    return item()->m_path;
}

Pen *PathSceneItem::pen() const
{
    return item()->pen();
}

Brush *PathSceneItem::brush() const
{
    return item()->brush();
}

void PathSceneItem::componentComplete()
{
}

void PathSceneItem::setPath(GraphicsPath *arg)
{
    if (item()->m_path == arg)
        return;
    if (item()->m_path) {
        item()->m_path->disconnect(0, this, 0);
    }

    item()->m_path = arg;
    item()->setOpaqueAreaDirty();
    connect(arg, SIGNAL(pathChanged()), this, SLOT(updateOutline()));
    connect(arg, SIGNAL(pathChanged()), this, SLOT(update()));
    emit pathChanged(arg);
}

void PathSceneItem::setPen(Pen *arg)
{
    if (item()->pen() == arg)
        return;
    if (item()->pen())
        item()->pen()->disconnect(0, this, 0);
    item()->setPen(arg);
    connect(item()->pen(), SIGNAL(penChanged()), this, SLOT(update()));
    connect(item()->pen(), SIGNAL(widthChanged(qreal)), this, SLOT(updateOutline()));
    emit penChanged(arg);
}

void PathSceneItem::setBrush(Brush *arg)
{
    if (item()->brush() == arg)
        return;
    item()->setBrush(arg);
    connect(item()->brush(), SIGNAL(brushChanged()), this, SLOT(update()));
    emit brushChanged(arg);
}

void PathSceneItem::updateOutline()
{
    item()->setBoundingRectDirty();
    item()->setOpaqueAreaDirty(true);
}

PathSceneItem::PathGraphicsItem::PathGraphicsItem(SceneItem *item, QGraphicsItem *parent) :
    QuickGraphicsShapeItem(item, parent),
    m_path(nullptr)
{}

QPainterPath PathSceneItem::PathGraphicsItem::shape() const
{
    if (opaqueAreaDirty)
        updateOutline();
    return m_opaqueArea;
}

QPainterPath PathSceneItem::PathGraphicsItem::opaqueArea() const
{
    if (opaqueAreaDirty)
        updateOutline();
    return m_opaqueArea;
}

bool PathSceneItem::PathGraphicsItem::isOpaqueAreaDirty() const
{
    return opaqueAreaDirty;
}

void PathSceneItem::PathGraphicsItem::setOpaqueAreaDirty(bool value)
{
    opaqueAreaDirty = value;
}

void PathSceneItem::PathGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    painter->save();
    if(!m_path)
        return;
    if (pen())
        painter->setPen(pen()->pen());
    if (brush())
        painter->setBrush(brush()->brush());
//    QList<QPolygonF> pols = path->path().toSubpathPolygons();
//    for (const QPolygonF &pol:pols) {
//        painter->drawPoints(pol);
//    }
    painter->drawPath(m_path->path());
    painter->restore();

    //    QPen p;
    //    p.setColor("#00ffff");
    //    p.setWidth( 1 );
    //    p.setCosmetic(true);
    //    QPainterPathStroker s;
    //    s.setWidth( 2 );
    //    s.setJoinStyle(Qt::MiterJoin);
    //    painter->strokePath( ( s.createStroke( shape() ) + shape() ).simplified(), p);
}

QRectF PathSceneItem::PathGraphicsItem::calculateBoundingRect() const
{
    if (m_path)
        return m_path->boundingRect();
    else
        return QRectF();
}




void PathSceneItem::PathGraphicsItem::updateOutline() const
{

    if (brush())
        m_opaqueArea = m_path->path();
    else {
        QPainterPathStroker str;
        str.setWidth(pen()->width());
        str.setCapStyle(pen()->capStyle());
        str.setJoinStyle(pen()->joinStyle());
        str.setMiterLimit(pen()->pen().miterLimit());
        str.setDashOffset(pen()->pen().dashOffset());
        str.setDashPattern(pen()->pen().dashPattern());
        m_opaqueArea = str.createStroke(m_path->path());
    }
    opaqueAreaDirty = false;
}
