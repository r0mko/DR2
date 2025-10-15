#ifndef EDGEMODEL_H
#define EDGEMODEL_H
#include <QAbstractListModel>
#include "graphmodel.h"

class GraphModel;
class Edge;
class EdgeModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged)
    Q_PROPERTY(GraphModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(bool all READ all WRITE setAll NOTIFY allChanged)
    Q_PROPERTY(int count READ count WRITE setCount)

    uint                m_rawtag = 0;
    GraphModel *        m_model = nullptr;
    bool                m_all = false;
    QList<uint>        m_edges;
    QHash<uint, Edge*>    uid2edge;

    void appendEdge(Edge* e);
//    friend void printIndex(QModelIndex); //!?
public:
    friend class Edge;
    friend class GraphModel;
    enum Roles
    {
        StartNodeData = Qt::UserRole + 100,
        EndNodeData,
        StartNodeIndex,
        EndNodeIndex,
        EdgeTagRole,
        EdgeDataRole,
        EdgeObjectRole
    };
    EdgeModel();
    ~EdgeModel();
    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
//    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const; //!
//    QModelIndex addEdge(QPersistentModelIndex start, QPersistentModelIndex end, QString tag, QVariant data); //?!
    Q_INVOKABLE EdgeHandle getHandle(int index) const;
    Q_INVOKABLE EdgeHandle getHandleByUid(uint uid) const;
    Q_INVOKABLE QVariant edgeData(int index) const;

public slots:
    void clear();

private:
    QModelIndex addEdge(Edge *edge); // added that, I did.

    int m_count;

private slots:
    void onEdgeAdded(Node *firstNode, Node *endNode, Edge *edge);
    void onEdgeAboutToBeRemoved( uint uid );


public:
    QString tag() const
    {
        return GraphModel::edgeTags.value(m_rawtag);
    }
    GraphModel *model() const;
    bool all() const
    {
        return m_all;
    }

    int count() const
    {
        return m_edges.size();
    }

public slots:
    void setTag(QString tag)
    {
        uint hash = qHash(tag, MAGIC_V2);
        if (hash == m_rawtag)
            return;

        m_rawtag = hash;
        GraphModel::edgeTags[hash] = tag;
        emit tagChanged(tag);
    }
    void setModel(GraphModel* model)
    {
        if (m_model == model)
            return;

        if( m_model !=nullptr )
            disconnect(m_model, 0, this, 0);

        m_model = model;
        emit modelChanged(model);
        connect( m_model, &GraphModel::edgeAdded, this, &EdgeModel::onEdgeAdded );
        connect( m_model, &GraphModel::edgeAboutToBeRemoved, this, &EdgeModel::onEdgeAboutToBeRemoved );
    }
    void setAll(bool all)
    {
        if (m_all == all)
            return;

        m_all = all;
        emit allChanged(all);
    }

    void setCount(int count)
    {
        m_count = count;
    }

signals:
    void itemAdded(int index, uint uid);
    void tagChanged(QString tag);
    void modelChanged(GraphModel* model);
    void allChanged(bool all);
};
#else

#endif // EDGEMODEL_H
