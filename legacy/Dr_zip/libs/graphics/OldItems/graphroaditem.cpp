#include "graphroaditem.h"
#include "graphroute.h"
#include <QDebug>

GraphRoadItem::GraphRoadItem(SceneItem *parent):
    SceneItem(new GraphRoadGraphicsItem(this), parent)
{

}

GraphRoadItem::~GraphRoadItem()
{
    qDebug() << "GraphRoadItem destroyed";
}

GraphRoute *GraphRoadItem::route() const
{
    return item()->m_route;
}

qreal GraphRoadItem::zoomFactor() const
{
    return item()->m_zoomFactor;
}

bool GraphRoadItem::chamfer() const
{
    return item()->m_chamfer;
}

void GraphRoadItem::clearRibs()
{
    item()->m_route->clear();
    emit ribsChanged();
}

void GraphRoadItem::appendRib(PathGuide *guide, qreal inPosition, qreal outPosition, qreal offset)
{
    item()->m_route->appendRib(guide, inPosition, outPosition, offset);
    connect(guide, SIGNAL(radiusChanged(qreal)), this, SLOT(updatePath()));
    emit ribsChanged();
}

void GraphRoadItem::updatePath()
{
//    if (m_route->ribs().isEmpty() || !pen())
//        return;

//    qreal pos = 0;
//    SpringWire ch;
//    GraphRoute::runner r = m_route->setRunner(pos);
//    WireNode w;
//    QPainterPath p;
//    qreal rad = m_route->ribs().first()->guide()->radius() / m_zoomFactor;
//    if (m_chamfer)
//        w.pt = m_route->getChamferPoint(r.position(), rad, &w.offset);
//    else {
//        w.pt = r.point();
//        w.offset = r.currentRib()->offset();
//    }
//    ch.addNode(w);
//    while (!r.atRouteEnd()) {
//        if (m_chamfer) {
//            qreal dst = qMin(r.legGapAhead(), r.ribGapAhead());
//            rad = r.currentRib()->guide()->radius() / m_zoomFactor;
//            qreal step = rad / 18;
//            if (dst > rad / 2) {
//                r.moveBy(dst - rad / 2);
//                w.pt = m_route->getChamferPoint(r.position(), rad, &w.offset);
//                ch.addNode(w);
//            }
//            r.moveBy(step);
//            qreal pos = r.travel();
//            while ((r.travel() - pos) < rad && (r.legGapBehind() < rad / 2 || r.legGapAhead() < rad / 2)) {
//                w.pt = m_route->getChamferPoint(r.position(), rad, &w.offset);
//                ch.addNode(w);
//                r.moveBy(step);
//                if (r.atRouteEnd())
//                    break;
//            }
//            w.pt = m_route->getChamferPoint(r.position(), rad, &w.offset);
//            ch.addNode(w);
//        } else {
//            r.moveToNextLeg();

//            w.pt = r.point();
//            w.offset = r.currentRib()->offset();
//            ch.addNode(w);
//        }
//    }
//    lines.clear();
//    p.addPolygon(ch.calculateOffset(pen()->width()/m_zoomFactor, &lines));
//    setPath(p);

    return;
}

void GraphRoadItem::setZoomFactor(qreal arg)
{
    if (item()->m_zoomFactor == arg)
        return;

    item()->m_zoomFactor = arg;
    updatePath();
    emit zoomFactorChanged(arg);
}

void GraphRoadItem::setChamfer(bool arg)
{
    qDebug() << item();
    if (item()->m_chamfer == arg)
        return;

    item()->m_chamfer = arg;
    emit chamferChanged(arg);
}

void GraphRoadItem::polyLineMerge(QPolygonF &dst, const QPolygonF &src) const
{
    if (src.isEmpty())
        return;
    if(!dst.isEmpty() && dst.last() == src.first())
        dst.removeLast();
    for (const QPointF &p:src) {
        dst.append(p);
    }
}

QQmlListProperty<RibReference> GraphRoadItem::qmlRibs()
{
    return QQmlListProperty<RibReference>(this, 0,
                                          &GraphRoadItem::qmlRouteCount,
                                          &GraphRoadItem::qmlRouteAt);
}

int GraphRoadItem::qmlRouteCount(QQmlListProperty<RibReference> *list)
{
    GraphRoadItem *o = qobject_cast<GraphRoadItem*>(list->object);
    return o->item()->m_route->size();
}

RibReference *GraphRoadItem::qmlRouteAt(QQmlListProperty<RibReference> *list, int index)
{
    GraphRoadItem *o = qobject_cast<GraphRoadItem*>(list->object);
    return o->item()->m_route->at(index);
}





GraphRoadItem::GraphRoadGraphicsItem::GraphRoadGraphicsItem(SceneItem *item, QGraphicsItem *parent):
    QuickGraphicsShapeItem(item, parent),
//    m_route(new GraphRoute(item)),
    m_chamfer(false)
{
    m_route = new GraphRoute( super() );
    connect(m_route, SIGNAL(routeChanged()), item, SLOT(updatePath()));
}

void GraphRoadItem::GraphRoadGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

QRectF GraphRoadItem::GraphRoadGraphicsItem::calculateBoundingRect() const
{

}
<<<<<<< .mine


GraphRoadItem::GraphRoadItem(SceneItem *parent):QQuickItem( parent)
{

}
=======


GraphRoadItem::GraphRoadItem(SceneItem *parent)
    : SceneItem(new GraphRoadGraphicsItem(this), parent)
{

}
>>>>>>> .r2047
