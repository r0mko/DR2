#ifndef SHAPEINSTANCE_H
#define SHAPEINSTANCE_H

#include "visualinstance.h"
#include "gpen.h"
#include "gbrush.h"

class ShapeInstance: public VisualInstance
{
    GR_INSTANCE(ShapeInstance)

public:
    using VisualInstance::VisualInstance;
    ShapeInstance(QGraphicsItem * parent = nullptr) : VisualInstance( parent ) {}
    virtual ~ShapeInstance() {}

protected:
    GPen pen;
    GBrush brush;
    int shapeType = 2;
};


class ShapeInstanceHandle: public VisualInstanceHandle
{
    Q_GADGET
    Q_ENUMS(ShapeType)

    Q_PROPERTY(GPen pen READ pen WRITE setPen)
    Q_PROPERTY(GBrush brush READ brush WRITE setBrush)
    Q_PROPERTY(int shapeType READ shapeType WRITE setShapeType)

    GR_HANDLE(ShapeInstance)
public:
    enum ShapeType {
        Invalid   = 0,
        Linear    = 1,
        Polygonal = 2
    };
    Q_ENUM(ShapeType)
    using VisualInstanceHandle::VisualInstanceHandle;

    GPen pen() const;
    void setPen(GPen pen);

    GBrush brush() const;
    void setBrush(GBrush brush);

    int shapeType() const;
    void setShapeType(int shapeType);
};

DECLARE_ITEM(ShapeInstance)

#endif // SHAPEINSTANCE_H
