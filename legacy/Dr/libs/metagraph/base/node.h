#ifndef NODE_H
#define NODE_H

#define QT_SHAREDPOINTER_TRACK_POINTERS
#define debugPrint qDebug()
#define SELF_TYPEID qMetaTypeId<std::remove_cv<std::remove_pointer<decltype(this)>::type>::type>()
#define SELF_TYPE QByteArray(QMetaType::typeName(SELF_TYPEID))
#define METAGRAPH_NODE(Class)                                                       \
    friend class Class ## Handle;                                                 \
    friend class Class ## Factory;
#define idxCheck(X) if (!d.isValid()) return X
#define NODE_FN(X) \
    friend class X;                                                           \
    friend class X ## Factory;                                                \
    inline X *node() const { return static_cast<X*>(NodeHandle::node()); }


#include "handletemplate.h"
#include <QJSValue>
#include <QQmlParserStatus>
#include <QVariant>
#include <QAbstractItemModel>
#include <QList>
#include <QSharedPointer>
#include <QDebug>

class Edge;
class GraphModel;

class Node
{
public:
    void checkAlloc();
    METAGRAPH_NODE(Node)

    Node *                                  m_parent = 0;   ///> родительский узел
    QList< Node * >                         m_childNodes;   ///> дочерние узлы
    QList< QSharedPointer<Edge> >           m_outEdges;     ///> исходящие рёбра
    QList< QWeakPointer<Edge> >             m_inEdges;      ///> входящие рёбра
    mutable QList<Node*>::const_iterator    it;             ///> индекс в родительском узле

    /**
     * @brief Флаги узла, определяющие состав хранимых данных и поведение
     */
    enum NodeFlag {
        // edges mode
        KeepsOutEdges = 0x1,                          ///> в узле есть список исходящих связей
        OwnsOutEdges = 0x3,                           ///> исходящие связи принадлежат узлу
        KeepsInEdges = 0x4,                           ///> в узле есть список входящих связей. Если не установлен ни один из флагов \c KeepsOutEdges или \c KeepsInEdges, список связей (рёбер) должен храниться во внешней модели.
        // children/parent mode
        IsMetaNode = 0x30,                            ///> метаузел — тип узла, не допускающий создание дочерних узлов. Если данный флаг установлен, то
        KeepsChildren = 0x20,                         ///> узел содержит список дочерних узлов. Если установлен этот флаг, но
        OwnsChildren = 0x60,                          ///> узел является владельцем списка дочерних узлов
        HasParent = 0x40,                             ///> в узле содержится указатель на родительский узел
        KeepsId = 0x80,                               ///> узел хранит идентификатор. В случае, если флаг не установлен, идентификатор узла хранится в модели.

    };

    enum TagFilter {
        MatchAny,
        MatchAll
    };

    friend class GraphModel;
    //    friend class MetaNode;


    Node();
    virtual ~Node();

    inline void fixIt(bool recursive = false) const;

    // edges getters
    inline const QList<QWeakPointer<Edge> > &inEdges() const { return m_inEdges; }
    inline const QList<QSharedPointer<Edge> > &outEdges() const { return m_outEdges; }
    inline const QList<Node*> &childNodes() const { return m_childNodes; }

    QList<Edge *> findEdges(QStringList tagFilter = QStringList(), TagFilter mode = MatchAny) const;
    QList<Node *> findNeighborNodes(QStringList tagFilter = QStringList(), TagFilter mode = MatchAny) const;

    QList<Edge *> findOutEdges(QStringList tagFilter = QStringList(), TagFilter mode = MatchAny) const;
    QList<Node *> findOutNodes(QStringList tagFilter = QStringList(), TagFilter mode = MatchAny) const;

    QList<Edge *> findInEdges(QStringList tagFilter = QStringList(), TagFilter mode = MatchAny) const;
    QList<Node *> findInNodes(QStringList tagFilter = QStringList(), TagFilter mode = MatchAny) const;
    // edges setters
    void removeInEdge(Edge *edge);
    void removeOutEdge(Edge *edge);

