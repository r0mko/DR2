#include "guidepathstroke.h"

#include <QGraphicsScene>
#include <QPainter>


GPen GuidePathStrokeHandle::pen() const
{
    return item()->pen;
}


void GuidePathStrokeHandle::setPen(GPen pen)
{
    if (item()->pen == pen)
        return;

    item()->pen = pen;
    item()->update();
}

GPen GuidePathStrokeHandle::sourcePen() const
{
    return item()->sourcePen;
}

void GuidePathStrokeHandle::setSourcePen(GPen sourcePen)
{
    if (item()->sourcePen == sourcePen)
        return;
    item()->sourcePen = sourcePen;
    update();
}

bool GuidePathStrokeHandle::drawResult() const
{
    return item()->drawResult;
}

void GuidePathStrokeHandle::setDrawResult(bool drawResult)
{
    if (item()->drawResult == drawResult)
        return;
    item()->drawResult = drawResult;
    update();
}

bool GuidePathStrokeHandle::drawSource() const
{
    return item()->drawSource;
}

void GuidePathStrokeHandle::setDrawSource(bool drawSource)
{
    if (item()->drawSource == drawSource)
        return;
    item()-> drawSource = drawSource;
    update();
}





qreal GuidePathStrokeHandle::sourceStrokeStart() const
{
    return item()->sourceStrokeStart;
}

void GuidePathStrokeHandle::setSourceStrokeStart(qreal sourceStrokeStart)
{
    if (item()->sourceStrokeStart == sourceStrokeStart)
        return;
    item()->sourceStrokeStart = sourceStrokeStart;
    update();
}

qreal GuidePathStrokeHandle::sourceStrokeEnd() const
{
    return item()->sourceStrokeEnd;
}

void GuidePathStrokeHandle::setSourceStrokeEnd(qreal sourceStrokeEnd)
{
    if (item()->sourceStrokeEnd == sourceStrokeEnd)
        return;
    item()->sourceStrokeEnd = sourceStrokeEnd;
    update();
}

qreal GuidePathStrokeHandle::strokeStart() const
{
    return item()->strokeStart;
}

void GuidePathStrokeHandle::setStrokeStart(qreal strokeStart)
{
    if (item()->strokeStart == strokeStart)
        return;
    item()->strokeStart = strokeStart;
    update();
}

qreal GuidePathStrokeHandle::strokeEnd() const
{
    return item()->strokeEnd;
}

void GuidePathStrokeHandle::setStrokeEnd(qreal strokeEnd)
{
    if (item()->strokeEnd == strokeEnd)
        return;
    item()->strokeEnd = strokeEnd;
    update();
}

void GuidePathStrokeHandle::setDebugMode(bool debug)
{
    item()->debugNodes = debug;
    update();
}

void GuidePathStroke::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (drawSource && sourceStrokeStart != sourceStrokeEnd)
        paintGuide(painter, source, sourceStrokeStart, sourceStrokeEnd, sourcePen);
    updatePath(false);
    if (drawResult && strokeStart != strokeEnd)
        paintGuide(painter, result, strokeStart, strokeEnd, pen);

}

int GuidePathStroke::type() const
{
    return SELF_PTR_TYPEID;
}

void GuidePathStroke::paintGuide(QPainter *painter, const GuidePath &path, qreal start, qreal end, const QPen &pen) const
{
    QPainterPath p;
    if (start == 0 && end < 0) {
        p = path.toPath();
        painter->strokePath(p, pen);
    } else {
        qreal pend = end;
        if (end < 0)
            pend = path.length();
        p = path.part(start, pend).toPath();
        painter->strokePath(p, pen);
    }
    if (!debugNodes)
        return;
    //    qDebug() << painter->worldMatrix() << painter->viewport() << painter->matrix();
    qreal s = painter->worldMatrix().m11();
    //    qreal s = currentViewTransform().m11();
    if (s < 2)
        return;
    for (QPolygonF polygon : p.toSubpathPolygons()) {
        QPen dotPen;
        dotPen.setWidthF(pen.width() * 3.5);
        dotPen.setCosmetic(true);
        dotPen.setCapStyle(Qt::RoundCap);
        dotPen.setColor(pen.color());
        painter->setPen(dotPen);
        painter->drawPoints(polygon);
        //    painter->drawLines(lines);
        QPen centerPen;
        centerPen.setCosmetic(true);
        centerPen.setColor(QColor(0,0,0));
        centerPen.setCapStyle(Qt::RoundCap);
        centerPen.setWidthF(pen.width() * 2);
        //    outlinePen.setWidthF(pen()->width() * 1.5);
        painter->setPen(centerPen);
        painter->drawPoints(polygon);
        QFont f;
        f.setPointSizeF(10);
        painter->setFont(f);
        int i = 0;

        painter->setPen(Qt::white);
        painter->save();
        painter->scale(1/s, 1/s);
        painter->translate(-4, 0);
        if (s > 8)
            for (QPointF pt:polygon) {
                painter->drawText(pt * s, QString::number(i++));
            }
        painter->restore();
    }
}
