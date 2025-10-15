#include "textlayer.h"
#include "scenemodel.h"

#include <QPainter>
#include <guidepath.h>
#include <pathrunner.h>
void TextLayer::readModel()
{
    if (!checkModel())
        return;
    for (int i = 0; i < m_model->rowCount(); ++i) {
        QModelIndex idx = m_model->index(i, 0);
        addItem(idx);
    }
}

void TextLayer::addItem(const QModelIndex &idx)
{
    QPolygonF polygon = idx.data(i_polygonRole).value<QPolygonF>();
    QString text = idx.data(i_textRole).toString();
    GuideLabel lbl;
    lbl.glyphIndexList = m_rawFont.glyphIndexesForString( text );
    lbl.path = GuidePath(polygon);
    for( quint32 id : lbl.glyphIndexList )
        m_glyphPathCache.insert( id, m_rawFont.pathForGlyph( id ) );
    lbl.advances = m_rawFont.advancesForGlyphIndexes( lbl.glyphIndexList, QRawFont::KernedAdvances );
    for( QPointF p : lbl.advances ) lbl.labelWidth += p.x();
    labels.append(lbl);
}

bool TextLayer::checkModel()
{
    if (!m_model)
        return false;
    i_polygonRole = findRole(m_polygonRole, m_model);
    if (i_polygonRole < 0)
        return false;
    i_textRole = findRole(m_textRole, m_model);
    if (i_textRole < 0)
        return false;
    return true;

}

void TextLayer::paintLabel(QPainter *painter, const QTransform &tr, const QRectF &viewportRect, TextLayer::GuideLabel &s)
{
    qreal scale = (tr.m11() + tr.m22()) / 2;

    if( scale < m_minimumScale ) return;

    if (scale < m_minimumScale)
        return;
    //    s.curl.setPolygons( s.path.toSubpathPolygons( tr ) );
    //    qDebug() << curl.currentAngle();
    painter->setFont(m_font);
    PathRunner r(&s.path, 0);
    //    if (m_oriantation == Qt::Horizontal) {
    //        painter->save();
    //        //        qDebug() << "updating horizontal text" << text << font.pointSize() << r.point() << currentViewTransform().m11() << m_oriantation << painter->device()->width();
    //        painter->translate(r.point());
    //        if (r.normal().x() < 0) {
    //            painter->rotate(r.angle() - 90);
    //        } else {
    //            painter->rotate(r.angle() + 90);
    //        }
    //        painter->scale(1 / m_viewTransform.m11(), 1 / m_viewTransform.m22());
    //        painter->drawText(0, (m_rawFont.ascent() - m_rawFont.descent()) / 2, s.text);
    //        painter->restore();
    //    } else {
    int i = 0;
    for( quint32 id : s.glyphIndexList ) {
        painter->save();
        painter->translate(r.point());
        painter->rotate(r.angle());
        painter->scale(1 / m_viewTransform.m11(), 1 / m_viewTransform.m22());
        painter->fillPath(m_glyphPathCache[ id ], QBrush(m_color));

        //            painter->drawPath(m_glyphPathCache[ id ]);
        painter->restore();
        r.moveForward(s.advances.at( i++ ).x() / m_viewTransform.m11());
        if (r.atEnd())
            break;
    }

}

TextLayer::TextLayer(QQuickItem *parent) : QQuickPaintedItem(parent)
{

}

QString TextLayer::polygonRole() const
{
    return m_polygonRole;
}

QString TextLayer::textRole() const
{
    return m_textRole;
}

QFont TextLayer::font() const
{
    return m_font;
}

QColor TextLayer::color() const
{
    return m_color;
}

QAbstractItemModel *TextLayer::model() const
{
    return m_model;
}

void TextLayer::setPolygonRole(QString polygonRole)
{
    if (m_polygonRole == polygonRole)
        return;

    m_polygonRole = polygonRole;
    emit polygonRoleChanged(polygonRole);
}

void TextLayer::setTextRole(QString textRole)
{
    if (m_textRole == textRole)
        return;

    m_textRole = textRole;
    emit textRoleChanged(textRole);
}

void TextLayer::setFont(QFont font)
{
    if (m_font == font)
        return;

    m_font = font;
    emit fontChanged(font);
}

void TextLayer::setColor(QColor color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
}

void TextLayer::setModel(QAbstractItemModel *model)
{
    if (m_model == model)
        return;

    m_model = model;
    connect(m_model, &QAbstractItemModel::rowsInserted, this, &TextLayer::onAddItem);
    emit modelChanged(model);
}

void TextLayer::setMinimumScale(qreal minimumScale)
{
    if (m_minimumScale == minimumScale)
        return;

    m_minimumScale = minimumScale;
    emit minimumScaleChanged(minimumScale);
}

void TextLayer::setViewTransform(QTransform viewTransform)
{
    if (m_viewTransform == viewTransform)
        return;

    m_viewTransform = viewTransform;
    emit viewTransformChanged(viewTransform);
}

void TextLayer::setViewportRect(QRectF viewportRect)
{
    if (m_viewportRect == viewportRect)
        return;

    m_viewportRect = viewportRect;
    emit viewportRectChanged(viewportRect);
}

void TextLayer::onAddItem(const QModelIndex &parent, int first, int last)
{
    for (int i = first; i <= last; ++i) {
        QModelIndex idx = parent.child(i, 0);
        addItem(idx);
    }
    update();
}

void TextLayer::paint(QPainter *painter)
{
    painter->setPen( m_color );
    for (GuideLabel l : labels) {
        paintLabel(painter, m_viewTransform, m_viewportRect, l);

    }
}

qreal TextLayer::minimumScale() const
{
    return m_minimumScale;
}

QTransform TextLayer::viewTransform() const
{
    return m_viewTransform;
}

QRectF TextLayer::viewportRect() const
{
    return m_viewportRect;
}
