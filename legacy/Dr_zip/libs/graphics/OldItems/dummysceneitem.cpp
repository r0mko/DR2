#include "dummysceneitem.h"

DummySceneItem::DummySceneItem(SceneItem *parent) :
    SceneItem(new DummyItem(this), parent)
{
    item()->setFlags(QGraphicsItem::ItemHasNoContents);
}


DummySceneItem::DummyItem::DummyItem(SceneItem *item, QGraphicsItem *parent) :
    QuickGraphicsItem(item, parent)
{

}

void DummySceneItem::DummyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

QRectF DummySceneItem::DummyItem::calculateBoundingRect() const
{
    return super()->rect();
}
