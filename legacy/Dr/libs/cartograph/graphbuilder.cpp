#include "graphbuilder.h"
#include "wkt.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>



GraphBuilder::GraphBuilder(QObject *parent) :
    QObject(parent),
    m_nodes(new NodeModel(this)),
    m_ribs(new RibModel(this))
{}

NodeModel *GraphBuilder::nodes() const
{
    return m_nodes;
}

RibModel *GraphBuilder::ribs() const
{
    return m_ribs;
}

QList<int> GraphBuilder::getNeighborNodes(int nodeId) const
{
    QList<int> ret;
    const GraphNode &node = m_nodes->m_nodes.at(nodeId);
    for (int outRib:node.outgoingRibs()) {
        const GraphRib &rib = m_ribs->m_ribs.at(outRib);
        if (rib.startNode() == nodeId)
            ret.append(rib.endNode());
        else
            ret.append(rib.startNode());
    }
    return ret;
}

QList<int> GraphBuilder::getReverseNeighborNodes(int nodeId) const
{
    QList<int> ret;
    const GraphNode &node = m_nodes->m_nodes.at(nodeId);
    for (int outRib:node.incomingRibs()) {
        const GraphRib &rib = m_ribs->m_ribs.at(outRib);
        if (rib.startNode() == nodeId)
            ret.append(rib.endNode());
        else
            ret.append(rib.startNode());
    }
    return ret;
}

QList<QPair<int, qreal>> GraphBuilder::getForwardNodes(int nodeId) const
{
    QList<QPair<int, qreal>> ret;
    const GraphNode &node = m_nodes->m_nodes.at(nodeId);
    for (int outRib:node.outgoingRibs()) {
        const GraphRib &rib = m_ribs->m_ribs.at(outRib);
        if (rib.startNode() == nodeId)
            ret.append(qMakePair(rib.endNode(), rib.length()));
        else
            ret.append(qMakePair(rib.startNode(), rib.length()));
    }
    return ret;
}

QList<QPair<int, qreal>> GraphBuilder::getReverseNodes(int nodeId) const
{
    QList<QPair<int, qreal>> ret;
    const GraphNode &node = m_nodes->m_nodes.at(nodeId);
    for (int outRib:node.incomingRibs()) {
        const GraphRib &rib = m_ribs->m_ribs.at(outRib);
        if (rib.startNode() == nodeId)
            ret.append(qMakePair(rib.endNode(), rib.length()));
        else
            ret.append(qMakePair(rib.startNode(), rib.length()));
    }
    return ret;
}



void GraphBuilder::addRib(QVariant geometryBlob, uint id, qreal length, bool oneway, qreal zIdx, QString name, MapView *view)
{
    QScopedPointer<Geometry> geom(new Geometry);
    QByteArray ba = geometryBlob.toByteArray();
    if (ba.isEmpty())
        return;
    geom->parseBlob(ba);
    QPolygonF lineString;
    QTransform tr = view ? view->transform() : QTransform();
    for (GeoEntity *e : geom->data()) {
        QPainterPath path = e->getPath(tr);
        QList<QPolygonF> pols = path.toSubpathPolygons();
        if (!lineString.isEmpty())
            break;
        lineString = pols.first(); // skipping the rest, if any
    }
    appendRib(lineString, id, length, oneway, zIdx, name);
}

void GraphBuilder::addRibFromPoints(QVariantList pol, uint id, qreal length, bool oneway, qreal z, QString name, MapView *view)
{
    QPolygonF lineString;
    for (QVariant v:pol) {
        QVariantList pt = v.toList();
        if (pt.size() == 2) {
            QPointF point(pt.first().toDouble(), pt.last().toDouble());
            lineString.append(point);
        }
    }
    appendRib(lineString, id, length, oneway, z, name);
}

void GraphBuilder::setNodes(NodeModel *arg)
{
    if (m_nodes == arg)
        return;

    m_nodes = arg;
    emit nodesChanged(arg);
}

void GraphBuilder::setRibs(RibModel *arg)
{
    if (m_ribs == arg)
        return;

    m_ribs = arg;
    emit ribsChanged(arg);
}

void GraphBuilder::appendRib(const QPolygonF &lineString, uint id, qreal length, bool oneway, qreal z, QString name)
{
    int n1idx = m_nodes->addNode(lineString.first());
    int n2idx = m_nodes->addNode(lineString.last());
    int ribId = m_ribs->addRib(n1idx, n2idx, lineString, length, oneway, z, name);
//    qDebug() << QString("Adding rib %1 l=%2 [%3:%4] name ='%5'").arg(id).arg(length).arg(n1idx).arg(n2idx).arg(name);

    m_nodes->node(n1idx)->appendOutgoingRib(ribId);
    m_nodes->node(n2idx)->appendIncomingRib(ribId);
    if (!oneway) {
        m_nodes->node(n2idx)->appendOutgoingRib(ribId);
        m_nodes->node(n1idx)->appendIncomingRib(ribId);
    }
}


QHash<int, QByteArray> NodeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IncomingRibsRole] = "incoming";
    roles[OutgoingRibsRole] = "outgoing";
    roles[XRole] = "xPos";
    roles[YRole] = "yPos";
    roles[PointRole] = "pos";
    return roles;
}

int NodeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_nodes.size();
}

GraphNode *NodeModel::node(int index)
{
    if (index < 0 || index >= m_nodes.size())
        return nullptr;
    return &m_nodes[index];
}

