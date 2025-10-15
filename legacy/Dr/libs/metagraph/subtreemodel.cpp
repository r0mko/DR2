#include "subtreemodel.h"
#include "graphmodel.h"
#define checkModel(ret) if (!m_sourceModel) return ret

SubtreeModel::SubtreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    GraphModel *m = qobject_cast<GraphModel*>(parent);
    if (m)
        setSourceModel(m);
    qDebug() << "created proxy tree model" << m;
}

QModelIndex SubtreeModel::index(int row, int column, const QModelIndex &parent) const
{
    return mapFromSource(m_sourceModel->index(row, column, mapToSource(parent)));
}

QModelIndex SubtreeModel::parent(const QModelIndex &child) const
{
    return mapFromSource(m_sourceModel->parent(mapToSource(child)));
}

int SubtreeModel::rowCount(const QModelIndex &parent) const
{
    checkModel(0);
    return m_sourceModel->rowCount(mapToSource(parent));
}

int SubtreeModel::columnCount(const QModelIndex &parent) const
{
    checkModel(0);
    return m_sourceModel->columnCount(mapToSource(parent));
}

QVariant SubtreeModel::data(const QModelIndex &index, int role) const
{
    checkModel(QVariant());
    return m_sourceModel->data(mapToSource(index), role);
}

QHash<int, QByteArray> SubtreeModel::roleNames() const
{
    QHash<int, QByteArray> ret;
    checkModel(ret);
    return m_sourceModel->roleNames();
}

GraphModel *SubtreeModel::sourceModel() const
{
    return m_sourceModel;
}

QModelIndex SubtreeModel::rootIndex() const
{
    return m_rootIndex;
}

void SubtreeModel::setSourceModel(GraphModel *src)
{
    if (m_sourceModel == src)
        return;
    m_sourceModel = src;

    connect(src, &QAbstractItemModel::modelReset, this, &SubtreeModel::onSourceModelReset);
    connect(src, &QAbstractItemModel::modelAboutToBeReset, this, &SubtreeModel::onSourceModelAboutToBeReset);
    connect(src, &QAbstractItemModel::dataChanged, this, &SubtreeModel::onSourceDataChanged);
    connect(src, &QAbstractItemModel::rowsAboutToBeInserted, this, &SubtreeModel::onSourceRowsAboutToBeInserted);
    connect(src, &QAbstractItemModel::rowsAboutToBeRemoved, this, &SubtreeModel::onSourceRowsAboutToBeRemoved);
    connect(src, &QAbstractItemModel::rowsAboutToBeMoved, this, &SubtreeModel::onSourceRowsAboutToBeMoved);
    connect(src, &QAbstractItemModel::rowsInserted, this, &SubtreeModel::onSourceRowsInserted);
    connect(src, &QAbstractItemModel::rowsRemoved, this, &SubtreeModel::onSourceRowsRemoved);
    connect(src, &QAbstractItemModel::rowsMoved, this, &SubtreeModel::onSourceRowsMoved);
    emit sourceModelChanged();
}

void SubtreeModel::setRootIndex(const QModelIndex &rootIndex)
{
    beginResetModel();
    m_rootIndex = rootIndex;
    GraphModel *m = qobject_cast<GraphModel*>(const_cast<QAbstractItemModel*>(rootIndex.model()));
    if (m && m_sourceModel != m) {
        setSourceModel(m);
    }
    endResetModel();
}

QModelIndex SubtreeModel::toSource(const QModelIndex &index) const
{
    return mapToSource(index);
}

QModelIndex SubtreeModel::fromSource(const QModelIndex &index) const
{
    return mapFromSource(index);
}

void SubtreeModel::onSourceModelReset()
{
    state = ResettingModel;
    m_rootIndex = QModelIndex();
    endResetModel();

}

void SubtreeModel::onSourceModelAboutToBeReset()
{
    if (state == ResettingModel) {
        beginResetModel();
        state = None;
    }
}

void SubtreeModel::onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (topLeft.parent() != m_rootIndex && bottomRight != m_rootIndex)
        return;
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
}

void SubtreeModel::onSourceRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    bool ok;
    QModelIndex local = mapFromSource(parent, &ok);
    if (ok) {
        beginInsertRows(local, first, last);
        state = InsertingRows;
    }
}

void SubtreeModel::onSourceRowsInserted(const QModelIndex &, int, int)
{
    if (state == InsertingRows) {
        endInsertRows();
        state = None;
    }
}

void SubtreeModel::onSourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    bool ok;
    QModelIndex local = mapFromSource(parent, &ok);
    if (!ok) {
        if (m_rootIndex.parent() == parent) {
            if (m_rootIndex.row() >= first && m_rootIndex.row() <= last) {
                //                qWarning() << "Destroying rootIndex of" << this;
                m_sourceModel->disconnect(0, this, 0);
                deleteLater();
                state = ResettingModel;
            }
        }
        return;
    }
    beginRemoveRows(local, first, last);
    state = RemovingRows;
}

void SubtreeModel::onSourceRowsRemoved(const QModelIndex &, int, int)
{
    if (state == RemovingRows) {
        endRemoveRows();
        state = None;
    }

}

void SubtreeModel::onSourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    bool okSrc;
    bool okDst;
    QModelIndex localSrc = mapFromSource(sourceParent, &okSrc);
    QModelIndex localDst = mapFromSource(destinationParent, &okDst);
    if (okSrc && okDst) {
        beginMoveRows(localSrc, sourceStart, sourceEnd, localDst, destinationRow);
        state = MovingRows;
    } else if (okSrc && !okDst) {
        beginRemoveRows(localSrc, sourceStart, sourceEnd);
        state = RemovingRows;
    } else if (!okSrc && okDst) {
        beginInsertRows(localDst, destinationRow, destinationRow + (sourceEnd - sourceStart));
        state = InsertingRows;
    }
}

void SubtreeModel::onSourceRowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)
{
    switch (state) {
    case InsertingRows:
        endInsertRows();
        break;
    case RemovingRows:
        endRemoveRows();
        break;
    case MovingRows:
        endMoveRows();
        break;
    default:
        break;
    }
    state = None;
}

QModelIndex SubtreeModel::mapToSource(const QModelIndex &proxyIndex) const
{
    checkModel(QModelIndex());

//    qDebug() << "map proxy index" << proxyIndex << "to source" << sourceModel()->address(m_rootIndex);
    if (!proxyIndex.isValid())
        return m_rootIndex;
    QModelIndex ret = m_sourceModel->createIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.internalPointer());
//    qDebug() << "mapped proxy index" << proxyIndex << "to source" << m_sourceModel->address(ret) << m_sourceModel->getNodeId(ret);
    return ret;
}

QModelIndex SubtreeModel::mapFromSource(const QModelIndex &sourceIndex, bool *hit) const
{
//    qDebug() << "map source index" << sourceIndex << m_sourceModel->address(sourceIndex) << m_sourceModel->address(m_rootIndex) << m_rootIndex.isValid();
    if (sourceIndex == m_rootIndex) {
        if (hit)
            *hit = true;
        return QModelIndex();
    }
    QModelIndex idx = sourceIndex;
    while (idx.isValid() && idx != m_rootIndex)
        idx = idx.parent();
    if (!idx.isValid()) {
        if (hit)
            *hit = false;
        return QModelIndex();
    }
    QModelIndex ret = createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
//    qDebug() << "mapped source index" << sourceIndex << "to proxy" << ret;
    if (hit)
        *hit = true;
    return ret;
}

