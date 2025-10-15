#ifndef TREEPROXY_H
#define TREEPROXY_H

#include <QAbstractItemModel>




class TreeProxy: public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractListModel* sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)

    QAbstractListModel* m_sourceModel{nullptr};

public:
    TreeProxy(QObject * parent = 0);



    // QAbstractProxyModel interface
public:
    QAbstractListModel* sourceModel() const
    {
        return m_sourceModel;
    }
public slots:
    void setSourceModel(QAbstractListModel* sourceModel)
    {
        if (m_sourceModel == sourceModel)
            return;

        m_sourceModel = sourceModel;
        emit sourceModelChanged(sourceModel);
    }
signals:
    void sourceModelChanged(QAbstractListModel* sourceModel);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // TREEPROXY_H
