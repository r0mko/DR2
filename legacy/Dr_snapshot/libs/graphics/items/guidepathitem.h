#ifndef GUIDEPATHITEM_H
#define GUIDEPATHITEM_H
#include "visualinstance.h"

#include <guidepath.h>

class GuidePathItem : public VisualInstance
{
    GR_INSTANCE(GuidePathItem)

public:
    GuidePathItem(QGraphicsItem * parent = nullptr) : VisualInstance( parent ) {}
    GuidePath path;
    QRectF boundingRect() const;
    QPainterPath shape() const;
};

#endif // GUIDEPATHITEM_H
