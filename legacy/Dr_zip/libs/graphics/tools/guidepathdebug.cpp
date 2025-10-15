#include "guidepathdebug.h"

#include <QPainter>

GuideDebug::GuideDebug()
{

}

void GuideDebug::paintNodes(Guide p, QPainter *painter, QPen pen, QColor textColor)
{
    qreal s = painter->worldMatrix().m11();
    //    qreal s = currentViewTransform().m11();

    QPen dotPen;

    if (s < 2) {
        dotPen = pen;
        dotPen.setWidthF(pen.widthF() * 2);
        dotPen.setCosmetic(pen.isCosmetic());
    } else {
        dotPen.setWidthF(pen.widthF() * 3.5);
        dotPen.setCosmetic(true);

    }
    dotPen.setCapStyle(Qt::RoundCap);
    dotPen.setColor(pen.color());

    Runner r(p, 0);
    painter->setPen(dotPen);
    while (!r.atEnd()) {
        painter->drawPoint(r.point());
        r.hopForward();
    }
    painter->drawPoint(r.point());
    if (s < 2)
        return;
    r = Runner(p, 0);
    QPen centerPen;
    centerPen.setCosmetic(true);
    centerPen.setColor(QColor(0,0,0));
    centerPen.setCapStyle(Qt::RoundCap);
    centerPen.setWidthF(pen.width() * 2);
    while (!r.atEnd()) {
        painter->drawPoint(r.point());
        r.hopForward();
    }
    painter->drawPoint(r.point());
    r = Runner(p, 0);
    QFont f;
    f.setPointSizeF(11);
    painter->setFont(f);
    painter->setPen(textColor);
    int i = 0;
    painter->save();
    painter->scale(1/s, 1/s);
    painter->translate(-4, 0);
    while (!r.atEnd()) {
        painter->drawText(r.point() * s, QString::number(i++));
        r.hopForward();
    }
    painter->drawText(r.point() * s, QString::number(i++));
    painter->restore();;
}

