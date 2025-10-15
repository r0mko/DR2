#include "rectsceneitem.h"
#include "qdebug.h"
#include "qstyleoption.h"
#include "qpainter.h"
#include "pen.h"
#include "brush.h"
#include "scene.h"
RectSceneItem::RectSceneItem(SceneItem *parent) :
    SceneItem(new GraphicsRectItem(this), parent)
{
    item()->rect = const_cast<QRectF*>(rectPtr());


    connect(this, SIGNAL(rectChanged(QRectF)), this, SLOT(update()));
}

QRectF RectSceneItem::boundingRect() const
{
    return rect();
}


Pen *RectSceneItem::pen() const
{
    return item()->pen();
}

Brush *RectSceneItem::brush() const
{
    return item()->brush();
}

qreal RectSceneItem::radius() const
{
    return item()->radius;
}

void RectSceneItem::setPen(Pen *arg)
{
    if (item()->pen() == arg)
        return;
    if (item()->pen())
        item()->pen()->disconnect(0, this, 0);
    item()->setPen(arg);
    connect(item()->pen(), SIGNAL(penChanged()), this, SLOT(update()));
    emit penChanged(arg);
}

void RectSceneItem::setBrush(Brush *arg)
{
    if (item()->brush() == arg)
        return;
    item()->setBrush(arg);
    connect(item()->brush(), SIGNAL(brushChanged()), this, SLOT(update()));
    emit brushChanged(arg);
}

void RectSceneItem::setRadius(qreal arg)
{
    if (item()->radius == arg)
        return;

    item()->radius = arg;
    item()->update(rect());
    emit radiusChanged(arg);
}


RectSceneItem::GraphicsRectItem::GraphicsRectItem(SceneItem *item, QGraphicsItem *parent) :
    QuickGraphicsShapeItem(item, parent),
    rect(nullptr),
    radius(0)
{}

void RectSceneItem::GraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!rect)
        return;
    if(rect->isNull())
        return;

    QRectF fixed;
    if (super()->flags() & SceneItem::ItemPixelAligned) {
        qreal step = 1 / super()->scene()->viewportScale();
        qreal x1, x2, y1, y2;
        rect->getCoords(&x1, &y1, &x2, &y2);
        x1 = qRound(x1 / step) * step;
        y1 = qRound(y1 / step) * step;
        x2 = qRound(x2 / step) * step;
        y2 = qRound(y2 / step) * step;
        fixed.setCoords(x1, y1, x2, y2);
    } else {
        fixed = *rect;
    }

    if (super()->flags() & SceneItem::ItemIgnoresTransformations) {
        QTransform tr = super()->scene()->viewTransform();
        painter->scale(1/tr.m11(), 1/tr.m11());
    }
    if (pen())
        painter->setPen(pen()->pen());
    if (brush())
        painter->setBrush(brush()->brush());
    if (radius > 0)
        painter->drawRoundedRect(fixed, radius, radius);
    else
        painter->drawRect(fixed);
}

QRectF RectSceneItem::GraphicsRectItem::calculateBoundingRect() const
{
    if (super()->flags() & SceneItem::ItemIgnoresTransformations) {
        QTransform tr = super()->scene()->viewTransform();
        qreal scale = 1/tr.m11();
        return QRectF(rect->x() * scale, rect->y() * scale, rect->width() * scale, rect->height() * scale);
    }
    return *rect;
}


