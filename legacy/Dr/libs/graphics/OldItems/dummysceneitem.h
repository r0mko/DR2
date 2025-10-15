#ifndef DUMMYSCENEITEM_H
#define DUMMYSCENEITEM_H
#include "sceneitem.h"
#include "graphicsitem.h"

class DummySceneItem : public SceneItem
{
    Q_OBJECT
    class DummyItem : public QuickGraphicsItem
    {
    public:
        DummyItem(SceneItem *item, QGraphicsItem *parent = 0);
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        QRectF calculateBoundingRect() const;
    };
    PROTO_ITEM(DummyItem)

public:
    explicit DummySceneItem(SceneItem *parent = 0);

signals:

public slots:

};

#endif // DUMMYSCENEITEM_H