    //children getters
    inline int childCount() const { return m_childNodes.size(); }
    inline bool isValid() const { return it.i != nullptr; }

    inline Node *parent() const { return m_parent; }
    inline int row() const;
    inline int getRowId() const;
    int treeSize() const;
    int depth() const;
    // children setters

    Node *takeChild(int i);
    void removeChild(Node *child);
    void removeChild(int n);
    void prependChild(Node *child);
    void appendChild(Node *child);
    void insertChild(int pos, Node *child);
    void setParent(Node *parent);
    void removeChildren(int n, int count, GraphModel *model);

    void moveChildren(int from, int count, int to);
    void moveChild(int from, int to);
    void moveChildUp(int index);
    void moveChildDown(int index);

    // RTTI
    virtual int typeId() const;

    //    void save(QDataStream &out, GraphModel *model);
    //    void load(QDataStream &in, GraphModel *model, const QModelIndex &parent = QModelIndex());
    template <typename T>
    bool is() const { return typeId() == qMetaTypeId<T>(); }
    bool addInEdge(QSharedPointer<Edge> edge);
    bool addOutEdge(QSharedPointer<Edge> edge);

    // Data
    inline virtual QVariant data() const { return QVariant(); }
    inline virtual void setData(const QVariant &) {}

    int allocSize = 4;
    inline void setRow(int idx);
    void reset();
    static GraphModel *model(const QModelIndex &nodeIndex);
    void unlink(GraphModel *model);

    //    virtual bool nodeChange(NodeChange change, const NodeChangeData &data);//; { Q_UNUSED(change); Q_UNUSED(data); return true; }
    inline QList<Node*> &_childNodes() { return m_childNodes; }
    // TODO: delete
    QList<Node *> outNodes(const QString &edgeTag) const;
    QList<Node *> inNodes(const QString &edgeTag) const;
    void clear();

};

void Node::fixIt(bool recursive) const
{
    for (auto it = m_childNodes.cbegin(); it != m_childNodes.cend(); ++it) {
        (*it)->it = it;
        if ((*it)->m_parent != this) {
            qWarning() << "child with wrong parent" << m_parent << (*it)->m_parent;
            (*it)->m_parent = const_cast<Node*>(this);
        }
        if(recursive) (*it)->fixIt(recursive);
    }
}

Q_DECLARE_METATYPE(Node)

#include <QPersistentModelIndex>

class GraphModel;
class ModelAgent;

class NodeHandle
{
    Q_GADGET

    Q_PROPERTY(QJSValue parent READ parent WRITE setParent)
    Q_PROPERTY(QString nodeId READ nodeId WRITE setNodeId)
    Q_PROPERTY(int rowIndex READ rowIndex WRITE setRowIndex)

    Q_PROPERTY(QString typeName READ typeName CONSTANT)
    Q_PROPERTY(int typeId READ typeId CONSTANT)
    Q_PROPERTY(int childCount READ childCount)
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(QModelIndex modelIndex READ modelIndex)
    Q_PROPERTY(QVariantList childNodes READ childNodes)
    Q_PROPERTY(QList<int> address READ address)
    Q_PROPERTY(QString mrl READ mrl)
    Q_PROPERTY(QModelIndex parentIndex READ parentIndex)
    Q_PROPERTY(ModelAgent *agent READ agent)
    Q_PROPERTY(QVariant nodeData READ nodeData)

    friend class EdgeHandle;
    friend class GraphModel;
public:
    GraphModel *model() const;

    NodeHandle() {}
    NodeHandle(const QModelIndex &index) : d(index) {}
    virtual ~NodeHandle();

    QJSValue parent() const;
    void setParent(QJSValue newParent);

    QString nodeId() const;
    void setNodeId(QString nodeId);

    int rowIndex() const;
    void setRowIndex(int rowIndex);

    QString typeName() const;
    int typeId() const;
    int childCount() const;
    inline QModelIndex parentIndex() const { return d.parent(); }
    bool isValid() const;
    inline const QModelIndex &modelIndex() const { return d;}
    QVariantList childNodes() const;
    QList<int> address() const;
    QString mrl() const;
    ModelAgent * agent() const;
    QVariant nodeData() const;

