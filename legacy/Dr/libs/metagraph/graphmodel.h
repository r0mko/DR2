#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H
#include <QAbstractListModel>
#include <QItemSelection>
#include <QQmlListProperty>
#include <QSet>
#include <QStack>
#include <QUrl>
#include <graphindex.h>
#include "node.h"
#include "edge.h"
#include "collection.h"
#include "datanode.h"
#include "metanode.h"
#include <QJsonDocument>
#include "modelagent.h"
#define MAGIC_V1 0x4D474632 // "MGF2"
#define MAGIC_V2 0x4D474633 // "MGF3"
class Collection2;
class AbstractModule;
class Edge;
class EdgeObject;
class NodeChildrenModel;

/**
 * @brief Хранит идентификатор узла и хэш дочерних узлов для быстрого поиска дочерних узлов по идентификатору.
 */
struct NId
{
    QString id;                                     ///> идентификатор узла
    QHash<QString, const Node *> children;  ///> словарь дочерних узлов
};

inline QDebug operator<<(QDebug debug, const NId &nodeId)
{
    debug.nospace() << "Node ID: [" << qPrintable(nodeId.id) << "] Child Node IDs: " << nodeId.children.keys();
    return debug.maybeSpace();
}
class ModelAgent;
class SubtreeModel;

int getRole(int nodeType, int propertyId);
bool roleToNodeProperty(int role, int *nodeTypeId, int *propertyId);
inline bool isParentOf(const QModelIndex &child, const QModelIndex &parent);


class GraphModel : public QAbstractItemModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<QObject> children READ qmlChildren NOTIFY childrenChanged)
    Q_PROPERTY(QVariantList nodeFactories READ nodeFactories NOTIFY nodeFactoriesChanged)
    Q_PROPERTY(ModelState status READ status WRITE setStatus NOTIFY statusChanged)

    Q_CLASSINFO("DefaultProperty", "children")

    struct private_sig {};

    friend class Node;
    friend class NodeFactory;
    friend class ModelWatcher;
    friend class NodeHandle;
    friend class SubtreeModel;
    friend class GraphModelHelper;
    friend class NodeChildrenModel;

