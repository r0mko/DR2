#include <QElapsedTimer>
#include <QSGFlatColorMaterial>
#include <QSGNode>
#include <QtConcurrent>
#include <functional>
#include "geombuffer.h"
#include "geolayer.h"
#include "spatialitedb.h"
#include "spatialquery.h"
#include "geoview.h"
#include "triangulator.h"
#include "geostyle.h"
#include "cosmeticstroke.h"
#include "solidcolorfill.h"
int GeomBuffer::entityCount() const
{
    if (!ds)
        return 0;
    return ds->m_rows.size();
}

bool GeomBuffer::highlight() const
{
    return m_highlight;
}

void GeomBuffer::setHighlight(bool highlight)
{
    if (m_highlight == highlight)
        return;

    m_highlight = highlight;
    m_layer->update();
    emit highlightChanged(highlight);
}

bool GeomBuffer::isValid() const
{
    return m_layer && m_layer->isValid();
}

QString GeomBuffer::clause() const
{
    return m_clause;
}

void GeomBuffer::setLayer(GeoLayer *layer)
{
    if (m_layer) {
        m_layer->removeBuffer(this);
        m_layer->disconnect(0, this, 0);
    }

    m_layer = layer;
    m_layer->appendBuffer(this);
    emit layerChanged(m_layer);
    connect(m_layer, &GeoLayer::isValidChanged, this, &GeomBuffer::onLayerValidChanged);
    //    if (m_layer->isValid())
    onLayerValidChanged();
}

void GeomBuffer::setClause(QString clause)
{
    if (m_clause == clause)
        return;
    m_clause = clause;
    emit clauseChanged(clause);
}

QSGNode *GeomBuffer::updateNode(QSGNode *node)
{
//    qDebug() << "updating geometry" << m_clause << "node" << node;

//    if (node != container) {
//        qWarning() << "node and container mismatch: node =" << node << "container =" << container;
//    }

    if (!m_ready) {
        if (!node)
            node = new QSGNode;
        node->setFlag(QSGNode::OwnedByParent);
        return node;
    }

    if (m_style.isEmpty())
        applyDefaultStyle();
    QSGNode *styled = m_style.updateNode(node, *this);

    //    QSGNode *updatedStyle = m_style.updateNode(node, *this);
    //    QSGNode *styled = container->firstChild();
    //    QSGNode *updatedStyle = m_style.updateNode(styled, *this);
    //    if (styled != updatedStyle) {
    //        if (styled)
    //            container->removeChildNode(styled);
    //        container->appendChildNode(updatedStyle);
    //    }


    //    if (node->lastChild() != )
    //    node = static_cast<StyleNode*>(m_style.data())->updateNode(node, *this);

    if (styled != node) {
        qDebug() << "geometry updated node: old node" << node << m_clause;

    }
    return styled;
    //    return node;
}

QTransform GeomBuffer::combinedTransform(bool triangulator) const
{
    if (triangulator)
        return m_layer->geomBufferTransform() * m_layer->view()->viewTransform();
    return m_layer->view()->viewTransform();
}

QByteArray GeomBuffer::getCache() const
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out << m_clause << fillIndices() << lineIndices() << vertices() << ds->rows().size();
    return ba;
}

void GeomBuffer::fetchGeometry(QStringList attributeFields)
{
    if (ds->status() == SpatialQuery::Queued || ds->status() == SpatialQuery::Fetching) {
        qWarning() << "another job is active" << m_layer->table() << m_clause;
        return;
    }
    setReady(false);
    if (!isValid()) {
        setStatus(Invalid);
        return;
    }
    QStringList columns;
    QString what;
    int targetSRID = m_layer->view()->targetSRID();
    QString geomColumn = m_layer->geometryColumn();
    if (geomColumn.isEmpty()) {
        qDebug() << "null geom column" << m_layer->table() << m_layer->isValid() << m_layer->dataSource()->layerInfo(m_layer->table()).geometryColumn();
        return;
    }
    QString from = m_layer->table();
    if (m_layer->srid() != targetSRID)
        what = QString("ST_Transform(%1, %2) as %3").arg(geomColumn).arg(targetSRID).arg(geomColumn);
    else
        what = geomColumn;
    columns << what << attributeFields;
    QString q = QString("SELECT %1 FROM %2").arg(columns.join(", ")).arg(from);
    if (!m_clause.isEmpty()) {
        q.append(QString(" WHERE %1").arg(m_clause));
    }
    ds->setQuery(q);
    ds->fetchGeometry(geomColumn);
    setStatus(Queued);
}

QVariantList GeomBuffer::buildGraph() const
{
    qDebug() << "building graph" << lineIndexCount();
    QList<QPolygonF> pols = generatePolygons();
    qDebug() << "built graph" << pols.size();
    QVariantList edges;

    for (QPolygonF pol : pols) {
        edges.append(QVariant::fromValue(pol));
    }
    return edges;

}

