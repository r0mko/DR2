#ifndef ABSTRACTGUIDEITEM_H
#define ABSTRACTGUIDEITEM_H
#include "visualinstance.h"
#include "guidepath.h"
#include <gpen.h>

class AbstractGuideItem : public VisualInstance
{
    GR_INSTANCE(AbstractGuideItem)
public:
    using VisualInstance::VisualInstance;
    GuidePath source;
    GuidePath result;
    qreal smooth = 0;
    qreal offset = 0;
    qreal precision = 1;
    qreal sourceStart = 0;
    qreal sourceEnd = -1;
    bool fast = false;

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void updatePath(bool doUpdate = true) ;

};

class AbstractGuideItemHandle: public VisualInstanceHandle
{
    Q_GADGET
    GR_HANDLE(AbstractGuideItem)
    Q_PROPERTY(GuidePath result READ result)
    Q_PROPERTY(GuidePath source READ source WRITE setSource)
    Q_PROPERTY(qreal smooth READ smooth WRITE setSmooth)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset)
    Q_PROPERTY(qreal precision READ precision WRITE setPrecision)
    Q_PROPERTY(qreal sourceStart READ sourceStart WRITE setSourceStart)
    Q_PROPERTY(qreal sourceEnd READ sourceEnd WRITE setSourceEnd)
    Q_PROPERTY(bool fast READ fast WRITE setFast)

public:
    using VisualInstanceHandle::VisualInstanceHandle;

    GuidePath result() const;
    void setResult(GuidePath result);

    GuidePath source() const;
    void setSource(GuidePath source);

    qreal smooth() const;
    void setSmooth(qreal smooth);

    qreal offset() const;
    void setOffset(qreal offset);

    qreal precision() const;
    void setPrecision(qreal precision);

    qreal sourceStart() const;
    void setSourceStart(qreal sourceStart);

    qreal sourceEnd() const;
    void setSourceEnd(qreal sourceEnd);

    Q_INVOKABLE void updatePath();

    bool fast() const;
    void setFast(bool fast);
};
DECLARE_ITEM(AbstractGuideItem)

#endif // ABSTRACTGUIDEITEM_H