public:
    /**
     * @brief Список ролей модели
     */
    enum NodeRoles {
        NodeIdRole = Qt::UserRole + 1,              ///< \c "nodeId" идентификатор узла
        DataRole,                                   ///< \c "data" данные узла
        HandleRole,                                 ///< \c "handle" ссылка на узел
        AgentRole,                                  ///< \c "object" объект \c ModelAgent узла
        RowRole,                                    ///< \c "rowIndex" индекс в родительском узле
        AddressRole,                                ///< \c "address" цепочка индексов до корневого узла
        MRLRole,                                    ///< \c "mrl" MRL узла
        NodeTypeRole,                               ///< \c "typeName" название типа узла
        GraphIndexRole,                             ///< \c "graphIndex" GraphIndex для узла
        ChildCountRole,                             ///< \c "childrenCount" количество дочерних узлов
        OutCountRole,                               ///< \c "outEdges" количество исходящих связей
        InCountRole,                                ///< \c "inEdges" количество входящих связей
        ChildrenRole,                               ///< \c "childrenModel" модель дочерних узлов
        LastNodeRole = ChildrenRole
    };
    /**
     * @brief Режим работы модели
     */
    enum ModelState {
        Invalid,     ///< Ошибочный режим.
        Ready,       ///< Нормальный режим. Разрешено добавление и удаление узлов.
        Locked,      ///< Режим блокировки. Любые операции с моделью запрещены.
        CreatingNode ///< Режим добавления узла. Создание новых элементов в этом режиме запрещено.
    };
    Q_ENUM(ModelState)

    static QHash<int, int> node2handle;
    static QHash<uint, QString> edgeTags;
    static QHash<QString, GraphModel*> graphModels;

    GraphModel(QObject *parent = 0);
    ~GraphModel();
    void classBegin();
    void componentComplete();

    void initModel(Node *root=nullptr);

    static QHash<int, QByteArray> extractRoles(int metaTypeId, const QMetaObject *mo, QHash<int, QByteArray> *propertyTypes, QString rolePrefix = QString());

    QHash<int, QByteArray> roleNames() const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &) const;
    bool removeRows(int row, int count, const QModelIndex &parent);
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild);
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    inline QModelIndex getNodeIndex(const Node *n) const;
    Node *getNode(const QModelIndex &idx) const;
    Node *getNode(const QString &mrl) const;
    Node *getNode(const Node *parent, const QString &id) const;

    void reparentNode(const Node *node, const Node *oldParent, const Node *newParent);
    void moveNode(Node *node, Node *destParent, int destIndex = -1);

    QModelIndex appendNode(Node *node, const QModelIndex &parent = QModelIndex());
    QModelIndex prependNode(Node *node, const QModelIndex &parent = QModelIndex());
    QModelIndex insertNode(int pos, Node *node, QModelIndex parent = QModelIndex());

    QModelIndex beginAppendNode(Node *node, const QModelIndex &parent = QModelIndex());
    QModelIndex beginPrependNode(Node *node, const QModelIndex &parent = QModelIndex());
    QModelIndex beginInsertNode(int pos, Node *node, QModelIndex parent = QModelIndex());

    void commit();
    ModelState status() const;

    void setNodeId(const Node *node, QString name, const Node *parent = 0);
    QString getNodeId(const Node* node) const;
    inline bool hasId(const QModelIndex &idx) const { return hasId(getNode(idx)); }
    QString getMRL(QModelIndex idx) const;
    inline const Node *root() const { return _root; }

    Q_INVOKABLE inline QString getNodeId(const QModelIndex &idx) const { return getNodeId(getNode(idx)); }
    Q_INVOKABLE NodeChildrenModel *getSubModel(QModelIndex idx) const;
    Q_INVOKABLE NodeChildrenModel *getSubModel(const QString &mrl) const;
    Q_INVOKABLE ModelAgent *get(const QModelIndex &idx) const;
    Q_INVOKABLE QString address(QModelIndex idx) const;
    Q_INVOKABLE QList<int> iPath(QModelIndex idx) const;
    Q_INVOKABLE QModelIndex childIndex(int n, const QModelIndex &parent = QModelIndex()) const;

    Q_INVOKABLE QVariant getRootHandle() const;
    Q_INVOKABLE QVariant getHandle(const QString &mrl) const;
    Q_INVOKABLE QVariant getHandle(const QModelIndex &idx) const;
    Q_INVOKABLE QVariant getHandle(QList<int> path) const;

    Q_INVOKABLE QVariant moveNode(const QModelIndex &nodeIndex, const QModelIndex &destParentIndex, int destRow = -1);

    void setNodeId(const QModelIndex &idx, QString id);
    QVariant getHandle(const Node *node) const;

    template <class N, class H = typename HandleType<N>::type>
    H createNodeHelper(NodeHandle handle = NodeHandle());

    template <class N, class F = typename HandleType<N>::factoryType>
    F factory() const;

    NodeFactory *factory(Node *node) const;
    NodeFactory *factory(int typeId) const;

    QVariantList nodeFactories() const;

    void exportNode(QDataStream &out, Node *node) const;
    QList<Edge *> exportEdges(QDataStream &out, const QModelIndex &root) const;
    Node *importNode(QDataStream &in, Node *parent, QString nodeId = QString());
    QList<int> iPathRelative(QModelIndex idx, const QModelIndex &root = QModelIndex(), bool *valid = nullptr) const;
    QModelIndex iPathToIndex(QList<int> addr, const QModelIndex &root = QModelIndex()) const;
    QModelIndexList importEdges(QDataStream &in, const QModelIndex &root);

    QJsonDocument dump(const QModelIndex &index = QModelIndex());
    QJsonValue dumpNode(const Node *n);
    QVariantMap namedChildren(const QModelIndex &parent) const;

    Node *importModelFile(const QUrl &source, const QModelIndex &parent = QModelIndex(), QString nodeId = QString(), bool open=false);
    Q_INVOKABLE QJSValue importFile(const QUrl &filename, const QModelIndex &parent, QString nodeId = QString());
    bool hasId(Node *node) const;
    void notify(const QModelIndex &index, const QVector<int> &roles);
    void addEdge(const QModelIndex &srcNode, const QModelIndex &destNode, QString tag, QVariant data = QVariant());
    void beginRemoveEdge(uint uid);
    void endRemoveEdges();

    inline static uint hashTag(const QString &str) { return qHash(str, MAGIC_V2); }


public slots:
    void clear();
    void exportModel(const QUrl &filename, const QModelIndex &rootElement = QModelIndex()) const;
    void loadModel(const QUrl &filename);
    void removeNode(const QModelIndex &index);
    void removeNodes(QModelIndexList indexList);
    void removeNodes(QItemSelection selection);
    void removeNode(int n, const QModelIndex &parent);
    void moveSelection(const QItemSelection &selection, const QModelIndex &destParentIndex, int destIndex = -1);
    void addEdge(int indexFrom, int indexTo, QString tag, QVariant data = QVariant(), NodeHandle parentFrom = NodeHandle(), NodeHandle parentTo = NodeHandle());
    void addEdge(const NodeHandle &srcNode, const NodeHandle &destNode, QString tag, QVariant data = QVariant());
//#ifdef QT_DEBUG
//    void dumpChildren(const QModelIndex& parent, bool recursive, int first = 0, int last = -1) const;
//#endif
private slots:
    void onRequestCollectionInit(Collection *collection, bool stored);
//    void onRequestDeferredMetaNodeMember(MetaNode *node, const QModelIndex &idx);
    void onRequestDeferredMetaNodeMember(MetaNode *node, const QList<int>& addr, QPersistentModelIndex rootindex);

