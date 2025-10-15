#ifndef SNIPFILTER_H
#define SNIPFILTER_H

#include <QObject>
#include <QPainterPath>
#include <QStringList>
#include <QHash>
#include <QRawFont>
#include <QColor>

class PathCurl
{
public:
    PathCurl() {}

    void setPosition(qreal pos);
    void skipLength(qreal length);
    QPolygonF unwindLength(qreal length);

    inline qreal currentPosition() const { return m_currentPosition; }
    inline const QPointF &currentPoint() const { return m_currentPoint; }

    qreal currentAngle() const;

    int currentSubpath() const;
    void setCurrentSubpath(int index);

    QList<QPolygonF> polygons() const;
    void setPolygons(const QList<QPolygonF> &polygons);
    void setPath( const QPainterPath &path, bool rotate = false ); // alternative

    int subpathCount() const;
    qreal currentSubpathLength() const;

    void reverseCurrentSubpath();
    bool atEnd() const;

    QRectF boundingRect() const;

private:
    int m_currentSubpath;
    QList<QPolygonF> m_polygons;
    QVector<qreal> m_lengths;
    QRectF m_boundingBox;
    int cpoint;
    qreal m_currentPosition;
    mutable qreal m_currentSubpathLength;
    QPointF m_currentPoint;
    bool m_atEnd;
};



class QPainter;
class StreetLayer;
class SnipFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList snipTypeList READ snipTypeList WRITE setSnipTypeList NOTIFY snipTypeListChanged)
    Q_PROPERTY(qreal fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(qreal threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)

public:
    explicit SnipFilter(QObject *parent = 0);
    void paint(QPainter* painter, const QTransform &tr , const QRectF &viewportRect);
    void addStreet(QPainterPath path, QString name );

    QStringList snipTypeList() const;
    qreal fontSize() const;
    StreetLayer *streetLayer() const;

    void setFont( QFont font );
    void setSnipTypeList(QStringList arg);
    void setFontSize(qreal arg);
    void setStreetLayer(StreetLayer *streetLayer);

    void clearData();

    const QHash<quint32, QPainterPath> &glyphPathCache() const;
    void setGlyphPathCache(const QHash<quint32, QPainterPath> &glyphPathCache);

    qreal threshold() const;

public slots:
    void setThreshold(qreal arg);

signals:
    void snipTypeListChanged(QStringList arg);
    void fontSizeChanged(qreal arg);


    void thresholdChanged(qreal arg);

private:

    QStringList m_snipTypeList;
    qreal m_fontSize;

    struct StreetLabel{
        QVector< quint32 > glyphIndexList;
//        QPainterPath path;
        QVector< QPointF > advances;
        qreal labelWidth;
        PathCurl curl;
    };
    QList< StreetLabel > m_streetList;

    void paintLabel(QPainter* painter, const QRawFont &rawfont, const QTransform& tr, StreetLabel &s)
    {

        qreal scale = (tr.m11() + tr.m22()) / 2;

        if( scale < m_threshold ) return;
        //    s.curl.setPolygons( s.path.toSubpathPolygons( tr ) );
        //    qDebug() << curl.currentAngle();

        for (int sp = 0; sp < s.curl.subpathCount(); ++sp) {
            s.curl.setCurrentSubpath(sp);
            s.curl.setPosition(100 / scale);
            if (s.curl.currentSubpathLength() < (s.labelWidth + 100) / scale)
                continue;
            while (!s.curl.atEnd()) {
                int i = 0;
                for( quint32 id : s.glyphIndexList ) {
                    QPointF pt = s.curl.currentPoint();
                    if (!m_streetLayer->viewPortRect().contains(pt)) {
                        s.curl.skipLength( s.advances.at( i++ ).x() / scale );
                        continue;
                    }
                    painter->save();
                    painter->translate(tr.map(pt));
                    painter->rotate(-s.curl.currentAngle());
                    painter->translate(0, (rawfont.ascent() - rawfont.descent()) / 2);
                    //            if (m_outlineWidth > 0)
                    //                painter->strokePath(glyphPathCache()[ id ], QPen(m_outlineColor, m_outlineWidth));
                    //                painter->fillPath(glyphPathCache()[ id ], QBrush(m_color));
                    painter->drawPath(glyphPathCache()[ id ]);
                    painter->restore();
                    s.curl.skipLength( s.advances.at( i++ ).x() / scale );
                    if (s.curl.atEnd())
                        break;
                }
                s.curl.skipLength( 200 / scale );
                if (s.curl.currentSubpathLength() - s.curl.currentPosition() < s.labelWidth / scale )
                    break;
            }
        }
    }


    QRawFont m_rawFont;
    QHash< quint32, QPainterPath > m_glyphPathCache;

    StreetLayer *m_streetLayer;

    qreal m_threshold;
};

#endif // SNIPFILTER_H
