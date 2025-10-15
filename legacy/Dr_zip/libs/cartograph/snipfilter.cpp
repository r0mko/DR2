#include "snipfilter.h"
#include <QPainter>
#include <QDebug>
#include <QtMath>
#include <algorithm>
#include "streetlayer.h"
//#include "../Graphics/scene.h"

inline QPointF segPointAtPercent(const QPointF &p1, const QPointF &p2, qreal percent)
{
    return p1 + percent * (p2 - p1);
}

StreetLayer *SnipFilter::streetLayer() const
{
    return m_streetLayer;
}

void SnipFilter::setStreetLayer(StreetLayer *streetLayer)
{
}
m_streetLayer = streetLayer;
SnipFilter::SnipFilter(QObject *parent) :
    QObject(parent),
    m_streetLayer(nullptr)

{
}

void SnipFilter::paint(QPainter *painter, const QTransform& tr, const QRectF &viewportRect)
{
    for( StreetLabel& s : m_streetList )
        if( s.curl.boundingRect().intersects(viewportRect))
            paintLabel(painter, m_rawFont, tr, s);
}

QStringList SnipFilter::snipTypeList() const
{
    return m_snipTypeList;
}

void SnipFilter::addStreet(QPainterPath path, QString name)
{
    StreetLabel s;

    s.curl.setPath( path, true );

    s.glyphIndexList = m_rawFont.glyphIndexesForString( name );

    for( quint32 id : s.glyphIndexList ) m_glyphPathCache.insert( id, m_rawFont.pathForGlyph( id ) );

    s.advances = m_rawFont.advancesForGlyphIndexes( s.glyphIndexList, QRawFont::KernedAdvances );

    s.labelWidth = 0;
    for( QPointF p : s.advances ) s.labelWidth += p.x();
    s.labelWidth += m_rawFont.boundingRect( s.glyphIndexList.last() ).width();


    m_streetList.append( s );
}

void SnipFilter::setFont(QFont font)
{
    font.setPixelSize( fontSize() );
    m_rawFont = QRawFont::fromFont( font );
}

qreal SnipFilter::fontSize() const
{
    return m_fontSize;
}


void SnipFilter::setSnipTypeList(QStringList arg)
{
    if (m_snipTypeList == arg)
        return;

    m_snipTypeList = arg;
    emit snipTypeListChanged(arg);
}

void SnipFilter::setFontSize(qreal arg)
{
    if (m_fontSize == arg)
        return;

    m_fontSize = arg;
    emit fontSizeChanged(arg);
}

void SnipFilter::clearData()
{
    m_streetList.clear();
    m_glyphPathCache.clear();
}


const QHash<quint32, QPainterPath> &SnipFilter::glyphPathCache() const
{
    return m_glyphPathCache;
}

void SnipFilter::setGlyphPathCache(const QHash<quint32, QPainterPath> &glyphPathCache)
{
    m_glyphPathCache = glyphPathCache;
}

qreal SnipFilter::threshold() const
{
    return m_threshold;
}

void SnipFilter::setThreshold(qreal arg)
{
    if (m_threshold == arg)
        return;

    m_threshold = arg;
    emit thresholdChanged(arg);
}



void PathCurl::setPosition(qreal pos)
{
    cpoint = 0;
    m_currentPosition = 0;
    m_currentPoint = m_polygons.at(m_currentSubpath).first();
    m_atEnd = false;
    skipLength(pos);
}

void PathCurl::skipLength(qreal length)
{
    // TODO: check this condition
    if (cpoint + 1 >= m_polygons.at(m_currentSubpath).size()) {
        m_atEnd = true;
        return;
    }
    if (m_currentPosition == 0.0) {
        m_currentPoint = m_polygons.at(m_currentSubpath).first();
    }
    do {
        const QPointF &pt = m_polygons.at(m_currentSubpath).at(cpoint + 1);
        qreal segLen = sqrt(pow(pt.x() - m_currentPoint.x(), 2) + pow(pt.y() - m_currentPoint.y() , 2));
        if (length > segLen) {
            m_currentPoint = pt;
            //            m_currentSegmentLength = 0;
            m_currentPosition += segLen;
            length -= segLen;
            if ((cpoint + 2) < m_polygons.at(m_currentSubpath).size()) {
                ++cpoint;
            } else {
                ++cpoint;
                m_atEnd = true;
                return;
            }
        } else {
            //            m_currentSegmentLength += length;
            m_currentPosition += length;
            m_currentPoint = segPointAtPercent(m_currentPoint, pt, length / segLen);
            break;
        }
    }
    while (true);
}