signals:
    void saved(int nodeCount, int edgeCount, int filesize) const;
    void loaded(int nodeCount, int edgeCount, int filesize);

    void edgeAboutToBeRemoved(quint32 uid);
    void edgesRemoved(QList<uint> uids);

    void requestDeferredMetaNodeMember(MetaNode *node, const QList<int>& addr, QPersistentModelIndex rootindex);
    void requestInitCollection(Collection *collection, bool stored);
    void childrenChanged();
    void nodeFactoriesChanged();

    void outEdgeAdded(const GraphIndex &index, QWeakPointer<Edge> edge);


    void edgeAdded( Node *firstNode, Node *endNode, Edge* edge );
    void edgeRemoved( Node *firstNode, Node *endNode/*, Edge *edge*/ );

    void statusChanged(ModelState status);

private:
    typedef QHash<const Node*, NId> Node2NId;                                      ///< \c typedef для QHash<const Node*, NId>
    typedef QHash<const Node*, QPointer<NodeChildrenModel>> Node2ProxyModel;       ///< \c typedef для QHash<const Node*, QPointer<NodeChildrenModel>>
    typedef QHash<int, NodeFactory*> TypeId2Factory;                               ///< \c typedef для QHash<int, NodeFactory*>
    typedef QHash<Node*, QPointer<ModelAgent>> Node2Agent;                         ///< \c typedef для QHash<Node*, QPointer<ModelAgent>>



    NodeHandle *handleForIndex(const QModelIndex &index, int handleMetaTypeId) const;

    Node            *_root = nullptr;   ///< Корневой узел
    mutable QSet<Edge*>     edgeset;            ///< Временный список рёбер. Используется для различных внутренних нужд, в остальное время не используется.
    mutable Node2NId        node2nid;           ///< Полный перечень узлов с именем, так же содержит корневой узел
    mutable Node2ProxyModel proxies;            ///< Справочник частичных моделей для узлов. Пока содержит только модели типа \c NodeChildrenModel. @todo Сделать единую базу для прокси-моделей (дерево, список)
    mutable Node2Agent      agents;             ///< Справочник агентов модели для узлов.
    QPersistentModelIndex   incubatedNode;      ///< Индекс узла, создаваемого в текущей транзакции
    ModelState              m_status = Invalid; ///< Текущий режим работы модели
    TypeId2Factory          factories;          ///< Словарь фабрик для метатипа узла @todo: change to modules in future
    ModelAgent::proto       agentData;          ///< Прототип для создания \c ModelAgent.
    QString                 qmlId;              ///< id элемента GraphModel в QML
    bool                    mute = false;
    QSet<uint>              destroyingEdges;

    void removeNodeId(const Node *node);
    void notifyTreeIdChange(const Node *node);
    void exportFactories(QDataStream &out, Node *node) const;
    QSet<int> collectTypes(Node *rootNode) const;
    QString getUniqueNodeId(QString desiredId, const Node *parentNode) const;
    QString urlToFilename(const QUrl &url, QString *filename = 0) const;
    QHash<int, int> importFactories(QDataStream &in) const;
    void removeProxy(const Node *n) const;
    QWeakPointer<Edge> createEdge(const QModelIndex &startNode, const QModelIndex &endNode, const QStringList &tags, const QVariant &data);
    Node *resolveNode(const QList<int> &addr) const;
    QModelIndex index(const QList<int> &addr) const;
//    QList<QObject*> m_data;

    bool initCollection(Collection2 *collection, Type *type);
    void setStatus(ModelState status);

    QQmlListProperty<QObject> qmlChildren();

    static void qmlChildrenAppend(QQmlListProperty<QObject> *list, QObject *object);
    static int qmlChildrenCount(QQmlListProperty<QObject> *list);
    static QObject *qmlChildrenAt(QQmlListProperty<QObject> *list, int index);



    // QQmlParserStatus interface

    Node *importV1(QDataStream &in, const QModelIndex &parent, Node *parentNode, QString nodeId, bool open);
    Node *importV2(QDataStream &in, const QModelIndex &parent, Node *parentNode, bool open);


    // QAbstractItemModel interface
public:
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    Qt::DropActions supportedDropActions() const;
    Qt::DropActions supportedDragActions() const;

//    void dumpChildren(const QModelIndex &parent, bool recursive, int first, int last) const;
};

template <class N, class H>
H GraphModel::createNodeHelper(NodeHandle handle)
{
    N* n = new N;
    QModelIndex idx = handle.modelIndex();
    return H(appendNode(n, idx));
}

template <class N, class F>
F GraphModel::factory() const
{
    return qobject_cast<F>(factories.value(qMetaTypeId<N>()));
}

QModelIndex GraphModel::getNodeIndex(const Node *n) const
{
    if (n->parent())
        return createIndex(n->row(), 0, const_cast<Node*>(n));
    return QModelIndex();
}

bool isParentOf(const QModelIndex &child, const QModelIndex &parent)
{
    QModelIndex i = child;
    while (i.isValid()) {
        if (i.parent() == parent)
            return true;
        i = i.parent();
    }
    return false;
}


Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QPersistentModelIndex)




#endif // GRAPHMODEL_H
