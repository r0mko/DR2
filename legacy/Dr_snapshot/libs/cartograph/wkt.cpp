#include "wkt.h"
#include <QDebug>

template <typename T>
QDataStream &operator>>(QDataStream &in, WktCollectionEntity<T> &c)
{
    quint8 magic;
    in >> magic;
    if (magic != WktCollectionEntity<T>::magic) {
        in.setStatus(QDataStream::ReadCorruptData);
        return in;
    }
    uint geomtype;
    in >> geomtype;
    c.m_geomtype = (GeomType)geomtype;
    in >> c.t;
    return in;
}

QPainterPath WktLineString::getPath(const QTransform &transform) const
{
    QPainterPath p;
    p.addPolygon(transform.map(d.value()));
    return p;
}

QPolygonF WktLineString::getPolygon(const QTransform &transform) const
{
    return transform.map(d.value());
}

QPainterPath WktPolygon::getPath(const QTransform &transform) const
{
    QPainterPath p;
    for (QPolygonF ring:d.value()) {
        p.addPolygon(ring);
        p.closeSubpath();
    }
    return transform.map(p);

}

QPolygonF WktPolygon::getPolygon(const QTransform &transform) const
{
    QPolygonF ret;
    for (QPolygonF ring:d.value()) {
        ret.append(ring);
    }
    return transform.map(ret);
}

QPainterPath WktMultiPoint::getPath(const QTransform &transform) const
{
    QPainterPath p;
    QPolygonF pol;
    for (const WktCollectionEntity<QPointF> &v:d.value()) {
        pol.append(transform.map(v.t));
    }
    p.addPolygon(pol);
    return p;
}

QPolygonF WktMultiPoint::getPolygon(const QTransform &transform) const
{
    QPolygonF pol;
    for (const WktCollectionEntity<QPointF> &v:d.value()) {
        pol.append(transform.map(v.t));
    }
    return pol;
}

QPainterPath WktMultiLineString::getPath(const QTransform &transform) const
{
    QPainterPath p;
    for (const WktCollectionEntity<QPolygonF> &c:d.value()) {
        p.addPolygon(transform.map(c.t));
    }
    return p;
}

QPolygonF WktMultiLineString::getPolygon(const QTransform &transform) const
{
    QPolygonF ret;
    for (const WktCollectionEntity<QPolygonF> &c:d.value()) {
        ret.append(transform.map(c.t));
    }
    return ret;
}

QPainterPath WktMultiPolygon::getPath(const QTransform &transform) const
{
    QPainterPath p;
    for (const WktCollectionEntity<QList<QPolygonF>> &c:d.value()) {
        for (QPolygonF pol_1:c.t) {
            p.addPolygon(transform.map(pol_1));
        }
    }
    return p;
}

QPolygonF WktMultiPolygon::getPolygon(const QTransform &transform) const
{
    QPolygonF ret;
    for (const WktCollectionEntity<QList<QPolygonF>> &c:d.value()) {
        for (QPolygonF pol_1:c.t) {
            ret.append(transform.map(pol_1));
        }
    }
    return ret;
}

QPainterPath WktPoint::getPath(const QTransform &transform) const
{
    QPainterPath p;
    p.moveTo(transform.map(d.value()));
    return p;
}

QPolygonF WktPoint::getPolygon(const QTransform &transform) const
{
    return {{ transform.map(d.value()) }};
}

GeoEntity *readWkb(QDataStream &wkb)
{
    uint gtype;
    wkb >> gtype;
    switch (gtype) {
    case Point: {
        auto p = new WktPoint;
        p->load(wkb);
        return p;
    }
    case LineString: {
        auto p = new WktLineString;
        p->load(wkb);
        return p;
    }
    case PolygonType: {
        auto p = new WktPolygon;
        p->load(wkb);
        return p;
    }
    case MultiPoint: {
        auto p = new WktMultiPoint;
        p->load(wkb);
        return p;
    }
    case MultiLineString: {
        auto p = new WktMultiLineString;
        p->load(wkb);
        return p;
    }
    case MultiPolygon: {
        auto p = new WktMultiPolygon;
        p->load(wkb);
        return p;
    }
    default:
        qWarning() << __func__ << (GeomType)gtype << "unknown";
        return nullptr;
    }
}

void SpatialEntity::parseBlob(const QByteArray &wkblob)
{
    if (entity) {
        delete entity;
        entity = nullptr;
    }
    QDataStream st(wkblob);
    quint32 srid;
    quint8 zero;
    quint8 endian;
    QPointF ul;
    QPointF br;
    quint8 sevenCee;
    st >> zero >> endian;
    if(zero != 0) {
        qWarning() << "Spatialite geometry error at pos" << st.device()->pos();
        return;
    }
    if (endian == 0)
        st.setByteOrder(QDataStream::BigEndian);
    else if (endian == 1)
        st.setByteOrder(QDataStream::LittleEndian);
    else {
        qWarning() << "Spatialite geometry error at pos" << st.device()->pos();
        return;
    }
    st >> srid >> ul >> br >> sevenCee;
    if(sevenCee != 0x7c) {
        qWarning() << "Spatialite geometry error at pos" << st.device()->pos();
        return;
    }
    entity = readWkb(st);
    quint8 end;
    st >> end;
}
