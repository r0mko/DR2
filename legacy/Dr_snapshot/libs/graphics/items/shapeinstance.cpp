#include "shapeinstance.h"

GPen ShapeInstanceHandle::pen() const
{
    chkItem(GPen());
    return item()->pen;
}

GBrush ShapeInstanceHandle::brush() const
{
    chkItem(GBrush());
    return item()->brush;
}

void ShapeInstanceHandle::setPen(GPen pen)
{
    chkItem();
    item()->pen = pen;
    item()->update();
}

void ShapeInstanceHandle::setBrush(GBrush brush)
{
    chkItem();
    item()->brush = brush;
    item()->update();
}

int ShapeInstanceHandle::shapeType() const
{
    chkItem(Invalid);
    return (ShapeInstanceHandle::ShapeType)item()->shapeType;
}

void ShapeInstanceHandle::setShapeType(int type)
{
    chkItem();
    item()->shapeType = type;
}
