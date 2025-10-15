#ifndef DEFS_H
#define DEFS_H
#include <QPointF>
#include <QPolygonF>
#include <QHash>
#include <QPainterPath>
#include <QList>
#include <QtMath>
#include <QDateTime>
#include <QMutex>
#include <QDebug>
#include <QSGGeometry>
//#include <QVariant>

#define Q_FIXED_POINT_SCALE 32
inline void serialize(QDataStream &) {}

template<typename Arg1, typename ...Args>
inline void serialize(QDataStream &s, const Arg1& arg1, const Args&... args) { s << arg1; serialize(s, args...); }

inline void deserialize(QDataStream &) {}

template<typename Arg1, typename ...Args>
inline void deserialize(QDataStream &s, Arg1& arg1, Args&... args) { s >> arg1; deserialize(s, args...); }

uint qHash(const QPointF &key, uint seed);

QPolygonF reversedPolygon(const QPolygonF &pol);
void copyPolygon(const QPolygonF &pol, QPainterPath &path);

qreal normalizeAngle(qreal angle);

qreal segmentAngle(qreal startAngle, qreal endAngle);

//bool attachPolygon(QPolygonF &dest, const QPolygonF &pol);
QPainterPath buildPath(QList<QPolygonF> parts);

QPolygonF mergePolygons(const QPolygonF &one, const QPolygonF &other);

QPainterPath multipolygon(QList<QPolygonF> outer, QList<QPolygonF> inner = QList<QPolygonF>());

inline qint64 now() { return QDateTime::currentMSecsSinceEpoch(); }

inline qreal rad2deg(qreal rad)
{
    return (rad * 180) / M_PI;
}

inline qreal deg2rad(qreal deg)
{
    return (deg * M_PI) / 180;
}

inline QPointF sphereMercator(const QPointF &coords, qreal radius)
{
    return QPointF(deg2rad(coords.y()) * radius, radius * log(tan(M_PI_4 + deg2rad(coords.x())/2)));
}

inline QPointF invSphereMercator(const QPointF &coords, qreal radius)
{
    return QPointF(rad2deg(2 * atan(exp( (coords.y() / radius ) )) - M_PI/2), rad2deg(coords.x() / radius));
}

class VertexBuffer
{
public:
    VertexBuffer() {}

    quint32 addVertex(const QPointF &pt, bool fill = true);
    void addLine(const QPointF &p1, const QPointF &p2);
    void addTriangle(const QPointF &p1, const QPointF &p2, const QPointF &p3);

    void addPolyLine(const QPolygonF &pol);
    void addPolyLines(const QList<QPolygonF> &pols);
    void addPoints(const QPolygonF &pol);

    void mergeFill(const VertexBuffer &buffer);
    void clear();

    inline QVector<QPointF> vertices() const { return m_vertices; }
    inline QVector<uint> fillIndices() const { return m_fillIndices; }
    inline QVector<uint> lineIndices() const { return m_lineIndices; }

    inline int vertexCount() const { return m_vertices.size(); }
    inline int fillIndexCount() const { return m_fillIndices.size(); }
    inline int lineIndexCount() const { return m_lineIndices.size(); }

    inline QPointF vertex(int i) const { return m_vertices.at(i); }
    inline QSGGeometry::Point2D point2d(int i) const { const QPointF &pt = m_vertices.at(i); return { (float)pt.x(), (float)pt.y() }; }
    inline uint lineIndex(int i) const { return m_lineIndices.at(i); }
    inline uint fillIndex(int i) const { return m_fillIndices.at(i); }

    inline bool hasFill() const { return m_fillIndices.size() > 0; }
    inline bool hasLine() const { return m_lineIndices.size() > 0; }

    QSGGeometry *createPointGeometry() const;
    QSGGeometry *createLineGeometry() const;
    QSGGeometry *createFillGeometry() const;
    QSGGeometry *createOutlineGeometry() const;

    bool updatePointGeometry(QSGGeometry *g, bool force = false) const;
    bool updateLineGeometry(QSGGeometry *g, bool force = false) const;
    bool updateFillGeometry(QSGGeometry *g, bool force = false) const;
    bool updateOutlineGeometry(QSGGeometry *g, bool force = false) const;

    QVector<QPointF> &verticesRef() { return m_vertices; }
    QVector<uint> &fillIndicesRef() { return m_fillIndices; }
    QVector<uint> &lineIndicesRef() { return m_lineIndices; }

    QVector<uint> generateOutlineIndices() const;
    QList<QPolygonF> generatePolygons() const;

    inline void merge(const VertexBuffer &other) { for (uint idx:other.fillIndices()) addVertex(other.vertices().at(idx), true); }
    void initTable();

private:
    void commitLine(QSGGeometry *g) const;
    void commitFill(QSGGeometry *g) const;
    void commitIndexBuffer(QSGGeometry *g, const QVector<uint> &idx) const;
    QHash<QPointF, quint32>     point2index;
    QVector<QPointF>            m_vertices;
    QVector<uint>               m_fillIndices;
    QVector<uint>               m_lineIndices;
};

Q_DECLARE_METATYPE(VertexBuffer)

#endif // DEFS_H