QPolygonF PathCurl::unwindLength(qreal length)
{
    QPolygonF ret;
    if (cpoint + 1 >= m_polygons.at(m_currentSubpath).size())
        return ret;
    if (m_currentPosition == 0.0) {
        ret.append(m_polygons.at(m_currentSubpath).first());
        m_currentPoint = m_polygons.at(m_currentSubpath).first();
    } else {
        ret.append(m_currentPoint);
    }

    do {
        const QPointF &pt = m_polygons.at(m_currentSubpath).at(cpoint+1);
        qreal segLen = sqrt(pow(pt.x() - m_currentPoint.x(), 2) + pow(pt.y() - m_currentPoint.y() , 2));
        if (length > segLen) {
            ret.append(pt);
            m_currentPoint = pt;
            //            m_currentSegmentLength = 0;
            m_currentPosition += segLen;
            length -= segLen;
            if ((cpoint + 2) < m_polygons.at(m_currentSubpath).size()) {
                ++cpoint;
            } else {
                ++cpoint;
                ret.append(m_polygons.at(m_currentSubpath).at(cpoint));
                return ret;
            }
        } else {
            //            m_currentSegmentLength += length;
            m_currentPosition += length;
            m_currentPoint = segPointAtPercent(m_currentPoint, pt, length / segLen);
            ret.append(m_currentPoint);
            break;
        }
    }
    while (true);
    return ret;
}

qreal PathCurl::currentAngle() const
{
    if (cpoint + 1 >= m_polygons.at(m_currentSubpath).size())
        return 0;
    const QPointF &p1 = m_polygons.at(m_currentSubpath).at(cpoint);
    const QPointF &p2 = m_polygons.at(m_currentSubpath).at(cpoint+1);
    return QLineF(p1, p2).angle();
}

int PathCurl::currentSubpath() const
{
    return m_currentSubpath;
}

void PathCurl::setCurrentSubpath(int index)
{
    if (m_polygons.size() > index) {
        m_currentSubpath = index;
        cpoint = 0;
        m_currentPosition = 0;
        m_currentPoint = m_polygons.at(m_currentSubpath).first();
        m_atEnd = false;
    }
}

QList<QPolygonF> PathCurl::polygons() const
{
    return m_polygons;
}

void PathCurl::setPolygons(const QList<QPolygonF> &polygons)
{
    m_currentSubpathLength = -1;
    cpoint = 0;
    m_currentSubpath = 0;
    m_currentPosition = 0;
    m_polygons = polygons;
    m_currentPoint = m_polygons.at(m_currentSubpath).first();
    m_atEnd = false;

    if( m_polygons.size() == 0 ) return;
    m_boundingBox = m_polygons.first().boundingRect();
    for( int i = 1; i < m_polygons.size(); ++i )
        m_boundingBox = m_boundingBox.united( m_polygons.at( i ).boundingRect() );

}

void PathCurl::setPath(const QPainterPath &path, bool rotate)
{
    m_boundingBox = path.boundingRect();


    m_currentSubpathLength = -1;
    cpoint = 0;
    m_currentSubpath = 0;
    m_currentPosition = 0;
    m_atEnd = false;
    m_polygons = path.toSubpathPolygons();


    for( int pi = 0; pi < m_polygons.size(); ++pi ){
        setCurrentSubpath( pi );
        m_lengths.append( currentSubpathLength() );
        if( rotate ){
            int c = 0;
            for( int i = 0; i < 5; ++i ){
                setPosition( m_lengths[ pi ] * i / 5.0 );
                qreal ang = currentAngle();
                if ( ang > 90 && ang < 270 ) ++c;
            }
            if ( c > 2.5 ) reverseCurrentSubpath();
        }
    }

    m_currentPoint = m_polygons.at(m_currentSubpath).first();
}

int PathCurl::subpathCount() const
{
    return m_polygons.size();
}

qreal PathCurl::currentSubpathLength() const
{
    if( m_lengths.size() > m_currentSubpath )
        return m_lengths.at( m_currentSubpath );
    if (m_currentSubpathLength == -1) {
        const QPolygonF &pol = m_polygons.at(m_currentSubpath);
        m_currentSubpathLength = 0;
        for (int i = 0; i < pol.size() - 1; ++i) {
            m_currentSubpathLength += QLineF(pol[i], pol[i+1]).length();
        }
    }
    return m_currentSubpathLength;
}

void PathCurl::reverseCurrentSubpath()
{
    QPolygonF &pol = m_polygons[m_currentSubpath];
    std::reverse(pol.begin(), pol.end());
}

bool PathCurl::atEnd() const
{
    return m_atEnd;
}

QRectF PathCurl::boundingRect() const
{
    return m_boundingBox;
}


