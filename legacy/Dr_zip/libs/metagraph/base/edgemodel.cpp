#include "edgemodel.h"
#include "graphmodel.h"
#include "edge.h"
#include "node.h"
#include "hashstring.h"
#include <QDebug>
#include <QThread>

GraphModel *EdgeModel::model() const
{
    return m_model;
}

void EdgeModel::setModel(GraphModel *model)
{
    if (m_model == model)
        return;

    if( m_model != nullptr ) {
        disconnect(m_model, 0, this, 0);
        clear();
    }

    m_model = model;
    connect( m_model, &GraphModel::modelAboutToBeReset, this, &EdgeModel::hold);
    connect( m_model, &GraphModel::modelReset, this, &EdgeModel::rescan);
    connect( m_model, &GraphModel::edgeAdded, this, &EdgeModel::onEdgeAdded);
    connect( m_model, &GraphModel::edgeTagsChanged, this, &EdgeModel::onEdgeTagsChanged);
    connect( m_model, &GraphModel::edgeDataChanged, this, &EdgeModel::onEdgeDataChanged);
    connect( m_model, &GraphModel::edgeAboutToBeRemoved, this, &EdgeModel::onEdgeAboutToBeRemoved );
    connect( m_model, &GraphModel::rowsAboutToBeMoved, this, &EdgeModel::onRowsAboutToBeMoved );
    connect( m_model, &GraphModel::rowsMoved, this, &EdgeModel::onRowsMoved );
    connect( m_model, &GraphModel::rowsAboutToBeRemoved, this, &EdgeModel::onRowsAboutToBeRemoved);
    connect( m_model, &GraphModel::rowsInserted, this, &EdgeModel::onRowsInserted);

    emit modelChanged();
    initBinding();

}

int EdgeModel::count() const
{
    return m_edges.size();
}

QModelIndex EdgeModel::rootIndex() const
{
    return m_rootIndex;
}

void EdgeModel::setRootIndex(QModelIndex rootIndex)
{
    if (m_rootIndex == rootIndex)
        return;

    if (bound) {
        bound = false;
        emit linkedChanged(bound);
    }
    m_rootIndex = rootIndex;
    initBinding();

//    if (m_mrl.isEmpty()) {
//        m_mrl = m_model->getMRL(m_rootIndex);
//        emit mrlChanged(m_mrl);
//    }
    emit rootIndexChanged();


}

EdgeModel::DirectionFlags EdgeModel::direction() const
{
    return m_direction;
}

void EdgeModel::setDirection(DirectionFlags direction)
{
    if (m_direction == direction)
        return;

    m_direction = direction;
    emit directionChanged();
    if (complete)
        rescan();
}

QString EdgeModel::mrl() const
{
    return m_mrl;
}

void EdgeModel::setMrl(QString mrl)
{
    if (m_mrl == mrl)
        return;

    m_mrl = mrl;

    emit mrlChanged(mrl);
    if (bound) {
        bound = false;
        emit linkedChanged(bound);
    }
    if (m_model) {
        initBinding();
    }
}

void EdgeModel::appendEdge(Edge *e)
{
    //    qDebug() << "Appending edge" << e->tag() << m_edges.size()
    //             << "\n complete" << complete
    //             << "\n this thread" << this->thread()
    //             << "\n model thread" << m_model->thread()
    //             << "\n current thread" << QThread::currentThread()
    //                ;
    uid2edge.insert(e->getUid(), e);
    m_edges.append(e->getUid());
}


void EdgeModel::clear()
{
    //    beginResetModel();
    //    qDebug() << "Clearing edge list";
    m_edges.clear();
    uid2edge.clear();
    //    endResetModel();
}

EdgeModel::EdgeModel(QObject *parent) : QAbstractListModel(parent)
{
}

EdgeModel::~EdgeModel()
{
}

void EdgeModel::componentComplete()
{
    complete = true;
    if (m_model)
        rescan();
}

QHash<int, QByteArray> EdgeModel::roleNames() const
{
    QHash<int, QByteArray> ret;
    ret[EdgeDataRole] = "edgeData";
    ret[EdgeTagRole] = "tag";
    ret[EdgeUidRole] = "uid";
    ret[StartNodeRole] = "startNode";
    ret[EndNodeRole] = "endNode";
    return ret;
}

//void EdgeModel::removeEdge(Edge *edge)
//{
//    m_edges.erase(std::remove_if(m_edges.begin(),
//                                   m_edges.end(),
//                                   [edge](QWeakPointer<Edge> e) { return edge == e.data(); } ),
//                    m_edges.end() );
//}

//void EdgeModel::removeEdge(QPersistentModelIndex ind)
//{
//    removeRow(ind.row());
//}

int EdgeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_edges.size();
    return 0;
}

QModelIndex EdgeModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex idx = QAbstractListModel::index(row, column, parent);
    if (idx.internalPointer()) {
        return idx;
    }
    //    qDebug() << "Edgemodel index request" << row << column << parent;
    return createIndex(row, 0, m_edges.at(row));
}

