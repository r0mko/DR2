#include "geolayer.h"
#include "geombuffer.h"
#include "geoview.h"
#include "spatialitedb.h"
#include <QElapsedTimer>
#include <QSGNode>
#include <QtConcurrent>

//const QRectF GeoLayer::maxRect = QRectF(-33554431, -33554431, 67108863, 67108863);
//const QRectF GeoLayer::maxRect = QRectF(-50000, -50000, 100000, 100000);
//const QRectF GeoLayer::maxRect = QRectF(-4194303, -4194303, 8388608, 8388608);
const QRectF GeoLayer::maxRect = QRectF(-419430, -419430, 838860, 838860);
GeoLayer::GeoLayer(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlags(QQuickItem::ItemHasContents);
    setClip(false);
}

GeoLayer::~GeoLayer()
{
    qDebug() << "Layer destructed" << m_buffers.size();
    for (GeomBuffer* b : m_buffers) {
        b->reset();
    }
//    m_buffers.clear();

}

int GeoLayer::srid() const
{
    if (!m_dataSource || m_table.isEmpty())
        return -1;
    return m_dataSource->layerInfo(m_table).srid();
}

//void GeoLayer::setSrid(int arg)
//{
//    if (m_srid == arg)
//        return;
//    m_srid = arg;
//    emit sridChanged(arg);
//}

QTransform GeoLayer::geomBufferTransform() const
{
    return m_transform;
}

QRectF GeoLayer::extent() const
{
    return m_extent;
}

bool GeoLayer::isValid() const
{
    return m_isValid;
}

int GeoLayer::entityCount() const
{
    if (!m_isValid)
        return 0;
    return m_dataSource->layerInfo(m_table).entityCount();

}

void GeoLayer::componentComplete()
{
    QQuickItem::componentComplete();
    if (!m_isValid)
        initLayer();
}

QSGNode *GeoLayer::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data)
{
    qDebug() << "updating layer" << m_table << "node" << node;
    if (!node) {
        node = new QSGNode;
        node->setFlag(QSGNode::OwnedByParent);
    }
    if (!highlight) {
        highlight = new QSGNode;
        highlight->setFlag(QSGNode::OwnedByParent);
    }
    QTransform combined = m_transform * m_view->viewTransform();
    if (combined != data->transformNode->matrix().toTransform()) {
//        qDebug() << "Updating paint node" << node << combined.mapRect(maxRect) << boundingRect() << m_table << m_extent << srid();
        data->transformNode->setMatrix(combined);
        //    node->markDirty(QSGNode::DirtyMatrix);
    }

    if( highlight->parent() == node ) {
        node->removeChildNode(highlight);
    }
    qDebug() << node->childCount() << m_buffers.size();
    if (node->childCount() != m_buffers.size()) {
        qDebug() << "buffers and children mismatch, childcount" << node->childCount() << "geom count" << m_buffers.size();
        node->removeAllChildNodes();
    }
//    node->removeChildNode(highlight);
    QSGNode *child = node->firstChild();

    for (GeomBuffer *n : m_buffers) {
        QSGNode *nd = n->updateNode(child);
        if (!child) {
            node->appendChildNode(nd);
            child = nd;
        } else if (child != nd) {
            qWarning() << "changed node! new =" << nd << "old =" << child << "remove old child";
            node->insertChildNodeAfter(nd, child);
            node->removeChildNode(child);
            child = nd;
//            delete child;
        }
        child = child->nextSibling();
    }
    highlight->removeAllChildNodes();
    for (GeomBuffer *n : m_buffers) {
        if (n->highlight())
            highlight->appendChildNode(n->getHightlightNode());
    }
    node->appendChildNode(highlight);

//    if (node->childCount() != m_buffers.size())
//        node->removeAllChildNodes();
//    QSGNode *child = node->firstChild();
//    for (GeomBuffer *n : m_buffers) {
//        QSGNode *nd = n->updateNode(child);
//        if (child && child != nd) {
//            node->removeChildNode(child);
//        }
//            if (nd) {
////                qDebug() << "add node" << n->clause();
//                node->appendChildNode(nd);
//            }
//        }
//    }
    return node;
}

qreal GeoLayer::progress() const
{
    return (qreal)m_fetchedCount / entityCount();
}

void GeoLayer::insertBuffer(int index, GeomBuffer *geom)
{
    if (m_buffers.contains(geom))
        return;
    if (m_buffers.size() < index)
        m_buffers.resize(index - 1);
    m_buffers.insert(index, geom);
    connect(geom, &GeomBuffer::readyChanged, this, &GeoLayer::update, Qt::UniqueConnection);
}

void GeoLayer::removeBuffer(int index)
{
    GeomBuffer *b = m_buffers.takeAt(index);
    b->reset();
    update();
}

void GeoLayer::removeBuffer(GeomBuffer *geom)
{
    m_buffers.removeAll(geom);
    update();
}

void GeoLayer::appendBuffer(GeomBuffer *geom)
{
    if (m_buffers.contains(geom))
        return;
    m_buffers.append(geom);
    connect(geom, &GeomBuffer::readyChanged, this, &GeoLayer::update, Qt::UniqueConnection);
    emit geometryBuffersChanged();
}

void GeoLayer::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &v)
{
    switch (change) {
    case QQuickItem::ItemParentHasChanged:
        initLayer();
        break;
    default:
        break;
    }
}