bool GeomBuffer::loadCache(const QByteArray &blob)
{
    QDataStream in(blob);
    int count;
    QString clause;
    clear();
    in >> clause >> fillIndicesRef() >> lineIndicesRef() >> verticesRef() >> count;
    if (m_clause != clause) {
        qWarning() << "not valid cache" << m_clause << clause;
    }
    if (in.status() == QDataStream::Ok) {
        qDebug() << "loaded cache size" << blob.size() << m_clause << clause << count;
        reportProgress(0);
        reportProgress(count);
        setReady(true);
        setStatus(Ready);
        return true;
    }
    return false;
}

int GeomBuffer::completedEntityCount() const
{
    return m_completedEntityCount;
}

Style GeomBuffer::style() const
{
    return m_style;
}


void addEntity(SpatialEntity &e, QTransform tr, GeoLayer::GeometryKind kind, GeomBuffer *geom)
{
    e.parseBlob(e.wkblob);
    e.wkblob.clear();

    if (!e.entity)
        return;
    switch (kind) {
    case GeoLayer::PolygonEntity: {
        QPainterPath surface = e.entity->getPath(tr);
        VertexBuffer vb = triangulate(surface);
        geom->lock();
        geom->mergeFill(vb);
        geom->addPolyLines(surface.toSubpathPolygons());
        geom->unlock();
        break;
    }
    case GeoLayer::LinearEntity: {
        QPolygonF p = e.entity->getPolygon(tr);
        geom->lock();
        geom->addPolyLine(p);
        geom->unlock();
        break;
    }
    case GeoLayer::PointEntity: {
        QPolygonF p = e.entity->getPolygon(tr);
        geom->lock();
        geom->addPoints(p);
        geom->unlock();
        break;
    }
    default:
        break;
    }
}

void GeomBuffer::processReply()
{
    using namespace std::placeholders;
    if (m_status != Fetching) {
        qWarning() << "Status changed during fetch" << m_status;
    }
    setStatus(Processing);
    //    m_entityCount = ds->m_rows.size();
    emit entityCountChanged();
    QTransform tr = m_layer->geomBufferTransform().inverted(); // преобразование в систему координат для триангуляции
    GeoLayer::GeometryKind kind = m_layer->geometryKind();
    auto mapFunc = std::bind(&addEntity, _1, tr, kind, this);
    future = QtConcurrent::map(ds->m_rows, mapFunc);
    job.setFuture(future);
}

void GeomBuffer::reportBufferProgress(int c)
{
    m_completedEntityCount = c;
    emit completedEntityCountChanged(c);
    //    qDebug() << "buffer progress" <<  c;
}

void GeomBuffer::reportBufferReady()
{
    //    qDebug() << m_layer->table() << m_clause << "buffer ready: V#" << vertices.size() << "I#" << (fillIndices.size() | lineIndices.size());
    setReady(true);
    setStatus(Ready);

}

void GeomBuffer::reportProgress(int count)
{
    static int lastCount;
    if (count < lastCount)
        lastCount = 0;
    if (count == 0)
        setStatus(Fetching);
    m_layer->m_fetchedCount += (count - lastCount);
    //    m_entityCount = count;
    emit entityCountChanged();
    emit m_layer->progressChanged();
    lastCount = count;
}

void GeomBuffer::onLayerValidChanged()
{
    if (m_layer->isValid()) {
        if (ds) {
            if (ds->status() == SpatialQuery::Complete || ds->status() == SpatialQuery::Ready || ds->status() == SpatialQuery::Error)
                ds->deleteLater();
            else {
                ds->cancel();
                connect(ds, &SpatialQuery::complete, ds, &SpatialQuery::deleteLater);
            }
        }
        ds = new SpatialQuery(this);
        ds->setDatabase(m_layer->dataSource());
        ds->setActive(false);
        connect(ds, &SpatialQuery::complete, this, &GeomBuffer::processReply);
        connect(ds, &SpatialQuery::countChanged, this, &GeomBuffer::reportProgress);
        setStatus(Empty);
    } else {
        if (ds) {
            ds->setDatabase(nullptr);
        }
    }
    emit isValidChanged();
}



GeomBuffer::~GeomBuffer()
{
    if (vertexCount() || fillIndexCount() || lineIndexCount()) {
        qDebug() << "non-empty buffer destroyed #V" << vertexCount() << "#I" << fillIndexCount() + lineIndexCount();
        if (m_layer) {
            m_layer->removeBuffer(this);
            m_layer = nullptr;
        }
    }

}

