#ifndef EDGEMODEL_H
#define EDGEMODEL_H
#include <QAbstractListModel>
#include "graphmodel.h"
#include "hashstring.h"

class GraphModel;
class Edge;
class EdgeModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_FLAGS(DirectionFlags)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged)
    Q_PROPERTY(GraphModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QModelIndex rootIndex READ rootIndex WRITE setRootIndex NOTIFY rootIndexChanged)
    Q_PROPERTY(DirectionFlags direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(QString mrl READ mrl WRITE setMrl NOTIFY mrlChanged)
    Q_PROPERTY(bool linked READ linked NOTIFY linkedChanged)

    void appendEdge(Edge* e);

    friend class Edge;
    friend class GraphModel;
public:

    enum DirectionFlag {
        Any = 0,
        Outgoing = 0x1,
        Incoming = 0x2,
        Interior = 0x3
    };
    Q_FLAG(DirectionFlag)
    Q_DECLARE_FLAGS(DirectionFlags, DirectionFlag)

    enum Roles
    {
        StartNodeRole = Qt::UserRole + 100,
        EndNodeRole,
        EdgeTagRole,
        EdgeUidRole,
        EdgeDataRole
    };

    EdgeModel(QObject *parent = nullptr);
    ~EdgeModel();

    void componentComplete();
    void classBegin() {}

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const; //!
    Q_INVOKABLE EdgeHandle getHandle(int index) const;
    Q_INVOKABLE EdgeHandle getHandleByUid(uint uid) const;
    Q_INVOKABLE int getIndexByUid(uint uid) const;
    Q_INVOKABLE QVariant edgeData(int index) const;

    QString tag() const;
    void setTag(QString tag);

    GraphModel *model() const;
    void setModel(GraphModel* model);

    int count() const;

    QModelIndex rootIndex() const;
    void setRootIndex(QModelIndex rootIndex);

    DirectionFlags direction() const;
    void setDirection(DirectionFlags direction);

    QString mrl() const;
    void setMrl(QString mrl);


    bool linked() const
    {
        return bound;
    }

signals:
    void itemAdded(int index, uint uid);
    void edgeRemoved(int index, uint uid);
    void tagChanged(QString tag);
    void modelChanged();
    void rootIndexChanged();
    void directionChanged();
    void countChanged();
    void linkedChanged(bool linked);

    void mrlChanged(QString mrl);

public slots:
    void clear();

private slots:
    void onEdgeAdded(Node *firstNode, Node *endNode, Edge *edge);
    void onEdgeAboutToBeRemoved( uint uid );
//    void
    void onRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void onRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);

//    void onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
//    void onRowsRemoved(const QModelIndex &parent, int first, int last);

    void hold();
    void onEdgeDataChanged(Edge *edge);
    void onEdgeTagsChanged(Edge *edge);
private:
    QModelIndex addEdge(Edge *edge); // added that, I did.
    bool filterEdge(Edge *edge) const;
    void rescan();
    void initBinding();

    HashString          m_tag;
//    uint                m_rawtag = 0;
    GraphModel *        m_model = nullptr;
    QList<uint>         m_edges;
    QHash<uint, Edge*>  uid2edge;
    QPersistentModelIndex m_rootIndex;
    DirectionFlags      m_direction = Interior;
    bool                complete = false;
    bool                bound = false;
    QString m_mrl;
};

#else

#endif // EDGEMODEL_H
