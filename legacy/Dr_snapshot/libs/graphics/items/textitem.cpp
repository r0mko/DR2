#include "textitem.h"


void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    preparePainter(painter);
    painter->setFont(font);
    QPointF pos;
    switch (horizontalAlignment) {
    case Qt::AlignHCenter:
        pos.setX(-stext.size().width() / 2);
        break;
    case Qt::AlignRight:
        pos.setX(-stext.size().width());
        break;
    case Qt::AlignLeft:
        pos.setX(0);
        break;
    default:
        break;
    }
    switch (verticalAlignment) {
    case Qt::AlignVCenter:
        pos.setY(-stext.size().height() / 2);
        break;
    case Qt::AlignTop:
        pos.setY(-stext.size().height());
        break;
    case Qt::AlignBottom:
        pos.setY(0);
    default:
        break;
    }
    painter->drawStaticText(pos, stext);
}

QRectF TextItem::boundingRect() const
{
    return QRectF(QPointF(), stext.size());
}

void TextItem::updateText()
{
    stext.prepare(currentViewTransform() * transform(), font);
    update();
}


QString TextItemHandle::text() const
{
    return item()->stext.text();
}


QFont TextItemHandle::font() const
{
    return item()->font;
}


QColor TextItemHandle::color() const
{
    return item()->color;
}


void TextItemHandle::setText(QString text)
{
    if (item()->stext.text() == text)
        return;
    item()->stext.setText(text);
    item()->updateText();
}


void TextItemHandle::setFont(QFont font)
{
    if (item()->font == font)
        return;
    item()->font = font;
    item()->updateText();
}


void TextItemHandle::setColor(QColor color)
{
    if (item()->color == color)
        return;

    item()->color = color;
    item()->update();
}