QVariant EdgeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int r = index.row();
    if (r < 0 || r >= m_edges.size())
        return QVariant();
    Edge *e = uid2edge.value(m_edges.at( index.row() ));
    //    qDebug() << "is valid edge" << (index.internalPointer() == e);
    switch (role) {
    case StartNodeRole:
        return m_model->getHandle(e->startNode());
    case EndNodeRole:
        return m_model->getHandle(e->endNode());
    case EdgeTagRole:
        return e->tag();
    case EdgeUidRole:
        return e->getUid();
    case EdgeDataRole:
        return e->data();
    default:
        break;
    }
    return QVariant();
}

EdgeHandle EdgeModel::getHandle(int index) const
{
    Q_ASSERT(m_model != nullptr);
    if (index >= 0 && index < m_edges.size())
        return EdgeHandle(uid2edge.value(m_edges.at(index)), m_model);
    return EdgeHandle();
}

EdgeHandle EdgeModel::getHandleByUid(uint uid) const
{
    if (!uid2edge.contains(uid))
        return EdgeHandle();
    return EdgeHandle(uid2edge.value(uid), m_model);
}

int EdgeModel::getIndexByUid(uint uid) const
{
//    qDebug() << Q_FUNC_INFO << uid << m_edges;
    return m_edges.indexOf(uid);
}

QVariant EdgeModel::edgeData(int index) const
{
    if (index >= 0 && index < m_edges.size())
        return uid2edge.value(m_edges.at(index))->data();
    return QVariant();
}

QString EdgeModel::tag() const
{
    return m_tag;
}

void EdgeModel::setTag(QString tag)
{
    if (m_tag == tag)
        return;
    m_tag = tag;
    emit tagChanged(m_tag);
    rescan();
}


QModelIndex EdgeModel::addEdge(Edge* edge)
{
    //    qDebug() << "c++ edge tags" << edge->tags();
    //    qDebug() << "Adding edge" << edge->tags() << edge->getUid();
    beginInsertRows(QModelIndex(), m_edges.size(), m_edges.size());
    appendEdge(edge);
    endInsertRows();
    emit itemAdded(m_edges.size() - 1, edge->getUid());
    emit countChanged();
    return createIndex(m_edges.size() - 1, 0, edge);
}

bool EdgeModel::filterEdge(Edge *edge) const
{
    //    qDebug() << this->thread() << QThread::currentThread() << m_model->thread();
    //    qDebug() << "filter edge" << m_model << m_rootIndex << edge->tags() << tag() << edge->rawTags() << m_rawtag << m_direction << edge->hasRawTag(m_rawtag);
    Node *firstNode = edge->startNode();
    Node *endNode = edge->endNode();
    Node *n = m_model->getNode(m_rootIndex);

    if (m_tag.hashValue() != 0 && !edge->hasRawTag(m_tag))
        return false;

    int f = 0;

    if (firstNode->isChildOf(n))
        f |= 0x1;

    if (endNode->isChildOf(n))
        f |= 0x2;

    //    qDebug() << __func__ << m_model->getNodeId(n) << m_model->getNodeId(firstNode) << m_model->getNodeId(endNode) << (DirectionFlags)f;
    if( m_direction == Any )
        return f & 0x3;

    return f == (int)m_direction;
}

void EdgeModel::hold()
{
    if (!bound)
        return;
    bound = false;
    m_rootIndex = QModelIndex();
    emit rootIndexChanged();
    emit linkedChanged(bound);

}

void EdgeModel::rescan()
{
    if (!complete)
        return;
    if (!bound) {
        initBinding();
        return;
    }
//    if (!m_rootIndex.isValid()) {
//        qWarning() << Q_FUNC_INFO << "root index invalid!";
//        return;
//    }
    QSet<Edge *> edges = m_model->dumpEdges(m_rootIndex);
    if (edges.size() == m_edges.size()) {
        qDebug() << __func__ << "sizes are same:" << m_edges.size() << "no need to rescan";
        return;
    }
    int lastSize = m_edges.size();
    beginResetModel();
    m_edges.clear();
    uid2edge.clear();
    for (Edge *e : edges) {
        if (filterEdge(e))
            appendEdge(e);
    }
//    qDebug() << __func__ << "old size" << lastSize << "new size" << m_edges.size() << m_model->getMRL(m_rootIndex);
    endResetModel();
}

void EdgeModel::initBinding()
{
    if (bound)
        return;
    if (m_rootIndex.isValid()) {
        if (!m_model) {
//            qDebug() << "no model, trying to extract one from provided QModelIndex";
            GraphModel *gm = const_cast<GraphModel*>(static_cast<const GraphModel*>(m_rootIndex.model()));
            setModel(gm);
            return;
        }
//        Q_ASSERT(m_model != nullptr);
        bound = true;
        emit linkedChanged(bound);
        if (m_mrl.isEmpty()) {
            m_mrl = m_model->getMRL(m_rootIndex);
            emit mrlChanged(m_mrl);
        }
        rescan();
        return;
    }
    if (m_model != nullptr && !m_mrl.isEmpty()) {
        Node *rootNode = m_model->getNode(m_mrl);
        if (!rootNode) {
            qDebug() << "unknown mrl" << m_mrl;
            return;
        }
        QModelIndex idx = m_model->getNodeIndex(rootNode);
        setRootIndex(idx);
        bound = true;
        emit linkedChanged(bound);
    }
}

