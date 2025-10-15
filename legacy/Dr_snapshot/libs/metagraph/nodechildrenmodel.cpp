#include "nodechildrenmodel.h"
#include "graphmodel.h"
#include <QDebug>
#include <QQmlEngine>
#include "qqml.h"


NodeChildrenModel::NodeChildrenModel(QObject *parent) :
    QAbstractListModel(parent),
    sentinel(false),
    m_linked(false)
{}

NodeChildrenModel::~NodeChildrenModel()
{
//    qDebug() << "submodel destroyed";
}

void NodeChildrenModel::componentComplete()
{
    if (m_model) {
        if (!m_rootIndex.isValid() && !m_mrl.isEmpty()) {
            setLinked(false);
            resolveMrl();
        }
    }
}

QModelIndex NodeChildrenModel::index(int row, int, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();
    return createIndex(row, 0, m_rootIndex.child(row, 0).internalPointer());
}

QModelIndex NodeChildrenModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

QHash<int, QByteArray> NodeChildrenModel::roleNames() const
{
//    qDebug() << "children model roles request";
    if (m_rootIndex.isValid())
        return m_rootIndex.model()->roleNames();
    return QHash<int, QByteArray>();
}

int NodeChildrenModel::rowCount(const QModelIndex &parent) const
{
    if (m_rootIndex.isValid() && !parent.isValid())
        return m_rootIndex.model()->rowCount(m_rootIndex);
    return 0;
}

int NodeChildrenModel::columnCount(const QModelIndex &parent) const
{
    if (m_rootIndex.isValid() && !parent.isValid())
        return 1;
    return 0;
}

int NodeChildrenModel::count() const
{
    if (m_rootIndex.isValid())
        return m_rootIndex.model()->rowCount(m_rootIndex);
    return 0;
}

QModelIndex NodeChildrenModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return m_rootIndex.model()->index(proxyIndex.row(), 0, m_rootIndex);
}

QModelIndex NodeChildrenModel::mapToSource(int rowIndex) const
{
    return m_rootIndex.model()->index(rowIndex, 0, m_rootIndex);
}

QModelIndex NodeChildrenModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (sourceIndex.parent() != m_rootIndex)
        return QModelIndex();
    return index(sourceIndex.row(), 0, QModelIndex());
}

GraphModel *NodeChildrenModel::sourceModel() const
{
//    qDebug() << (m_model == nullptr);
    return m_model;
}

QVariant NodeChildrenModel::data(const QModelIndex &index, int role) const
{
    if(role < lastSrcRole) {
        QVariant ret = mapToSource(index).data(role);
//        qDebug() << "NodeChildrenModel data" << ret;
        return ret;
    }

    qDebug() << "Unknown role" << role;
    return QVariant();
}

QJSValue NodeChildrenModel::rootHandle() const
{
    QQmlEngine *e = qmlEngine(this);
    if(!e)
        return QJSValue();
    QJSValue ret = e->toScriptValue(m_model->getHandle(m_rootIndex));
    return ret;
}

GraphModel *NodeChildrenModel::model() const
{
    return m_model;
}

bool NodeChildrenModel::isLinked() const
{
    return m_rootIndex.isValid();
}

QString NodeChildrenModel::mrl() const
{
    return m_mrl;
}

void NodeChildrenModel::setHandle(NodeHandle node)
{
    if (m_rootIndex == node.modelIndex())
        return;
    setRootIndex(node.modelIndex());
}

void NodeChildrenModel::setModel(GraphModel *model)
{
    if (m_model == model)
        return;
//    del();
    m_model = model;
//    endResetModel();
    resetLink();
    resolveMrl();
    emit modelChanged();
}

void NodeChildrenModel::setMrl(QString mrl)
{
    if (m_mrl == mrl)
        return;
    m_mrl = mrl;
    if (!m_model)
        return;
    resetLink();
    resolveMrl();
}

ModelAgent *NodeChildrenModel::get(int index)
{
    if (!m_model)
        return nullptr;
    if (index < 0)
        return nullptr;
    return m_model->get(m_model->index(index, 0, m_rootIndex));
}

QVariant NodeChildrenModel::getHandle(int index)
{
    if (!m_model)
        return QVariant();
    return m_model->getHandle(m_model->index(index, 0, m_rootIndex));
}

