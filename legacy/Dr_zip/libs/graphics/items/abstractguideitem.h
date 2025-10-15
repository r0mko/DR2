#ifndef ABSTRACTGUIDEITEM_H
#define ABSTRACTGUIDEITEM_H
#include "visualinstance.h"
#include <guide.h>
#include <gpen.h>

class AbstractGuideItem : public VisualInstance
{
    GR_INSTANCE(AbstractGuideItem)
public:
    using VisualInstance::VisualInstance;
    Guide source;
    Guide result;
    qreal smooth = 0;
    qreal offset = 0;
    qreal precision = 1;
    qreal sourceStart = 0;
    qreal sourceEnd = -1;
    bool fast = false;
    qreal scalingFactor = 1;


    QPainterPath shape() const;
    void updatePath(bool doUpdate = true) ;

};

class AbstractGuideItemHandle: public VisualInstanceHandle
{
    Q_GADGET
    GR_HANDLE(AbstractGuideItem)
    Q_PROPERTY(Guide result READ result)
    Q_PROPERTY(Guide source READ source WRITE setSource)
    Q_PROPERTY(qreal smooth READ smooth WRITE setSmooth)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset)
    Q_PROPERTY(qreal precision READ precision WRITE setPrecision)
    Q_PROPERTY(qreal sourceStart READ sourceStart WRITE setSourceStart)
    Q_PROPERTY(qreal sourceEnd READ sourceEnd WRITE setSourceEnd)
    Q_PROPERTY(bool fast READ fast WRITE setFast)

    Q_PROPERTY(qreal scalingFactor READ scalingFactor WRITE setScalingFactor)

public:
    using VisualInstanceHandle::VisualInstanceHandle;

    Guide result() const;
    void setResult(Guide result);

    Guide source() const;
    void setSource(Guide source);

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


    qreal scalingFactor() const;
    void setScalingFactor(qreal scalingFactor);
};
DECLARE_ITEM(AbstractGuideItem)

#endif // ABSTRACTGUIDEITEM_H
