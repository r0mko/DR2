#ifndef GUIDEPATHSTROKE_H
#define GUIDEPATHSTROKE_H

#include "abstractguideitem.h"

class GuidePathStroke : public AbstractGuideItem
{
    GR_INSTANCE(GuidePathStroke)
public:
    using AbstractGuideItem::AbstractGuideItem;
    GPen pen;
    GPen sourcePen;
    qreal sourceStrokeStart = 0;
    qreal sourceStrokeEnd = -1;
    qreal strokeStart = 0;
    qreal strokeEnd = -1;
    bool drawSource = false;
    bool drawResult = true;
    bool debugNodes = false;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    int type() const;
private:
    void paintGuide(QPainter *painter, const GuidePath &path, qreal start, qreal end, const QPen &pen) const;
};


class GuidePathStrokeHandle : public AbstractGuideItemHandle
{
    Q_GADGET
    GR_HANDLE(GuidePathStroke)
    Q_PROPERTY(GPen pen READ pen WRITE setPen)
    Q_PROPERTY(GPen sourcePen READ sourcePen WRITE setSourcePen)

    Q_PROPERTY(bool drawResult READ drawResult WRITE setDrawResult)
    Q_PROPERTY(bool drawSource READ drawSource WRITE setDrawSource)

    Q_PROPERTY(qreal sourceStrokeStart READ sourceStrokeStart WRITE setSourceStrokeStart)
    Q_PROPERTY(qreal sourceStrokeEnd READ sourceStrokeEnd WRITE setSourceStrokeEnd)

    Q_PROPERTY(qreal strokeStart READ strokeStart WRITE setStrokeStart)
    Q_PROPERTY(qreal strokeEnd READ strokeEnd WRITE setStrokeEnd)



public:
    using AbstractGuideItemHandle::AbstractGuideItemHandle;

    GPen pen() const;
    void setPen(GPen pen);

    GPen sourcePen() const;
    void setSourcePen(GPen sourcePen);

    bool drawResult() const;
    void setDrawResult(bool drawResult);

    bool drawSource() const;
    void setDrawSource(bool drawSource);


    qreal sourceStrokeStart() const;
    void setSourceStrokeStart(qreal sourceStrokeStart);

    qreal sourceStrokeEnd() const;
    void setSourceStrokeEnd(qreal sourceStrokeEnd);

    qreal strokeStart() const;
    void setStrokeStart(qreal strokeStart);

    qreal strokeEnd() const;
    void setStrokeEnd(qreal strokeEnd);


    Q_INVOKABLE void setDebugMode(bool debug);

};

DECLARE_ITEM(GuidePathStroke)

#endif // GUIDEPATHSTROKE_H
