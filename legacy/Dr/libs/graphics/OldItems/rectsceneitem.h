#ifndef GRAPHICSRECTITEM_H
#define GRAPHICSRECTITEM_H
#include "sceneitem.h"
#include "graphicsitem.h"

class Pen;
class Brush;
class RectSceneItem : public SceneItem
{
    Q_OBJECT
    Q_PROPERTY(Pen *pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(Brush *brush READ brush WRITE setBrush NOTIFY brushChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)

    class GraphicsRectItem : public QuickGraphicsShapeItem
    {
    public:
        GraphicsRectItem(SceneItem *item, QGraphicsItem *parent = 0);
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


        QRectF *rect;
        qreal radius;
        QRectF calculateBoundingRect() const;
    };

    PROTO_ITEM(GraphicsRectItem)


public:
    RectSceneItem(SceneItem *parent = 0);
    QRectF boundingRect() const;

    Pen *pen() const;
    Brush *brush() const;

    qreal radius() const;

public slots:
    void setPen(Pen *arg);
    void setBrush(Brush *arg);
    void setRadius(qreal arg);

signals:
    void penChanged(Pen *arg);
    void brushChanged(Brush *arg);

    void radiusChanged(qreal arg);
};
#endif // GRAPHICSRECTITEM_H