QVariant NodeChildrenModel::getHandle(const QString &mrl) const
{
    if (mrl.isEmpty())
        return QVariant();
    const Node *parent = m_model->getNode(m_rootIndex);
//    qDebug() << "get handle for mrl" << mrl << m_model->getMRL(m_rootIndex);

    if (!m_model->node2nid.contains(parent)) {
        qDebug() << "node" << parent << "has no id";
        return QVariant();
    }
    NId id = m_model->node2nid[parent];
//    qDebug() << "Nid" << id.id << id.children.keys();
    return m_model->getHandle(id.children.value(mrl));
}

bool NodeChildrenModel::hasChild(QString id) const
{
    const Node *parent = m_model->getNode(m_rootIndex);
    if (!parent)
        return false;
    NId nid = m_model->node2nid.value(parent);
    return nid.children.contains(id);
}

void NodeChildrenModel::remove(int index)
{
    if (!m_model)
        return;
    m_model->removeNode(m_model->index(index, 0, m_rootIndex));
}

void NodeChildrenModel::remove(QList<int> list)
{
    if (!m_model)
        return;
    if (list.isEmpty())
        return;
//    qDebug() << "Removing indexes" << list;
    QItemSelection sel;
    int range_begin = list.first();
    int range_end = list.first();
    for (int i : list) {
        int delta = i - range_end;
        if (delta > 1) {
            qDebug() << "detected range" << range_begin << range_end;
            sel.append(QItemSelectionRange(mapToSource(range_begin), mapToSource(range_end)));
            range_begin = i;
            range_end = i;
        } else if (delta == 1) {
            qDebug() << "expanding range" << range_begin << i;
            range_end = i;
        }
    }
    qDebug() << "last range" << range_begin << range_end;
    sel.append(QItemSelectionRange(mapToSource(range_begin), mapToSource(range_end)));
    model()->removeNodes(sel);

}

void NodeChildrenModel::setRootIndex(QModelIndex rootIndex)
{
    if (m_rootIndex == rootIndex)
        return;
//    qDebug() << "set root index" << rootIndex << "current is" << m_rootIndex << "model" << m_model;
    if (!m_model) {
        m_model = const_cast<GraphModel*>(qobject_cast<const GraphModel*>(rootIndex.model()));
        emit modelChanged();
    } else {
        m_model->disconnect(0, this, 0);
//        m_model->proxies.remove(m_model->getNode(m_rootIndex));
    }
//    beginResetModel();
    m_rootIndex = rootIndex;
//    endResetModel();
    initLink();

    emit rootIndexChanged();
    emit rootHandleChanged();
    emit mrlChanged();
    emit countChanged();
    setLinked(true);
}

void NodeChildrenModel::onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (topLeft.parent() != m_rootIndex && bottomRight != m_rootIndex)
        return;
    qDebug() << "Source data changed" << topLeft.row() << bottomRight.row() << isParentOf(topLeft, m_rootIndex);
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
}

void NodeChildrenModel::onSourceRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    if (parent != m_rootIndex)
        return;
    beginInsertRows(QModelIndex(), first, last);
}

void NodeChildrenModel::onSourceRowsInserted(const QModelIndex &parent, int first, int last)
{
    if (parent != m_rootIndex)
        return;
    endInsertRows();
    emit countChanged();
    for (int i = first; i <= last; ++i) {
        emit nodeAdded(i, getHandle(i));
    }
}

void NodeChildrenModel::onSourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    if (parent != m_rootIndex) {
        if (m_rootIndex.parent() == parent) {
            if (m_rootIndex.row() >= first && m_rootIndex.row() <= last) {
//                qWarning() << "Destroying rootIndex of" << this;
                m_rootIndex.model()->disconnect(0, this, 0);
                deleteLater();
            }
        }
        return;
    }
    beginRemoveRows(QModelIndex(), first, last);
//    for (int i = first; i <= last; ++i) {
//        emit nodeRemoved(i, getHandle(i - 1));
//    }
}

void NodeChildrenModel::onSourceRowsRemoved(const QModelIndex &parent, int, int)
{
    if (parent != m_rootIndex)
        return;
    endRemoveRows();
}

void NodeChildrenModel::onSourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    if (sourceParent == m_rootIndex && destinationParent == m_rootIndex) {
        beginMoveRows(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationRow);
        return;
    }
    if ( destinationParent == m_rootIndex) {
        beginInsertRows(QModelIndex(), destinationRow, destinationRow + (sourceEnd - sourceStart));
        return;
    }
    beginRemoveRows(QModelIndex(), sourceStart, sourceEnd);
}