QVariant NodeModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_nodes.size())
        return QVariant();
    const GraphNode &node = m_nodes[index.row()];
    switch (role) {
    case IncomingRibsRole:
        return QVariant::fromValue<QList<int>>(node.incomingRibs());
    case OutgoingRibsRole:
        return QVariant::fromValue<QList<int>>(node.outgoingRibs());
    case XRole:
        return node.point().x();
    case YRole:
        return node.point().y();
    case PointRole:
        return node.point();
    default:
        break;
    }
    return QVariant();
}

int NodeModel::addNode(const QPointF &pt)
{
    int idx = buffer.addVertex(pt);
    if (idx == m_nodes.size()) { // add a new node
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        GraphNode n;
        n.setPoint(pt);
        m_nodes.append(n);
        emit countChanged();
        endInsertRows();
    } else if (idx > m_nodes.size()) {
        qWarning() << "Node model totally wrecked";
        qApp->exit(1);
    }
    return idx;
}

QVariantMap NodeModel::nodeAt(int index) const
{
    QVariantMap ret;
    if (index < 0 || index >= m_nodes.size())
        return ret;
    const GraphNode &node = m_nodes.at(index);
    ret["incoming"] = QVariant::fromValue<QList<int>>(node.incomingRibs());
    ret["outgoing"] = QVariant::fromValue<QList<int>>(node.outgoingRibs());
    ret["xPos"] = node.point().x();
    ret["yPos"] = node.point().y();
    ret["pos"] = node.point();
    return ret;
}

int NodeModel::count() const
{
    return m_nodes.size();
}

void NodeModel::dumpNodes(QUrl filename)
{
    QFile f(filename.toLocalFile());
    QPolygonF pol;
    for (const GraphNode &n:m_nodes) {
        pol.append(n.point());
    }
    if (f.open(QIODevice::WriteOnly)) {
        QDataStream out(&f);
        out << pol;
    }
}

void NodeModel::loadDump(QUrl filename)
{
    QFile f(filename.toLocalFile());
    QPolygonF pol;
    for (const GraphNode &n:m_nodes) {
        pol.append(n.point());
    }
    if (f.open(QIODevice::ReadOnly)) {
        QDataStream in(&f);
        in >> pol;
    }
    beginInsertRows(QModelIndex(), 0, pol.size());
    for (QPointF p:pol) {
        GraphNode node;
        node.setPoint(p);
        m_nodes.append(node);

    }
    endInsertRows();
}


QList<int> GraphNode::incomingRibs() const
{
    return m_incomingRibs;
}

void GraphNode::setIncomingRibs(const QList<int> &incomingRibs)
{
    m_incomingRibs = incomingRibs;
}

QList<int> GraphNode::outgoingRibs() const
{
    return m_outgoingRibs;
}

void GraphNode::appendIncomingRib(int ribId)
{
    m_incomingRibs.append(ribId);
}

void GraphNode::appendOutgoingRib(int ribId)
{
    m_outgoingRibs.append(ribId);
}

void GraphNode::setPoint(const QPointF &point)
{
    m_point = point;
}

void GraphNode::setOutgoingRibs(const QList<int> &outgoingRibs)
{
    m_outgoingRibs = outgoingRibs;
}
QPointF GraphNode::point() const
{
    return m_point;
}


QHash<int, QByteArray> RibModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[StartNodeRole] = "startNode";
    roles[EndNodeRole] = "endNode";
    roles[OnewayRole] = "oneway";
    roles[NameRole] = "name";
    roles[LengthRole] = "length";
    roles[PolygonLengthRole] = "polygonLength";
    roles[PolygonRole] = "polygonData";
    roles[ZRole] = "z";
    return roles;
}

int RibModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_ribs.size();
}

QVariant RibModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_ribs.size())
        return QVariant();
    const GraphRib &rib = m_ribs[index.row()];
    switch (role) {
    case StartNodeRole:
        return rib.startNode();
    case EndNodeRole:
        return rib.endNode();
    case OnewayRole:
        return rib.oneway();
    case NameRole:
        return rib.name();
    case LengthRole:
        return rib.length();
    case ZRole:
        return rib.z();
    case PolygonLengthRole:
        return rib.polygonLength();
    case PolygonRole:
        return rib.polygon();
    default:
        break;
    }
    return QVariant();
}

int RibModel::addRib(int nodeFrom, int nodeTo, const QPolygonF &polygon, qreal length, bool oneway, qreal z, QString name)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    GraphRib r;
    int ribId = m_ribs.size();
    r.setStartNode(nodeFrom);
    r.setEndNode(nodeTo);
    r.setLength(length);
    r.setOneway(oneway);
    r.setZ(z);
    r.setPolygon(polygon);
    r.setName(name);
    m_ribs.append(r);
    endInsertRows();
    emit countChanged();
    return ribId;
}

QVariantMap RibModel::ribAt(int index) const
{
    QVariantMap ret;
    if (index < 0 || index >= m_ribs.size())
        return ret;
    const GraphRib &rib = m_ribs.at(index);
    ret["startNode"] = rib.startNode();
    ret["endNode"] = rib.endNode();
    ret["oneway"] = rib.oneway();
    ret["name"] = rib.name();
    ret["length"] = rib.length();
    ret["polygonLength"] = rib.polygonLength();
    ret["polygonData"] = rib.polygon();
    ret["z"] = rib.z();
    return ret;
}

int RibModel::count() const
{
    return m_ribs.size();
}