void GeoLayer::initLayer()
{
//    qDebug() << "try initializing layer" << m_table << this << m_view << m_dataSource;
    if (!m_view || !m_dataSource || m_table.isEmpty()) {
        setValid(false);
        return;
    }
    if (!m_dataSource->ready()) {
        setValid(false);
        if (connect(m_dataSource, &SpatiaLiteDB::readyChanged, this, &GeoLayer::initLayer, Qt::UniqueConnection))
            qDebug() << "Datasource is not ready yet";
        return;
    }
    if (!m_dataSource->hasLayer(m_table)) {
        qWarning() << qPrintable(QString("layer '%1' not found!").arg(m_table));
    }
//    qDebug() << "initializing layer" << m_table;
    SpatialLayer l = m_dataSource->layerInfo(m_table);
    m_geometryColumn = l.geometryColumn();
    //    Q_ASSERT(!m_geometryColumn.isEmpty());
    setExtent(m_dataSource->extent(m_table, m_view->targetSRID()));
//    setSrid(l.srid());
    emit sridChanged(l.srid());
//    qDebug() << "table" << l.tableName() << "srid" << l.srid();
    m_geometryType = (GeometryType)l.geometryType();
    switch (m_geometryType) {
    case Point:
    case MultiPoint:
        m_entityKind = PointEntity;
        break;
    case LineString:
    case MultiLineString:
    case CircularString:
    case CompoundCurve:
    case MultiCurve:
    case Curve:
        m_entityKind = LinearEntity;
        break;
    case Polygon:
    case MultiPolygon:
    case CurvePolygon:
    case MultiSurface:
    case Surface:
    case PolyhedralSurface:
        m_entityKind = PolygonEntity;
        break;
    case GeometryCollection:
        m_entityKind = CompoundObjects;
        break;
    default:
        m_entityKind = UnknownGeometry;
        break;
    }
    emit geometryTypeChanged();
    emit geometryKindChanged(m_entityKind);
    emit entityCountChanged(l.entityCount());
    setValid(true);
}

void GeoLayer::setDataSource(SpatiaLiteDB *arg)
{
    if (m_dataSource == arg)
        return;
    if (m_dataSource) {
        m_dataSource->disconnect(0, this, 0);
    }
    m_dataSource = arg;
    if (!m_dataSource->ready()) {
        connect(m_dataSource, &SpatiaLiteDB::readyChanged, this, &GeoLayer::initLayer, Qt::UniqueConnection);
    }
    emit dataSourceChanged();
    initLayer();
}

QString GeoLayer::geometryColumn() const
{
    return m_geometryColumn;
}

QString GeoLayer::table() const
{
    return m_table;
}

void GeoLayer::setGeometryColumn(QString geometryColumn)
{
    if (m_geometryColumn == geometryColumn)
        return;
    m_geometryColumn = geometryColumn;
    emit geometryColumnChanged();
}

void GeoLayer::setGeometryType(GeoLayer::GeometryType type)
{
    if (m_geometryType == type)
        return;
    m_geometryType = type;
    emit geometryTypeChanged();
}

QQmlListProperty<GeomBuffer> GeoLayer::qmlGeometryBuffers() {
    return QQmlListProperty<GeomBuffer>(this, 0,
                                        &GeoLayer::qmlGeometryBuffersCount,
                                        &GeoLayer::qmlGeometryBuffersAt);
}

int GeoLayer::qmlGeometryBuffersCount(QQmlListProperty<GeomBuffer> *list)
{
    GeoLayer *o = qobject_cast<GeoLayer*>(list->object);
    return o->m_buffers.size();
}

GeomBuffer *GeoLayer::qmlGeometryBuffersAt(QQmlListProperty<GeomBuffer> *list, int index)
{
    GeoLayer *o = qobject_cast<GeoLayer*>(list->object);
    return o->m_buffers.at(index);
}

void GeoLayer::setTable(QString geometryTable)
{
    if (m_table == geometryTable)
        return;
    m_table = geometryTable;
    emit tableChanged();
    if (!m_isValid)
        initLayer();
}

void GeoLayer::setExtent(QRectF arg)
{
    if (m_extent == arg)
        return;
    m_extent = arg;
    emit extentChanged();
    QPolygonF src(maxRect);
    QPolygonF dst(m_extent);
    src.removeLast();
    dst.removeLast();
    QTransform newTr;
    if (!QTransform::quadToQuad(src, dst, newTr)) {
        qWarning() << "Illegal geometry!" << src << dst;
        return;
    }
    if (m_transform == newTr)
        return;
    m_transform = newTr;
//    qDebug() << "Layer transform:"
//             << "\nscale: x =" << newTr.m11() << ", y =" << newTr.m22()
//             << "\ntranslate: dx =" << newTr.m31() << ", dy =" << newTr.m32()
//             << "maxRect" << maxRect << "extent" << m_extent << "mapped" << m_transform.mapRect(maxRect);
    emit transformChanged();
}

void GeoLayer::setValid(bool isValid)
{
    if (m_isValid == isValid)
        return;
    m_isValid = isValid;
    emit isValidChanged();
}

GeoView *GeoLayer::view() const
{
    return m_view;
}

void GeoLayer::setView(GeoView *view)
{
    if (m_view == view)
        return;
    m_view = view;
    emit viewChanged();
    setParentItem(view);
    initLayer();

    //    if (!m_isValid)
}

