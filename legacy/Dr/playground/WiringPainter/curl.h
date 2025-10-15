#ifndef CURL_H
#define CURL_H
#include <QtMath>
#include <QObject>
#include <QMetaProperty>
#include <QPolygonF>
#include <QPainterPath>

class GPolygon;
class GraphicsPath;

class Curl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GPolygon* polygon READ polygon WRITE setPolygon NOTIFY polygonChanged)
    Q_PROPERTY(GraphicsPath* path READ path WRITE setPath NOTIFY pathChanged)

public:
    explicit Curl(QObject *parent = 0);
    Q_INVOKABLE QPointF currentPoint() const;
    Q_INVOKABLE qreal length() const;
    Q_INVOKABLE qreal currentPosition() const;
    Q_INVOKABLE qreal currentSegmentLength() const;
    Q_INVOKABLE qreal currentSegmentPosition() const;
    Q_INVOKABLE qreal nearestObstacle(qreal max = HUGE) const;


    Q_INVOKABLE int currentSegment() const;
    Q_INVOKABLE bool atEnd() const;
    Q_INVOKABLE QPointF getTangent(qreal radius, int order, bool *onSameSegment = nullptr, qreal *angle = nullptr, qreal position = 0.5);
    QPainterPath getPath(qreal smooth, qreal offset, int order = 4, qreal curveThreshold = 2);

    GPolygon *polygon() const;
    GraphicsPath *path() const;

signals:
    void polygonChanged(GPolygon* arg);
    void pathChanged(GraphicsPath* arg);

public slots:
    void init();
    void rewind();
    void rewindToNode(int nodeIdx);
    void wind(qreal length);
    void save();
    void restore();

    void updatePath(qreal smooth, qreal offset, int order = 4, qreal curveThreshold = 2);

    void setPolygon(GPolygon* arg);
    void setPath(GraphicsPath* arg);

private:
    QPolygonF getPolygon() const;
    void writePath(const QPainterPath &path) const;
    QMetaProperty polProp;
    QMetaProperty pathProp;
    qreal m_length;
    GPolygon* m_polygon;

    int     cpoint;
    qreal   m_currentPosition;
    QPointF m_currentPoint;
    bool    m_atEnd;

    int     s_cpoint;
    qreal   s_m_currentPosition;
    QPointF s_m_currentPoint;
    bool    s_m_atEnd;

    GraphicsPath* m_path;
    QVector<QPointF> pts;
};



#endif // CURL_H