    Q_INVOKABLE QVariantList edges(QString tag = QString()) const;
    Q_INVOKABLE QVariantList neighbors(QString tag = QString()) const;
    Q_INVOKABLE QVariantList outEdges(QString tag = QString()) const;
    Q_INVOKABLE QVariantList inEdges(QString tag = QString()) const;
    Q_INVOKABLE QVariantList outNodes(QString tag = QString()) const;
    Q_INVOKABLE QVariantList inNodes(QString tag = QString()) const;
    Q_INVOKABLE QString dumpToJson() const;
    Q_INVOKABLE void destroy();
    Q_INVOKABLE QVariant sibling(int index) const;
    Q_INVOKABLE QVariant nextSibling() const;
    Q_INVOKABLE QVariant prevSibling() const;

    Q_INVOKABLE void moveUp();
    Q_INVOKABLE void moveDown();
    Q_INVOKABLE void moveToHead();
    Q_INVOKABLE void moveToTail();


    Node *node() const;
    bool operator ==(NodeHandle other) const { return d == other.d; }
    inline operator const QModelIndex&() const { return d; }


protected:
    QPersistentModelIndex d;
};

Q_DECLARE_METATYPE(NodeHandle)
//template<>
//struct Handle<Node>
//{
//    static inline int handlerMetaType() { return qMetaTypeId<NodeHandle>(); }
//    typedef NodeHandle type;
//    typedef NodeFactory* factoryType;
//    enum { isDefined = 1 };
//};

class NodeFactory;
template<>
struct HandleType<Node>
{
    static inline int handlerMetaType() { return qMetaTypeId<NodeHandle>(); }
    typedef NodeHandle type;
    typedef NodeFactory* factoryType;
    enum { isDefined = 1 };
};
class NodeChildrenModel;

class NodeFactory : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString nodeId READ nodeId WRITE setNodeId NOTIFY nodeIdChanged)
    Q_PROPERTY(NodeHandle parentHandle READ parentHandle WRITE setParentHandle NOTIFY parentChanged RESET resetParent)
    Q_PROPERTY(GraphModel* model READ model CONSTANT)
    Q_PROPERTY(QString nodeTypeName READ nodeTypeName CONSTANT)

    GraphModel *            m_model = nullptr;
    QPersistentModelIndex   m_parentIndex;
    QString                 m_nodeId;

    friend class GraphModel;

public:
    NodeFactory(QObject *parent = 0);
    virtual ~NodeFactory();
    void classBegin();
    void componentComplete();
    inline GraphModel *model() const { return m_model; }
    virtual int nodeMetatypeId() const;
    Node *createNode(const QString &nodeId = QString());
    //    virtual void serialize(QDataStream &out) const { Q_UNUSED(out); }
    //    virtual void deserialize(QDataStream &in) { Q_UNUSED(in); }
    virtual void importNode(QDataStream &in, Node *node) ;
    virtual void exportNode(QDataStream &out, Node *node) const;

    inline QString nodeId() const { return m_nodeId; }
    inline NodeHandle parentHandle() const { return NodeHandle(m_parentIndex); }
    inline QPersistentModelIndex rootIndex() const { return m_parentIndex; }

    void setNodeId(QString nodeId);
    void setParentIndex(const QPersistentModelIndex &rootIndex);
    void setParentHandle(NodeHandle parent);

    QString nodeTypeName() const;
    Q_INVOKABLE QVariant beginCreate();
signals:
    void nodeIdChanged();
    void parentChanged();

public slots:
    void endCreate();
    void setParent(NodeChildrenModel *subModel);
    void resetParent();
    QVariant create(const QString &nodeId = QString());

protected:
    //    void initNode(Node *node, const QString &nodeId = QString());
    virtual void setupNode(Node *) {}
};

int Node::row() const { return std::distance(m_parent->m_childNodes.cbegin(), it); }

int Node::getRowId() const { return m_parent->m_childNodes.indexOf(const_cast<Node*>(this)); }

void Node::setRow(int idx) { it = m_parent->m_childNodes.cbegin() + idx; }



#endif // NODE_H
