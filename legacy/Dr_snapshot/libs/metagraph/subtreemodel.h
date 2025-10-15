#ifndef SUBTREEMODEL_H
#define SUBTREEMODEL_H

#include <qabstractitemmodel.h>

class GraphModel;
/**
 * @brief Частичная модель в виде дерева.
 */
class SubtreeModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QModelIndex rootIndex READ rootIndex WRITE setRootIndex NOTIFY rootIndexChanged)
    Q_PROPERTY(GraphModel *sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)

    enum State {
        None,
        InsertingRows,
        RemovingRows,
        MovingRows,
        ResettingModel
    };

    State state = None;
    GraphModel *m_sourceModel = nullptr;
    QPersistentModelIndex m_rootIndex;

public:
    SubtreeModel(QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    GraphModel *sourceModel() const;
    QModelIndex rootIndex() const;
    void setSourceModel(GraphModel * sourceModel);
    void setRootIndex(const QModelIndex &rootIndex);
    Q_INVOKABLE QModelIndex toSource(const QModelIndex &index) const;
    Q_INVOKABLE QModelIndex fromSource(const QModelIndex &index) const;
public slots:
private slots:
    void onSourceModelReset           ();
    void onSourceModelAboutToBeReset  ();
    void onSourceDataChanged          (const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void onSourceRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onSourceRowsInserted         (const QModelIndex &parent, int, int);
    void onSourceRowsAboutToBeRemoved (const QModelIndex &parent, int first, int last);
    void onSourceRowsRemoved          (const QModelIndex &parent, int, int);
    void onSourceRowsAboutToBeMoved   (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);
    void onSourceRowsMoved            (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);

signals:
    void sourceModelChanged();
    void rootIndexChanged(QModelIndex rootIndex);

protected:
    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex, bool *hit = 0) const;


};

#endif // SUBTREEMODEL_H
