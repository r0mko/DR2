#include "abstractguideitem.h"
#include <QGraphicsScene>

GuidePath AbstractGuideItemHandle::result() const
{
    return item()->result;
}

void AbstractGuideItemHandle::setResult(GuidePath result)
{
    item()->result = result;
    item()->updatePath();
}

GuidePath AbstractGuideItemHandle::source() const
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

void AbstractGuideItemHandle::setSource(GuidePath source)
{
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

QRectF AbstractGuideItem::boundingRect() const
{
    return result.boundingRect();
}

QPainterPath AbstractGuideItem::shape() const
{
    //    return outline;
    SceneData *d = static_cast<SceneData*>(scene()->userData(0));
    return result.outline(10 / d->viewTransform.m11(), Qt::RoundCap);
}


void AbstractGuideItem::updatePath()
{
    GuidePath *p = nullptr;
    GuidePath part;
    if (sourceEnd < 0 && sourceStart == 0)
        p = &source;
    else {
        qreal end = sourceEnd;
        if(sourceEnd < 0)
            end = source.length();
        part = source.part(sourceStart, end);
        p = &part;
        qDebug() << "Updating part path, path size" << p->count() << "len" << p->length() << "start" << sourceStart << "end" << sourceEnd;
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
        if (smooth > 0) {
            result = p->smoothed(smooth, precision);
        } else {
            result = *p;
        }
        if (offset > 0)
            result = result.offseted(offset);
    }
    prepareGeometryChange();
    update();
}



