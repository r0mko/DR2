#include "textlayer.h"
#include "scenemodel.h"

#include <QDateTime>
#include <QPainter>
#include <guide.h>
#include <runner.h>
#include <QtGlobal>
#include <QTimer>
#include <QVariant>
//#define DEBUGPAINTTEXTLAYER_BSP
//#define DEBUGPAINTTEXTLAYER_MESH
//#define DEBUGPAINTTEXTLAYER_FPS
//#define DEBUGPAINTTEXTLAYER_REVERSE
//#define DEBUGPAINTTEXTLAYER_VIEWPORTRECT

void TextLayer::paint(QPainter *painter)
{
    if (m_delayPaint.isActive())
        return;
    #ifdef DEBUGPAINTTEXTLAYER_FPS
    qint64 st = QDateTime::currentMSecsSinceEpoch();
    #endif

    painter->save();
    painter->setTransform(m_viewTransform);

    qreal invScaleX{ 1.0 / m_viewTransform.m11() };
    qreal invScaleY{ 1.0 / m_viewTransform.m22() };
    painter->setRenderHints( QPainter::Antialiasing | QPainter::HighQualityAntialiasing );

    for( auto it = --m_labels.end(); it != --m_labels.begin(); --it ){
        m_font.setPixelSize(it.key());
        painter->setFont( m_font );
        for ( GuideLabel &l :it.value().labels )
            paintLabel(painter, l, invScaleX, invScaleY, it.value() );
    }

    #ifdef DEBUGPAINTTEXTLAYER_BSP
    for( auto r: m_bsp.dumpRects() )
        painter->fillRect(r, QColor(0x88, 0x88, 0xff, 0x88));
    #endif


#ifdef DEBUGPAINTTEXTLAYER_VIEWPORTRECT
    painter->fillRect(m_viewportRect, QColor(0x88, 0x88, 0xff, 0x44));
#endif


    painter->restore();


    #ifdef DEBUGPAINTTEXTLAYER_FPS
    qDebug() << "paintLabel" << QDateTime::currentMSecsSinceEpoch() - st << "ms";
    #endif
}

void TextLayer::paintLabel(QPainter *painter,  const GuideLabel &s, qreal invScaleX, qreal invScaleY, const LablesData &labelData)
{
    if( !s.path.boundingRect().intersects(m_viewportRect) ) return;
    if( s.path.length() < s.labelWidth * invScaleX ) return; // skip short guides
    Runner peekRunner(s.path, 0);
    Runner waitRunner(s.path, 0);
    while (!peekRunner.atEnd()) {
        LabelPosition position = findSuitableLabelPosition(peekRunner, s, invScaleX, invScaleY, labelData);
        if( !position.success ) return;
        waitRunner.setPosition(position.pos);

        int i = 0;
        int corrector = 1;
        if( position.reversed ) {
            waitRunner.moveForward( s.labelWidth * invScaleX );
            corrector = -1;
        }
        for( quint32 id : s.glyphIndexList ) {
            painter->save();
            qreal verticaloffset = labelData.ascent * 0.3 * corrector;
            QPointF pos( waitRunner.point() + waitRunner.normal() * verticaloffset * invScaleX ) ;
            painter->translate(pos);
            painter->rotate(waitRunner.angle());
            painter->scale(invScaleX * corrector, invScaleY * corrector);


            if( m_outline ) {
                QPen p;
                p.setColor(m_outlineColor);
                p.setWidthF( 3 );
                p.setStyle(Qt::SolidLine);
                painter->strokePath(labelData.glyphPathCache[ id ], p);
            }


            #ifdef DEBUGPAINTTEXTLAYER_REVERSE
            painter->fillPath(labelData.glyphPathCache[ id ], QBrush(Qt::blue));
            #else
            painter->fillPath(labelData.glyphPathCache[ id ], QBrush(m_color));
            #endif



            painter->restore();

            if( position.reversed ) waitRunner.moveBack( s.advances.at( i++ ).x() * invScaleX );
            else waitRunner.moveForward( s.advances.at( i++ ).x() * invScaleX );

            if (waitRunner.atEnd()) break;
        }

        #ifdef DEBUGPAINTTEXTLAYER_MESH
        QPen p;
        p.setColor(Qt::green);
        p.setWidth(4);
        p.setCosmetic(true);
        p.setStyle(Qt::SolidLine);
        painter->setPen(p);
        painter->drawPolyline(position.mesh);
        #endif

        peekRunner.moveForward( m_spacing * invScaleX ); // make some room
    }

}

