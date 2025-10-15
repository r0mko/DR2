#include "polygonsceneitem.h"
#include <QPainter>
#include <QPen>
#include "pen.h"
#include "brush.h"
#include "scene.h"
#include <QPainterPathStroker>
#include <QDebug>
PolylineSceneItem::PolylineSceneItem(SceneItem *parent) :
    ShapeSceneItem(parent),
    m_polygon(nullptr)
{
}

GPolygon *PolylineSceneItem::polygon() const
{
    return m_polygon;
}


void PolylineSceneItem::setPolygon(GPolygon *arg)
{
    if (m_polygon == arg)
        return;
    if (m_polygon)
        m_polygon->disconnect(0, this, 0);
    m_polygon = arg;
    connect(m_polygon, &GPolygon::boundingRectChanged, [&]() {
        setBoundingRectDirty();
    });
    connect(m_polygon, &GPolygon::polygonChanged, this, &PolylineSceneItem::updatePath);
    updatePath();
}

void PolylineSceneItem::afterDraw(QPainter *painter)
{
    qreal s = scene()->viewportScale();
    if (s < 4)
        return;
    QList<QPolygonF> pl = pathRef().toSubpathPolygons();
    QPen linePen;
    linePen.setWidth(pen()->width() * 2);
    linePen.setCosmetic(true);
    linePen.setColor(QColor(0,255,255));
    QFont f;
    f.setPointSizeF(11);
    painter->setFont(f);
    int i = 0;
    for (QPolygonF pol:pl) {
        painter->setPen(linePen);
        painter->drawPoints(pol);
        painter->setPen(Qt::white);
        //        painter->setBrush();
        painter->save();
        painter->scale(1/s, 1/s);
        for (QPointF pt:pol) {
            painter->drawText(pt * s, QString::number(i++));
        }
        painter->restore();;
    }
}

void PolylineSceneItem::updatePath()
{
    QPainterPath p;
    p.addPolygon(m_polygon->polygon());
    setPath(p);

}

