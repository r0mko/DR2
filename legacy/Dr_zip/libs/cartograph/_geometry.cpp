#include "geometry.h"
#include <QDebug>
#include "spatialitedb.h"
#include "qtransform.h"
#include <QSGNode>
#include "style.h"
#include "maplayer.h"
#include "mapview.h"
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QDir>
#include <QFile>

void Geometry::setQuery(QString arg)
{
    if (m_query == arg)
        return;

    m_query = arg;
    emit queryChanged(arg);
    tryFetch();
}

void Geometry::setLayerName(QString arg)
{
    if (m_layerName == arg)
        return;
    //    qDebug() << "layer was set" << this << arg;
    m_layerName = arg;
    updateQuery();
    emit layerNameChanged(arg);
}

void Geometry::setStyle(Style *arg)
{
    if (m_style == arg)
        return;

    m_style = arg;
    qDebug() << "set style" << arg << arg->nodes();
    emit styleChanged(arg);
}

void Geometry::setType(Geometry::GeomType arg)
{
    if (m_type == arg)
        return;

    m_type = arg;
    emit typeChanged(arg);
}

void Geometry::setWhere(QString arg)
{
    if (m_where == arg)
        return;
    m_where = arg;
    updateQuery();
    emit whereChanged(arg);
}

void Geometry::setLayer(MapLayer *arg)
{
    if (m_layer == arg)
        return;
    if (m_layer)
        m_layer->disconnect(0, this, 0);
    m_layer = arg;
    if (m_layer)
//    connect(m_layer, &MapLayer::viewReady, this, &Geometry::updateQuery);
    connect(this, &Geometry::buffersReady, m_layer, &MapLayer::update, Qt::QueuedConnection);
    emit layerChanged(arg);
    emit sourceSRIDChanged();
    emit databaseChanged();

}

void Geometry::tryFetch()
{
    if (!m_style) {
        qDebug() << "No style defined for layer" << m_layerName;
        return;
    }
    if (database()) {
        if (!m_query.isEmpty())
            database()->fetchGeometry(this, m_query, database()->layerInfo(m_layerName).geometryColumn());
    }
}


void Geometry::updateQuery()
{
//    if (!database() || !database()->isOpen())
//        return;
//    SLVectorLayer l = database()->layerInfo(m_layerName);
//    if (l.geometryType() == Invalid) {
//        qDebug() << "Layer" << m_layerName << "not found!";
//        return;
//    }
//    QString geomColumn = database()->layerInfo(m_layerName).geometryColumn();

//    QString what;
//    if (targetSRID() != 0) {
//        if (m_srid != 0)
//            what = QString("Transform(%1, %2) as %3").arg(geomColumn).arg(targetSRID()).arg(geomColumn);
//        else
//            what = QString("Transform(SetSRID(%1, 4326), %2) as %3").arg(geomColumn).arg(targetSRID()).arg(geomColumn);
//    } else
//        what = geomColumn;
//    QString q = QString("SELECT %1 FROM %2").arg(what).arg(m_layerName);
//    if (!m_where.isEmpty()) {
//        q += QString(" WHERE %1").arg(m_where);
//    }
//    m_query = q;
//    emit queryChanged(q);
//    tryFetch();
}



void Geometry::setHidden(bool arg)
{
    if (m_hidden == arg)
        return;

    m_hidden = arg;
    qDebug()<< "Hidden" << m_hidden;
    emit hiddenChanged();
}

void Geometry::setComplete(bool arg )
{
    m_complete = arg;
//    QFuture<void> future = QtConcurrent::run(this, &Geometry::buildBuffers, layer()->view()->transform());
    buildBuffers(layer()->view()->transform());
    emit completed();
}

QSGNode *Geometry::node() const
{
    if (m_inScene)
        return m_node;
    m_node = new QSGNode;
    for (StyleNode *n:m_style->nodes()) {
        m_node->appendChildNode(n->getNode());
    }
    m_inScene = true;
    return m_node;
}

uint Geometry::targetSRID() const
{
    if (!m_layer)
        return 0;
    return m_layer->targetSRID();
}