TextLayer::LabelPosition TextLayer::findSuitableLabelPosition(Runner &peekRunner, const GuideLabel &s, qreal invScaleX, qreal invScaleY, const LablesData &labelData)
{
    LabelPosition result;
    result.pos = peekRunner.position();
    result.success = false;

    while( !peekRunner.atEnd() ) {
        QPointF cp{ peekRunner.point() };
        QPointF np{ cp + peekRunner.vectorLengthAhead() * peekRunner.unitVector() }; // next node point
        QLine seg(m_bsp.mapPoint(cp), m_bsp.mapPoint(np));
        QPoint ul = m_bsp.mapPoint(m_viewportRect.topLeft());
        QPoint br = m_bsp.mapPoint(m_viewportRect.bottomRight());
        QRect mappedVp(ul, br);

        auto intersection = lineRectIntersection(mappedVp, seg);
        if( intersection != None && intersection != Contains ) {
            // adjust pos
        } else {
            peekRunner.hopForward();
            result.pos = peekRunner.position();
            result.success = false;
            result.mesh.clear();
        }
        int i = 0;
        QPointF r;
        for( quint32 id : s.glyphIndexList ) {
            Q_UNUSED(id)

            qreal verticaloffset = labelData.ascent * 0.3;
            qreal highestPart = labelData.ascent * 0.7;
            qreal lowestPart = labelData.descent;


            QPointF normal{ peekRunner.normal() };
            r += normal;
            QPointF ap{ peekRunner.point() - normal * highestPart * invScaleX + normal * verticaloffset * invScaleX };
            QPointF dp{ peekRunner.point() + normal * lowestPart * invScaleY + normal * verticaloffset * invScaleX };
            if( !checkPoint( ap) || !checkPoint( dp ) ) {
                peekRunner.moveForward( labelData.maxCharWidth * invScaleX );
                result.pos = peekRunner.position();
                result.mesh.clear();
                result.success = false;
                break;
            }
            result.mesh.append( ap );
            result.mesh.append( dp );
            result.success = true;
            peekRunner.moveForward( s.advances.at( i++ ).x() * invScaleX );
            if (peekRunner.atEnd()) {
                result.mesh.clear();
                result.success = false;
                break;
            }
        }
        if( result.success ) {
            if( r.y() < 0 ) result.reversed = true;
            for( const QPointF &p: result.mesh )
                if( m_viewportRect.contains(p) )
                    m_bsp.insertPoint(p, s);
            return result;
        }
    }
    return result;
}


TextLayer::TextLayer(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    setFlags( QQuickItem::ItemHasContents );
    m_delayPaint.setSingleShot(true);
    m_delayPaint.setInterval(60);
    connect(&m_delayPaint, &QTimer::timeout, this, &TextLayer::repaint);
}


QFont TextLayer::font() const
{
    return m_font;
}

QColor TextLayer::color() const
{
    return m_color;
}

void TextLayer::setFont(QFont font)
{
    if (m_font == font)
        return;

    m_font = font;
    emit fontChanged(font);
    fontUpdate();
}

void TextLayer::setColor(QColor color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
}


void TextLayer::setViewTransform(QTransform viewTransform)
{
    if (m_viewTransform == viewTransform)
        return;

    m_viewTransform = viewTransform;
    emit viewTransformChanged(viewTransform);
    if (!m_delayPaint.isActive()) {
        bspReset();
        update();
        emit vanished();
    }
    m_delayPaint.start();
}

void TextLayer::setViewportRect(QRectF viewportRect)
{
    if (m_viewportRect == viewportRect)
        return;

    m_viewportRect = viewportRect;
    emit viewportRectChanged(viewportRect);
}

