#include "guidestroke.h"

#include <QGraphicsScene>
#include <QPainter>
#include <guidepathdebug.h>


GPen GuideStrokeHandle::pen() const
{
    return item()->pen;
}


void GuideStrokeHandle::setPen(GPen pen)
{
    if (item()->pen == pen)
        return;

    item()->pen = pen;
    item()->update();
}

GPen GuideStrokeHandle::sourcePen() const
{
    return item()->sourcePen;
}

void GuideStrokeHandle::setSourcePen(GPen sourcePen)
{
    if (item()->sourcePen == sourcePen)
        return;
    item()->sourcePen = sourcePen;
    update();
}

bool GuideStrokeHandle::drawResult() const
{
    return item()->drawResult;
}

void GuideStrokeHandle::setDrawResult(bool drawResult)
{
    if (item()->drawResult == drawResult)
        return;
    item()->drawResult = drawResult;
    update();
}

bool GuideStrokeHandle::drawSource() const
{
    return item()->drawSource;
}

void GuideStrokeHandle::setDrawSource(bool drawSource)
{
    if (item()->drawSource == drawSource)
        return;
    item()-> drawSource = drawSource;
    update();
}





qreal GuideStrokeHandle::sourceStrokeStart() const
{
    return item()->sourceStrokeStart;
}

void GuideStrokeHandle::setSourceStrokeStart(qreal sourceStrokeStart)
{
    if (item()->sourceStrokeStart == sourceStrokeStart)
        return;
    item()->sourceStrokeStart = sourceStrokeStart;
    update();
}

qreal GuideStrokeHandle::sourceStrokeEnd() const
{
    return item()->sourceStrokeEnd;
}

void GuideStrokeHandle::setSourceStrokeEnd(qreal sourceStrokeEnd)
{
    if (item()->sourceStrokeEnd == sourceStrokeEnd)
        return;
    item()->sourceStrokeEnd = sourceStrokeEnd;
    update();
}

qreal GuideStrokeHandle::strokeStart() const
{
    return item()->strokeStart;
}

void GuideStrokeHandle::setStrokeStart(qreal strokeStart)
{
    if (item()->strokeStart == strokeStart)
        return;
    item()->strokeStart = strokeStart;
    update();
}

qreal GuideStrokeHandle::strokeEnd() const
{
    return item()->strokeEnd;
}

void GuideStrokeHandle::setStrokeEnd(qreal strokeEnd)
{
    if (item()->strokeEnd == strokeEnd)
        return;
    item()->strokeEnd = strokeEnd;
    update();
}

void GuideStrokeHandle::setDebugMode(bool debug)
{
    item()->debugNodes = debug;
    update();
}

void GuideStrokeHandle::setDebugTextColor(QColor color)
{
    item()->debugTextColor = color;
    update();
}





QRectF GuideStroke::boundingRect() const
{
    QRectF src_rect;
    qreal s_adj = sourcePen.width();

    QRectF res_rect;
    qreal r_adj = pen.width();

    if (drawSource)
        src_rect = source.boundingRect().adjusted(-s_adj, -s_adj, s_adj, s_adj);
    if (drawResult)
        res_rect = result.boundingRect().adjusted(-r_adj, -r_adj, r_adj, r_adj);
    return src_rect.united(res_rect);
}

void GuideStroke::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (drawSource && sourceStrokeStart != sourceStrokeEnd)
        paintGuide(painter, source, sourceStrokeStart, sourceStrokeEnd, sourcePen);

    if (drawResult && strokeStart != strokeEnd) {
        paintGuide(painter, result, strokeStart, strokeEnd, pen);
    }
}

//int GuideStroke::type() const
//{
//    return SELF_PTR_TYPEID;
//}

void GuideStroke::paintGuide(QPainter *painter, const Guide &path, qreal start, qreal end, const QPen &pen) const
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
    GuideDebug::paintNodes(path, painter, pen, debugTextColor);

    //    qDebug() << painter->worldMatrix() << painter->viewport() << painter->matrix();

}
