#include "guidepathtext.h"

#include <QPainter>



QString GuidePathTextHandle::text() const
{
    return item()->text;
}


QFont GuidePathTextHandle::font() const
{
    return item()->font;
}


QColor GuidePathTextHandle::textColor() const
{
    return item()->textColor;
}

QColor GuidePathTextHandle::outlineColor() const
{
    return item()->outlineColor;

}


Qt::Orientation GuidePathTextHandle::orientation() const
{
    return item()->orientation;
}

qreal GuidePathTextHandle::position() const
{
    return item()->position;
}


void GuidePathTextHandle::setText(QString text)
{
    if (item()->text == text)
        return;

    item()->text = text;
    item()->updateText();
    //    item()->update();
}


void GuidePathTextHandle::setFont(QFont font)
{
    if (item()->font == font)
        return;

    item()->m_rawFont = QRawFont::fromFont(font);
    item()->update();
}


void GuidePathTextHandle::setTextColor(QColor textColor)
{
    if (item()->textColor == textColor)
        return;

    item()->textColor = textColor;
    item()->update();
}

void GuidePathTextHandle::setOutlineColor(QColor outlineColor)
{
    if (item()->outlineColor == outlineColor)
        return;

    item()->outlineColor = outlineColor;
    item()->update();

}


void GuidePathTextHandle::setOrientation(Qt::Orientation orientation)
{
    if (item()->orientation == orientation)
        return;

    item()->orientation = orientation;
    item()->update();
}

void GuidePathTextHandle::setPosition(qreal position)
{
    if (item()->position == position)
        return;

    item()->position = position;
    item()->update();
}

int GuidePathText::type() const
{
    return SELF_PTR_TYPEID;
}

void GuidePathText::updateText()
{
    glyphIndexList = m_rawFont.glyphIndexesForString( text );
    for( quint32 id : glyphIndexList )
        m_glyphPathCache.insert( id, m_rawFont.pathForGlyph( id ) );
    advances = m_rawFont.advancesForGlyphIndexes( glyphIndexList, QRawFont::KernedAdvances );
    for( QPointF p : advances ) labelWidth += p.x();
    //    labelWidth += m_rawFont.boundingRect( glyphIndexList.last() ).width();
    update();
}

void GuidePathText::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen;
    pen.setColor(textColor);
    painter->setFont(font);
    QTransform tr = currentViewTransform();
    PathRunner r(&path, position);
    if (orientation == Qt::Horizontal) {
        painter->save();
        //        qDebug() << "updating horizontal text" << text << font.pointSize() << r.point() << currentViewTransform().m11() << orientation << painter->device()->width();
        painter->translate(r.point());
        if (r.normal().x() < 0) {
            painter->rotate(r.angle() - 90);
        } else {
            painter->rotate(r.angle() + 90);
        }
        painter->scale(1 / tr.m11(), 1 / tr.m22());
        painter->drawText(0, (m_rawFont.ascent() - m_rawFont.descent()) / 2, text);
        painter->restore();
    } else {
        int i = 0;
        for( quint32 id : glyphIndexList ) {
            painter->save();
            painter->translate(r.point());
            painter->rotate(r.angle());
            painter->scale(1 / tr.m11(), 1 / tr.m22());
            if (outlineColor != Qt::transparent) {
                QPen outlinePen;
                outlinePen.setWidth(1);
                outlinePen.setCosmetic(true);
                outlinePen.setColor(outlineColor);
                painter->strokePath(m_glyphPathCache[ id ], outlinePen);
            }
            painter->fillPath(m_glyphPathCache[ id ], QBrush(textColor));

            //            painter->drawPath(m_glyphPathCache[ id ]);
            painter->restore();
            r.moveForward(advances.at( i++ ).x() / tr.m11());
            if (r.atEnd())
                break;
        }


    }

}


QRectF GuidePathText::boundingRect() const
{
    return path.boundingRect();
}
