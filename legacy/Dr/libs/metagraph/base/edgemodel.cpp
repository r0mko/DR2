#include "edgemodel.h"
#include "graphmodel.h"
#include "edge.h"
#include "node.h"
#include <QDebug>


GraphModel *EdgeModel::model() const
{
    return m_model;
}

void EdgeModel::appendEdge(Edge *e)
{
    uid2edge.insert(e->getUid(), e);
    m_edges.append(e->getUid());
}


void EdgeModel::clear()
{
    beginResetModel();
    qDebug() << "Clearing edge list";
    m_edges.clear();
    uid2edge.clear();
    endResetModel();
}

EdgeModel::EdgeModel() :
    QAbstractListModel()
{
}

EdgeModel::~EdgeModel()
{
}

QHash<int, QByteArray> EdgeModel::roleNames() const
{
    QHash<int, QByteArray> ret;
    ret[EdgeDataRole] = "edgeData";
    ret[EdgeTagRole] = "tag";
    ret[StartNodeData] = "startNode";
    ret[EndNodeData] = "endNode";
    ret[StartNodeIndex] = "startNodeIndex";
    ret[EndNodeIndex] = "endNodeIndex";
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
//    case StartNodeData: {
//        TreeNode *n = m_model->getNode(e->startNodeIndex());
//        return n->data();
//    }
//    case EndNodeData: {
//        TreeNode *n = m_model->getNode(e->endNodeIndex());
//        return n->data();
//    }
    case EdgeTagRole: {
        return e->tag();
    }
//    case StartNodeIndex:
//        return QVariant::fromValue(e->startNodeIndex());
//    case EndNodeIndex:
//        return QVariant::fromValue(e->endNodeIndex());
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
    if (index > 0 && index < m_edges.size())
        return EdgeHandle(uid2edge.value(m_edges.at(index)), m_model);
    return EdgeHandle();
}

EdgeHandle EdgeModel::getHandleByUid(uint uid) const
{
    if (!uid2edge.contains(uid))
        return EdgeHandle();
    return EdgeHandle(uid2edge.value(uid), m_model);
}

QVariant EdgeModel::edgeData(int index) const
{
    if (index > 0 && index < m_edges.size())
        return uid2edge.value(m_edges.at(index))->data();
    return QVariant();
}


QModelIndex EdgeModel::addEdge(Edge* edge)
{
    beginInsertRows(QModelIndex(), m_edges.size(), m_edges.size());
    appendEdge(edge);
    endInsertRows();
//    qDebug() << "edge added to edge model; edge model sise" << m_edges.size();
    emit itemAdded(m_edges.size() - 1, edge->getUid());
    return createIndex(m_edges.size() - 1, 0, edge);
}



void EdgeModel::onEdgeAdded(Node *firstNode, Node *endNode, Edge* edge)
{
    if( m_all ) {
        addEdge( edge );
    }
    else if( edge->hasRawTag( m_rawtag ) ) {
        addEdge( edge );
    }
}

void EdgeModel::onEdgeAboutToBeRemoved(uint uid)
{
    if (!uid2edge.contains(uid)) {
        qDebug() << "edge model knows no uid" << uid;
        return;
    }
    int r = m_edges.indexOf(uid);
//    qDebug() << "model removes edge uid" << uid << "at" << r;
    beginRemoveRows(QModelIndex(), r, r);
    m_edges.removeAt(r);
    uid2edge.remove(uid);
    endRemoveRows();

}


