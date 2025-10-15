#ifndef WKT_H
#define WKT_H
#include <QDataStream>
#include <QPainterPath>
#include <QPolygonF>
#include <QTransform>
#include <QVariantList>

enum GeomType {
    Invalid= -1,
    Geometry = 0,
    Point = 1,
    LineString = 2,
    PolygonType = 3,
    MultiPoint = 4,
    MultiLineString = 5,
    MultiPolygon = 6,
    GeometryCollection = 7,
    CircularString = 8,
    CompoundCurve = 9,
    CurvePolygon = 10,
    MultiCurve = 11,
    MultiSurface = 12,
    Curve = 13,
    Surface = 14,
    PolyhedralSurface = 15,
    TIN = 16,
    Triangle = 17,
    WKT_M = 1000,
    WKT_Z = 2000,
    WKT_ZM = 3000
};
//Q_ENUM(GeomType)

class GeoEntity
{
    GeomType m_type;
public:
    GeoEntity(GeomType type) : m_type(type) {}
    GeomType type() const { return m_type; }
    virtual ~GeoEntity() {}
    virtual QPainterPath getPath(const QTransform &transform) const = 0;
    virtual QPolygonF getPolygon(const QTransform &transform) const = 0;
    virtual void load(QDataStream &s) = 0;
};

class SpatialEntity
{
public:
    SpatialEntity() {}
    ~SpatialEntity() { if (entity) delete entity; }
    QVector<QVariant> attributes;
    GeoEntity *entity = nullptr;
    QByteArray wkblob;
    QPainterPath shape;
    int kind = -1;
    void parseBlob(const QByteArray &wkblob);
};
Q_DECLARE_METATYPE(SpatialEntity)
template <typename T> class Geom
{
    T t;
public:
    T &value() { return t; }
    const T &value() const { return t; }
    void load(QDataStream &s) {
        s >> t;
    }
};

template <typename T>
class WktCollectionEntity
{
public:
    static const quint8 magic = 0x69;
    GeomType m_geomtype;
    T t;
};


template <typename T>
class WktCollection : public QList<WktCollectionEntity<T>>
{};


template <typename T>
QDataStream &operator>>(QDataStream &in, WktCollectionEntity<T> &c);


class WktPoint : public GeoEntity
{
    Geom<QPointF> d;

public:
    WktPoint() : GeoEntity(Point) {}
    QPainterPath getPath(const QTransform &transform) const;
    QPolygonF getPolygon(const QTransform &transform) const;
    void load(QDataStream &s) { d.load(s); }

};
class WktLineString : public GeoEntity
{
    Geom<QPolygonF> d;
public:
    WktLineString() : GeoEntity(LineString) {}
    QPainterPath getPath(const QTransform &transform) const;
    QPolygonF getPolygon(const QTransform &transform) const;
    void load(QDataStream &s) { d.load(s); }

    // GeoEntity interface
};

class WktPolygon : public GeoEntity
{
    Geom<QList<QPolygonF>> d;
public:
    WktPolygon() : GeoEntity(PolygonType) {}
    QPainterPath getPath(const QTransform &transform) const;
    QPolygonF getPolygon(const QTransform &transform) const;
    void load(QDataStream &s) { d.load(s); }
};

class WktMultiPoint : public GeoEntity
{
    Geom<WktCollection<QPointF>> d;
public:
    WktMultiPoint() : GeoEntity(MultiPoint) {}
    QPainterPath getPath(const QTransform &transform) const;
    QPolygonF getPolygon(const QTransform &transform) const;
    void load(QDataStream &s) { d.load(s); }
};

class WktMultiLineString : public GeoEntity
{
    Geom<WktCollection<QPolygonF>> d;
public:
    WktMultiLineString() : GeoEntity(MultiLineString) {}
    QPainterPath getPath(const QTransform &transform) const;
    QPolygonF getPolygon(const QTransform &transform) const;
    void load(QDataStream &s) { d.load(s); }
};

class WktMultiPolygon : public GeoEntity
{
    Geom<WktCollection<QList<QPolygonF>>> d;
public:
    WktMultiPolygon() : GeoEntity(MultiPolygon) {}
    QPainterPath getPath(const QTransform &transform) const;
    QPolygonF getPolygon(const QTransform &transform) const;
    void load(QDataStream &s) { d.load(s); }
};

#endif // WKT_H