void EdgeModel::onEdgeTagsChanged(Edge* edge)
{
    uint uid = edge->getUid();
    if (uid2edge.contains(edge->getUid())) {
        QModelIndex idx = createIndex(getIndexByUid(uid), 0, edge);
        emit dataChanged(idx, idx, { EdgeTagRole });
    }
}

void EdgeModel::onEdgeDataChanged(Edge* edge)
{
    uint uid = edge->getUid();
    if (uid2edge.contains(edge->getUid())) {
        QModelIndex idx = createIndex(getIndexByUid(uid), 0, edge);
        emit dataChanged(idx, idx, { EdgeDataRole });
    }
}

void EdgeModel::onEdgeAdded(Node *, Node *, Edge* edge)
{
    if (filterEdge(edge))
        addEdge( edge );
}

void EdgeModel::onEdgeAboutToBeRemoved(uint uid)
{
    if (!uid2edge.contains(uid)) {
        //        qDebug() << "edge model knows no uid" << uid;
        return;
    }
    int r = m_edges.indexOf(uid);
    qDebug() << "model removes edge uid" << uid << "at" << r;
    emit edgeRemoved(r, uid);
    beginRemoveRows(QModelIndex(), r, r);
    m_edges.removeAt(r);
    uid2edge.remove(uid);
    endRemoveRows();

}

void EdgeModel::onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    qDebug() << "Model rows moved from" << m_model->getMRL(sourceParent) << "to" << m_model->getMRL(destinationParent);
    Node *root = m_model->getNode(m_rootIndex);
    if (!root)
        return;

    Node *destParent = m_model->getNode(destinationParent);
    Node *srcParent = m_model->getNode(sourceParent);

    if (srcParent == root || srcParent->isChildOf(root)) {
        if (destParent == root || destParent->isChildOf(root))
            return;
        return;
    }
    if (destParent == root || destParent->isChildOf(root)) {
        for (int i = sourceStart; i <= sourceEnd; ++i) {
            const Node *c = srcParent->childNodes().at(i);
            for (Edge *e : c->findEdges()) {
                uint uid = e->getUid();
                if (!uid2edge.contains(uid)) {
                    addEdge(e);
                }
            }
        }
    }

}

void EdgeModel::onRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    qDebug() << "Model rows about to move from" << m_model->getMRL(sourceParent) << "to" << m_model->getMRL(destinationParent);
    Node *root = m_model->getNode(m_rootIndex);
    if (!root)
        return;

    Node *destParent = m_model->getNode(destinationParent);
    Node *srcParent = m_model->getNode(sourceParent);

    if (destParent == root || destParent->isChildOf(root)) {
        if (srcParent == root || srcParent->isChildOf(root))
            return;
        return;
    }
    if (srcParent == root || srcParent->isChildOf(root)) {
        for (int i = sourceStart; i <= sourceEnd; ++i) {
            const Node *c = srcParent->childNodes().at(i);
            for (Edge *e : c->findEdges()) {
                uint uid = e->getUid();
                if (uid2edge.contains(uid)) {
                    int r = m_edges.indexOf(e->getUid());
                    emit edgeRemoved(r, uid);
                    beginRemoveRows(QModelIndex(), r, r);
                    m_edges.removeAt(r);
                    uid2edge.remove(uid);
                    endRemoveRows();
                }
            }
        }
    }

}

void EdgeModel::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    Node *root = m_model->getNode(m_rootIndex);
    if (!root)
        return;
    Node *pNode = m_model->getNode(parent);
    if (pNode == root || pNode->isChildOf(root)) {
        for (int i = first; i <= last; ++i) {
            const Node *c = pNode->childNodes().at(i);
            for (Edge *e : c->findEdges()) {
                if (filterEdge(e)) {
                    uint uid = e->getUid();
                    if (!uid2edge.contains(uid))
                        addEdge(e);
                }
            }
        }
    }
}

void EdgeModel::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    Node *root = m_model->getNode(m_rootIndex);
    if (!root)
        return;
    Node *pNode = m_model->getNode(parent);
    if (pNode == root || pNode->isChildOf(root)) {
        for (int i = first; i <= last; ++i) {
            const Node *c = pNode->childNodes().at(i);
            for (Edge *e : c->findEdges()) {
                uint uid = e->getUid();
                if (uid2edge.contains(uid)) {
                    int r = m_edges.indexOf(e->getUid());
                    emit edgeRemoved(r, uid);
                    beginRemoveRows(QModelIndex(), r, r);
                    m_edges.removeAt(r);
                    uid2edge.remove(uid);
                    endRemoveRows();
                }
            }
        }
    }
}


