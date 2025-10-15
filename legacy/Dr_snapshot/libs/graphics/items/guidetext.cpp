#include "guidetext.h"

#include <QPainter>



QString GuideTextHandle::text() const
{
    return item()->text;
}


QFont GuideTextHandle::font() const
{
    return item()->font;
}


QColor GuideTextHandle::textColor() const
{
    return item()->textColor;
}

QColor GuideTextHandle::outlineColor() const
{
    return item()->outlineColor;

}


Qt::Orientation GuideTextHandle::orientation() const
{
    return item()->orientation;
}

qreal GuideTextHandle::position() const
{
    return item()->position;
}

Guide GuideTextHandle::path() const
{
    return item()->path;
}


void GuideTextHandle::setText(QString text)
{
    if (item()->text == text)
        return;

    item()->text = text;
    item()->updateText();
    //    item()->update();
}


void GuideTextHandle::setFont(QFont font)
{
    if (item()->font == font)
        return;

    item()->m_rawFont = QRawFont::fromFont(font);
    item()->ascent = item()->m_rawFont.ascent();
    item()->descent = item()->m_rawFont.descent();
    item()->update();
}


void GuideTextHandle::setTextColor(QColor textColor)
{
    if (item()->textColor == textColor)
        return;

    item()->textColor = textColor;
    item()->update();
}

void GuideTextHandle::setOutlineColor(QColor outlineColor)
{
    if (item()->outlineColor == outlineColor)
        return;

    item()->outlineColor = outlineColor;
    item()->update();

}


void GuideTextHandle::setOrientation(Qt::Orientation orientation)
{
    if (item()->orientation == orientation)
        return;

    item()->orientation = orientation;
    item()->update();
}

void GuideTextHandle::setPosition(qreal position)
{
    if (item()->position == position)
        return;

    item()->position = position;
    item()->update();
}

//int GuideText::type() const
//{
//    return SELF_PTR_TYPEID;
//}

void GuideText::updateText()
{
    glyphIndexList = m_rawFont.glyphIndexesForString( text );
    for( quint32 id : glyphIndexList )
        m_glyphPathCache.insert( id, m_rawFont.pathForGlyph( id ) );
    advances = m_rawFont.advancesForGlyphIndexes( glyphIndexList, QRawFont::KernedAdvances );
    labelWidth = 0;
    for( QPointF p : advances ) labelWidth += p.x();
    //    labelWidth += m_rawFont.boundingRect( glyphIndexList.last() ).width();
    updateMesh();
    update();
}

void GuideText::updateMesh()
{
    qDebug() << "updated text mesh";
    mesh.clear();
    QTransform tr = currentViewTransform();
    Runner r(path, position);
    if (orientation == Qt::Horizontal) {
        qDebug() << "horizontal text mesh update. MAKE BODY";
//        painter->save();
//        painter->translate(r.point());
//        mesh.append(r.point());

//        if (r.normal().x() < 0) {
//            painter->rotate(r.angle() - 90);
//        } else {
//            painter->rotate(r.angle() + 90);
//        }

//        painter->scale(1 / tr.m11(), 1 / tr.m22());
//        painter->drawText(0, (m_rawFont.ascent() - m_rawFont.descent()) / 2, text);
//        painter->restore();
    } else {
        int i = 0;
        for( quint32 id : glyphIndexList ) {
//            painter->save();
//            painter->translate(r.point());
            QPointF cp = r.point();
            mesh.append( cp - r.normal() * ascent / tr.m11() );
            mesh.append( cp - r.normal() * descent / tr.m11() );
//            painter->rotate(r.angle());
//            painter->scale(1 / tr.m11(), 1 / tr.m22());
//            if (outlineColor != Qt::transparent) {
//                QPen outlinePen;
//                outlinePen.setWidth(1);
//                outlinePen.setCosmetic(true);
//                outlinePen.setColor(outlineColor);
//                painter->strokePath(m_glyphPathCache[ id ], outlinePen);
//            }
//            painter->fillPath(m_glyphPathCache[ id ], QBrush(textColor));

            //            painter->drawPath(m_glyphPathCache[ id ]);
//            painter->restore();
            r.moveForward(advances.at( i++ ).x() / tr.m11());
            if (r.atEnd())
                break;
        }
    }
}

void GuideText::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    updateMesh();
//#define ZIGZAGTESTTEXT
#ifndef ZIGZAGTESTTEXT
    QPen pen;
    pen.setColor(textColor);
    painter->setFont(font);
    QTransform tr = currentViewTransform();
    Runner r(path, position);
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
        painter->drawText(0, (ascent - descent) / 2, text);
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
#else
    painter->drawPolygon(mesh);
#endif
}


QRectF GuideText::boundingRect() const
{
    return path.boundingRect();
}