void NodeChildrenModel::onSourceRowsMoved(const QModelIndex &sourceParent, int, int, const QModelIndex &destinationParent, int)
{
    if (sourceParent == m_rootIndex && destinationParent == m_rootIndex) {
        endMoveRows();
        return;
    }
    if ( destinationParent == m_rootIndex) {
        endInsertRows();
        return;
    }
    endRemoveRows();
}

void NodeChildrenModel::onOutEdgeAdded(const QModelIndex &index, EdgeHandle edgeHandle)
{
    Node *n = m_model->getNode(index);
    if (!n)
        return;
    Node *root = m_model->getNode(m_rootIndex);
    if (n->isChildOf(root, true))
        emit outEdgeAdded(index.row(), edgeHandle);
}

void NodeChildrenModel::onInEdgeAdded(const QModelIndex &index, EdgeHandle edgeHandle)
{
    Node *n = m_model->getNode(index);
    if (!n)
        return;
    Node *root = m_model->getNode(m_rootIndex);
    if (n->isChildOf(root, true))
        emit inEdgeAdded(index.row(), edgeHandle);
}


void NodeChildrenModel::onSourceModelReset()
{

//    qDebug() << "source model was reset";
    resolveMrl();
}

void NodeChildrenModel::onSourceModelAboutToBeReset()
{
//    qDebug() << "source model about to be reset!";
//    resetLink();
    resetLink();
}

void NodeChildrenModel::resolveMrl()
{
    NodeHandle h = m_model->getHandle(m_mrl).value<NodeHandle>();
    if (!h.isValid()) {
        if (sentinel)
            return;
//        qDebug() << "node" << m_mrl << "not found: initializing sentinel";
        m_model->disconnect(0, this, 0);
        connect(m_model, &QAbstractItemModel::modelReset, this, &NodeChildrenModel::onSourceModelReset);
        connect(m_model, &QAbstractItemModel::dataChanged, this, &NodeChildrenModel::resolveMrl);
        connect(m_model, &QAbstractItemModel::rowsInserted, this, &NodeChildrenModel::resolveMrl);
        sentinel = true;
        setLinked(false);
    } else {
        m_model->disconnect(0, this, 0);
        sentinel = false;
        setHandle(h);
    }
}

void NodeChildrenModel::resetLink()
{
    beginResetModel();
    setLinked(false);
    m_rootIndex = QModelIndex();
    m_model->disconnect(0, this, 0);
    connect(m_model, &QAbstractItemModel::modelReset, this, &NodeChildrenModel::onSourceModelReset);
//    if (!m_mrl.isEmpty()) {
    lastSrcRole = 0;
//        resolveMrl();
//    }

//    endResetModel();
}

void NodeChildrenModel::initLink()
{
    Q_ASSERT(m_model);
    QList<int> roles = m_model->roleNames().keys();
    QList<int>::const_iterator res;
    res = std::max_element(roles.cbegin(), roles.cend());
    lastSrcRole = *res + 1;
//    beginResetModel();
    connect(m_model, &QAbstractItemModel::dataChanged, this, &NodeChildrenModel::onSourceDataChanged);
    connect(m_model, &QAbstractItemModel::rowsAboutToBeInserted, this, &NodeChildrenModel::onSourceRowsAboutToBeInserted);
    connect(m_model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &NodeChildrenModel::onSourceRowsAboutToBeRemoved);
    connect(m_model, &QAbstractItemModel::rowsAboutToBeMoved, this, &NodeChildrenModel::onSourceRowsAboutToBeMoved);
    connect(m_model, &QAbstractItemModel::rowsInserted, this, &NodeChildrenModel::onSourceRowsInserted);
    connect(m_model, &QAbstractItemModel::rowsRemoved, this, &NodeChildrenModel::onSourceRowsRemoved);
    connect(m_model, &QAbstractItemModel::rowsMoved, this, &NodeChildrenModel::onSourceRowsMoved);
    connect(m_model, &QAbstractItemModel::modelReset, this, &NodeChildrenModel::onSourceModelReset);
    connect(m_model, &QAbstractItemModel::modelAboutToBeReset, this, &NodeChildrenModel::onSourceModelAboutToBeReset);
    connect(m_model, &GraphModel::outEdgeAdded, this, &NodeChildrenModel::onOutEdgeAdded);
    connect(m_model, &GraphModel::inEdgeAdded, this, &NodeChildrenModel::onInEdgeAdded);
    endResetModel();
    //    m_model->proxies.insert(m_model->getNode(m_rootIndex), this);
}

void NodeChildrenModel::setLinked(bool linked)
{
    if (linked == m_linked)
        return;
    m_linked = linked;
    emit linkedChanged(m_linked);
}






