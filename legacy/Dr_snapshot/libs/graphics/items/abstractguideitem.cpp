#include "abstractguideitem.h"
#include <QGraphicsScene>
#include <guideoffsetter.h>

Guide AbstractGuideItemHandle::result() const
{
    return item()->result;
}

void AbstractGuideItemHandle::setResult(Guide result)
{
    item()->result = result;
    item()->updatePath();
}

Guide AbstractGuideItemHandle::source() const
{
    return item()->source;
}

qreal AbstractGuideItemHandle::smooth() const
{
    return item()->smooth;
}

qreal AbstractGuideItemHandle::offset() const
{
    return item()->offset;
}

void AbstractGuideItemHandle::setSource(Guide source)
{
    if (source.isNull())
        return;
    item()->source = source;
    item()->updatePath();
}

void AbstractGuideItemHandle::setSmooth(qreal smooth)
{
    if (item()->smooth == smooth)
        return;

    item()->smooth = smooth;
    item()->updatePath();
}

void AbstractGuideItemHandle::setOffset(qreal offset)
{
    if (item()->offset == offset)
        return;
    item()->offset = offset;
    item()->updatePath();
}

qreal AbstractGuideItemHandle::precision() const
{
    return item()->precision;
}

void AbstractGuideItemHandle::setPrecision(qreal precision)
{
    if (item()->precision == precision)
        return;
    item()->precision = precision;
    item()->updatePath();
}

qreal AbstractGuideItemHandle::sourceStart() const
{
    return item()->sourceStart;
}

void AbstractGuideItemHandle::setSourceStart(qreal sourceStart)
{
    if (item()->sourceStart == sourceStart)
        return;
    item()->sourceStart = sourceStart;
    updatePath();
}

qreal AbstractGuideItemHandle::sourceEnd() const
{
    return item()->sourceEnd;
}

void AbstractGuideItemHandle::setSourceEnd(qreal sourceEnd)
{
    if (item()->sourceEnd == sourceEnd)
        return;
    item()->sourceEnd = sourceEnd;
    updatePath();
}

void AbstractGuideItemHandle::updatePath()
{
    item()->updatePath();
}

bool AbstractGuideItemHandle::fast() const
{
    return item()->fast;
}

void AbstractGuideItemHandle::setFast(bool fast)
{
    if (item()->fast == fast)
        return;

    item()->fast = fast;
    item()->updatePath();

}

qreal AbstractGuideItemHandle::scalingFactor() const
{
    return item()->scalingFactor;
}

void AbstractGuideItemHandle::setScalingFactor(qreal scalingFactor)
{
    if (item()->scalingFactor == scalingFactor)
        return;

    item()->scalingFactor = scalingFactor;
    item()->updatePath();
}


QPainterPath AbstractGuideItem::shape() const
{
    //    return outline;
    SceneData *d = static_cast<SceneData*>(scene()->userData(0));
    QPainterPath src = source.outline(15 / d->viewTransform.m11(), Qt::RoundCap);
    QPainterPath res = result.outline(15 / d->viewTransform.m11(), Qt::RoundCap);
    return src + res;
}

void AbstractGuideItem::updatePath(bool doUpdate)
{
//    qDebug() << "updating path" << source.length();
    Guide *p = nullptr;
    Guide part;
    if (sourceEnd < 0 && sourceStart == 0)
        p = &source;
    else {
        qreal end = sourceEnd;
        if(sourceEnd < 0)
            end = source.length();
        part = source.part(sourceStart, end);
        p = &part;
    }
    if (p->isEmpty()) {
        if (!result.isEmpty()) {
            result.clear();
            prepareGeometryChange();
        }
        return;
    }
    if (offset == 0 && smooth == 0)
        result = *p;
    else {
        Guide pth;
        if (offset != 0 && scalingFactor > 0.1) {
            Wavefront f(*p);
//            result.clear();
            f.setOffset(offset / scalingFactor);
            f.update(!fast);
            pth = f.toGuide();
        } else {
            pth = *p;
        }
        //            if (s < 0.1)
        if (smooth > 0) {
            result = pth.smoothed(smooth / scalingFactor, precision);
        }
        else {
            result = pth;
        }

    }
    if (!doUpdate)
        return;
    prepareGeometryChange();
}