void TextLayer::appendLabel(const QPolygonF &path, const QString &text, int priority)
{
    appendLabel(Guide(path), text, priority);
}

void TextLayer::appendLabel(const Guide &path, const QString &text, int priority)
{
    if ( path.isEmpty() ) return;
    GuideLabel a;
    a.path = path;
    a.text = text;
    a.priority = priority;

    if( !m_labels.contains(priority) ) {
        LablesData f;
        m_font.setPixelSize( priority );
        f.rawFont = QRawFont::fromFont(m_font);
        f.ascent = f.rawFont.ascent();
        f.descent = f.rawFont.descent();
        f.maxCharWidth = f.rawFont.maxCharWidth();
        m_labels[priority] = f;
    }


    a.glyphIndexList = m_labels[priority].rawFont.glyphIndexesForString( text );
    for( quint32 id : a.glyphIndexList )
        m_labels[priority].glyphPathCache.insert( id, m_labels[priority].rawFont.pathForGlyph( id ) );
    a.advances = m_labels[priority].rawFont.advancesForGlyphIndexes( a.glyphIndexList, QRawFont::KernedAdvances );
    a.labelWidth = 0;
    for( QPointF p : a.advances )
        a.labelWidth += p.x();

    m_labels[priority].labels.append(a);


}

void TextLayer::clear()
{
    m_labels.clear();
}

void TextLayer::repaint()
{
    emit emerged();
    update();
}


void TextLayer::setDepth(int depth)
{
    if (m_bsp.maxDepth() == depth)
        return;

    m_bsp.setMaxDepth(depth);
    emit depthChanged(depth);
    bspReset();
}

void TextLayer::setSpacing(qreal spacing)
{
    if (m_spacing == spacing)
        return;

    m_spacing = spacing;
    emit spacingChanged(spacing);
}

void TextLayer::setOutline(bool outline)
{
    if (m_outline == outline)
        return;

    m_outline = outline;
    emit outlineChanged(outline);
}

void TextLayer::setOutlineColor(QColor outlineColor)
{
    if (m_outlineColor == outlineColor)
        return;

    m_outlineColor = outlineColor;
    emit outlineColorChanged(outlineColor);
}

QTransform TextLayer::viewTransform() const
{
    return m_viewTransform;
}

QRectF TextLayer::viewportRect() const
{
    return m_viewportRect;
}

qreal TextLayer::spacing() const
{
    return m_spacing;
}

int TextLayer::count() const
{
    int ret{0};
    for( auto it = m_labels.begin(); it != m_labels.end(); ++it )
        ret += it.value().labels.size();
    return ret;
}

bool TextLayer::outline() const
{
    return m_outline;
}

QColor TextLayer::outlineColor() const
{
    return m_outlineColor;
}

void TextLayer::bspReset()
{
    m_bsp.clear();
    m_bsp.setRect(m_viewportRect);
}

void TextLayer::fontUpdate()
{
    m_labels.clear();
    for( auto it = m_labels.begin(); it != m_labels.end(); ++it ){
        LablesData f;
        m_font.setPixelSize( it.key() );
        f.rawFont = QRawFont::fromFont(m_font);
        f.ascent = f.rawFont.ascent();
        f.descent = f.rawFont.descent();
        f.maxCharWidth = f.rawFont.maxCharWidth();
        for ( GuideLabel &l : it.value().labels ) {
            l.glyphIndexList = f.rawFont.glyphIndexesForString( l.text );
            for( quint32 id : l.glyphIndexList )
                f.glyphPathCache.insert( id, f.rawFont.pathForGlyph( id ) );
            l.advances = f.rawFont.advancesForGlyphIndexes( l.glyphIndexList, QRawFont::KernedAdvances );
            l.labelWidth = 0;
            for( QPointF p : l.advances ) l.labelWidth += p.x();
        }
        m_labels[it.key()] = f;
    }
    update();
}

int TextLayer::depth() const
{
    return m_bsp.maxDepth();
}