void GeomBuffer::componentComplete()
{
    connect(&job, &QFutureWatcher<void>::finished, this, &GeomBuffer::reportBufferReady);
    connect(&job, &QFutureWatcher<void>::progressValueChanged, this, &GeomBuffer::reportBufferProgress);
    complete = true;
    setStatus(Empty);
}

GeoLayer *GeomBuffer::layer() const
{
    return m_layer;
}

void GeomBuffer::setStatus(GeomBuffer::Status status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit statusChanged(status);
}

void GeomBuffer::setReady(bool ready)
{
    if (m_ready == ready)
        return;
    m_ready = ready;
    emit readyChanged(m_ready);
}

void GeomBuffer::applyDefaultStyle()
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    //    if (highlightNode)
    //        return highlightNode;
    //    highlightNode = new QSGNode;
    QColor color = QColor::fromHsl(qrand() % 360, 255, 64, 200);

    CosmeticStroke s;
    s.setColor(color);
    s.setWidth(1);
    s.init(*this);
    if (hasFill()) {
        SolidColorFill f;
        color.setAlpha(40);
        f.setColor(color);
        f.init(*this);
        m_style.appendChild(QVariant::fromValue(f));
    }
    m_style.appendChild(QVariant::fromValue(s));
}


QSGNode *GeomBuffer::createNode()
{
    container = new QSGOpacityNode;
    return container;
}

QSGGeometryNode *GeomBuffer::getHightlightNode()
{
    if (highlightNode)
        return highlightNode;
    highlightNode = new QSGGeometryNode;
    QSGGeometry *g;
    if (hasLine())
        g = createLineGeometry();
    else if (hasFill())
        g = createOutlineGeometry();
    else {
        VertexBuffer buf;
        QPainterPath path;
        for (const QPointF &p : vertices())
            path.addEllipse(p, 200, 200);
        buf.addPolyLines(path.toSubpathPolygons());
        g = buf.createLineGeometry();
    }
    g->setLineWidth(4);
    QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
    material->setColor("#00EEFF");
    highlightNode->setGeometry(g);
    highlightNode->setMaterial(material);
    highlightNode->setOpaqueMaterial(material);
    highlightNode->setFlags(QSGNode::OwnsGeometry | QSGNode::OwnsMaterial);
    return highlightNode;
}


void GeomBuffer::setStyle(Style style)
{
    qDebug() << "Set style node" << style;
    //    if (style.canConvert<StyleNode>()) {
//    m_style.setDeleted(true);
    m_style = style;
    emit styleChanged();
    //        qDebug() << "Set style actually" << style;
    m_style.init(*this);
    m_layer->update();
    //    }
}

void GeomBuffer::reset()
{
    m_layer = nullptr;
    clear();
}


QSGGeometryNode *GeomBuffer::getPointGeometry(QColor color, qreal size)
{
    QSGGeometry *geom1 = createPointGeometry();
    QSGGeometry *geom2 = createPointGeometry();
    geom1->setLineWidth(size);
    geom2->setLineWidth(size - 4);
    QSGFlatColorMaterial *material1 = new QSGFlatColorMaterial;
    QSGFlatColorMaterial *material2 = new QSGFlatColorMaterial;
    material1->setColor(color);
    material2->setColor(Qt::white);
    QSGGeometryNode *node = new QSGGeometryNode;
    node->setGeometry(geom1);
    node->setMaterial(material1);
    node->setFlags(QSGNode::OwnsGeometry | QSGNode::OwnsMaterial | QSGNode::OwnedByParent);
    //    memcpy(geom2->vertexData(), geom1->vertexData(), geom1->vertexCount() * geom1->sizeOfVertex());
    //    geom2->vertexDataAsPoint2D()
    QSGGeometryNode *node2 = new QSGGeometryNode;
    node2->setMaterial(material2);
    node2->setGeometry(geom2);
    node->appendChildNode(node2);
    return node;
}

QSGGeometryNode *GeomBuffer::getLinearGeometry(QColor color, qreal width)
{
    QSGGeometry *geometry = createLineGeometry();
    //    if (!geometry)
    //        return nullptr;
    geometry->setLineWidth(width);
    QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
    material->setColor(color);
    QSGGeometryNode *node = new QSGGeometryNode;
    node->setGeometry(geometry);
    node->setMaterial(material);
    node->setFlags(QSGNode::OwnsGeometry | QSGNode::OwnsMaterial | QSGNode::OwnedByParent);
    return node;
}

QSGGeometryNode *GeomBuffer::getFillGeometry(QColor color)
{
    QSGGeometry *geometry = createFillGeometry();
    QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
    material->setColor(color);
    QSGGeometryNode *node = new QSGGeometryNode;
    node->setGeometry(geometry);
    node->setMaterial(material);
    node->setFlags(QSGNode::OwnsGeometry | QSGNode::OwnsMaterial | QSGNode::OwnedByParent);
    return node;
}