QPainterPath Geometry::getPath(const QTransform &transform, bool clearData) const
{
    Q_UNUSED(clearData)
    QPainterPath ret;
    qDebug() << "Building path";
    int i = 0;
    for(GeoEntity *e:m_geometry) {
//                qDebug() << "Adding entity" << e;
        ret.addPath(e->getPath(transform));
        ++i;
    }
    qDebug() << "Processed" << i << "geoEntities";
//    qDebug() << "Generated path size" << ret.elementCount() << ret.elementAt(0).x<< ret.elementAt(0).y;
    return ret;
}

SpatiaLiteDB *Geometry::database() const
{
    if (!m_layer)
        return nullptr;
    return m_layer->database();
}

void Geometry::buildBuffers(const QTransform &transform)
{
    if (!m_style) {
        qDebug() << "no style defined for layer" << layerName();
        return;
    }
    qDebug() << "Building buffers for" << layerName() << m_style;

    for (StyleNode *n:m_style->nodes()) {
//        n->setVertexBuffer();
        n->buildBuffer(transform, this);
    }
    qDebug() << "Building buffers for" << layerName() << "done";
    m_buffersComplete = true;
//    if( m_cache )
//        saveToCache( QUrl( QString( "%1/%2%3" ).arg( m_cachePath ).arg( m_defaultCachePrefix ).arg( m_cacheId ) ) );
    emit buffersReady();
}

void Geometry::readWkb(QDataStream &wkb)
{
    uint gtype;
    wkb >> gtype;
    switch (gtype) {
    case Point: {
        auto p = new WktPoint;
        p->load(wkb);
        m_geometry.append(p);
        return;
    }
    case LineString: {
        auto p = new WktLineString;
        p->load(wkb);
        m_geometry.append(p);
        return;
    }
    case Polygon: {
        auto pol = new WktPolygon;
        pol->load(wkb);
        m_geometry.append(pol);
        return;
    }
    case MultiPoint: {
        auto p = new WktMultiPoint;
        p->load(wkb);
        m_geometry.append(p);
        return;
    }
    case MultiLineString: {
        auto p = new WktMultiLineString;
        p->load(wkb);
        m_geometry.append(p);
        return;
    }
    case MultiPolygon: {
        auto p = new WktMultiPolygon;
        p->load(wkb);
        m_geometry.append(p);
        return;
    }
    default:
        qWarning() << __func__ << (GeomType)gtype << "unknown";
        return;
    }
}
void Geometry::parseBlob(const QByteArray &ba)
{
    QDataStream st(ba);
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
    readWkb(st);
    quint8 end;
    st >> end;
}

uint Geometry::sourceSRID() const
{
    if (m_layer)
        return m_layer->sourceSRID();
    return 0;
}

QDebug operator<<(QDebug dbg, const GeomType &c)
{
    switch (c) {
    case Invalid:
        dbg << "WKT::Invalid";
        break;
    case Geometry:
        dbg << "WKT::Geometry";
        break;
    case Point:
        dbg << "WKT::Point";
        break;
    case LineString:
        dbg << "WKT::LineString";
        break;
    case Polygon:
        dbg << "WKT::Polygon";
        break;
    case MultiPoint:
        dbg << "WKT::MultiPoint";
        break;
    case MultiLineString:
        dbg << "WKT::MultiLineString";
        break;
    case MultiPolygon:
        dbg << "WKT::MultiPolygon";
        break;
    case GeometryCollection:
        dbg << "WKT::GeometryCollection";
        break;
    case CircularString:
        dbg << "WKT::CircularString";
        break;
    case CompoundCurve:
        dbg << "WKT::CompoundCurve";
        break;
    case CurvePolygon:
        dbg << "WKT::CurvePolygon";
        break;
    case MultiCurve:
        dbg << "WKT::MultiCurve";
        break;
    case MultiSurface:
        dbg << "WKT::MultiSurface";
        break;
    case Curve:
        dbg << "WKT::Curve";
        break;
    case Surface:
        dbg << "WKT::Surface";
        break;
    case PolyhedralSurface:
        dbg << "WKT::PolyhedralSurface";
        break;
    case TIN:
        dbg << "WKT::TIN";
        break;
    case Triangle:
        dbg << "WKT::Triangle";
        break;
    case WKT_M:
        dbg << "WKT::WKT_M";
        break;
    case WKT_Z:
        dbg << "WKT::WKT_Z";
        break;
    case WKT_ZM:
        dbg << "WKT::WKT_ZM";
        break;
    default:
        break;
    }
    return dbg;
}
