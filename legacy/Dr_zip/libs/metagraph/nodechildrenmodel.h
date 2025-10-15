#ifndef SUBMODEL_H
#define SUBMODEL_H

#include <QAbstractListModel>
#include <edge.h>
#include "node.h"


class NodeChildrenModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QModelIndex rootIndex READ rootIndex WRITE setRootIndex NOTIFY rootIndexChanged)
    Q_PROPERTY(QJSValue rootHandle READ rootHandle NOTIFY rootHandleChanged)
    Q_PROPERTY(GraphModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString mrl READ mrl WRITE setMrl NOTIFY mrlChanged)
    Q_PROPERTY(bool linked READ isLinked NOTIFY linkedChanged)

public:
    NodeChildrenModel(QObject *parent = 0);
    ~NodeChildrenModel();
    void classBegin() {}
    void componentComplete();
    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &) const;
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    int count() const;

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex mapToSource(int rowIndex) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

    GraphModel *sourceModel() const;
    QVariant data(const QModelIndex &index, int role) const;

    inline const QModelIndex &rootIndex() const { return m_rootIndex; }
    QJSValue rootHandle() const;
    GraphModel *model() const;
    bool isLinked() const;

    QString mrl() const;

    void setRootIndex(QModelIndex rootIndex);
    void setHandle(NodeHandle rootHandle);
    void setModel(GraphModel* model);
    void setMrl(QString mrl);

    Q_INVOKABLE ModelAgent *get(int index);
    Q_INVOKABLE QVariant getHandle(int index);
    Q_INVOKABLE QVariant getHandle(const QString &mrl) const;
    Q_INVOKABLE inline QVariant findChild(const QString &mrl) const { return getHandle(mrl); }
    Q_INVOKABLE bool hasChild(QString id) const;

public slots:
    void remove(int index);
    void remove(QList<int> list);

private slots:
    void onSourceDataChanged          (const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void onSourceRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onSourceRowsInserted         (const QModelIndex &parent, int, int);
    void onSourceRowsAboutToBeRemoved (const QModelIndex &parent, int first, int last);
    void onSourceRowsRemoved          (const QModelIndex &parent, int, int);
    void onSourceRowsAboutToBeMoved   (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);
    void onSourceRowsMoved            (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);
    void onOutEdgeAdded               (const QModelIndex &index, EdgeHandle edgeHandle);
    void onInEdgeAdded                (const QModelIndex &index, EdgeHandle edgeHandle);
    void onSourceModelReset           ();
    void onSourceModelAboutToBeReset  ();
    void resolveMrl();

protected:
//    void setRootIndex(const QModelIndex &root) { m_rootIndex = root; }
signals:
    void countChanged();
    void rootHandleChanged();
    void rootIndexChanged();
    void modelChanged();
    void mrlChanged();
    void nodeAdded(int index, QVariant handle);
    void nodeRemoved(int index, QVariant handle);
    void linkedChanged(bool isLinked);
    void inEdgeAdded(int index, EdgeHandle edge);
    void outEdgeAdded(int index, EdgeHandle edge);




private:
    friend class GraphModel;
    void resetLink();
    void initLink();
    void setLinked(bool isLinked);

    GraphModel*             m_model = nullptr;
    QPersistentModelIndex   m_rootIndex;
    int                     lastSrcRole = 0;
    QString                 m_mrl;
    bool                    sentinel :1;
    bool                    m_linked :1;

};

#endif // SUBMODEL_H
