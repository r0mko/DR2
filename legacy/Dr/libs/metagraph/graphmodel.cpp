#include "graphmodel.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonDocument>
#include <QQmlEngine>
#include <qqml.h>
#include <QStack>
#include "nodechildrenmodel.h"
#include "collection2.h"
#include <QCoreApplication>
#include <QDataStream>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QMimeData>
#include <QQmlContext>
#include <abstractmodule.h>
#include <graphindex.h>
#include "modelagent.h"
#include "modelwatcher.h"
#include "subtreemodel.h"
#include "type.h"
#include "rootnode.h"
QHash<int, int> GraphModel::node2handle = QHash<int, int>();
QHash<QString, GraphModel*> GraphModel::graphModels = QHash<QString, GraphModel*>();
QHash<uint, QString> GraphModel::edgeTags = QHash<uint, QString>();
QHash<int, int> nodeTypeTable;

/**
 * @brief Экспортирует модель или её часть в файл
 * @param filename имя файла
 * @param rootElement начальный узел экспортируемой модели. Если параметр не указан, экспортируется вся модель.
 */
void GraphModel::exportModel(const QUrl &filename, const QModelIndex &rootElement) const
{
    int magic = MAGIC_V2;
    QFile f(filename.toLocalFile());
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "Unable to save file" << filename.toLocalFile();
        return;
    }
    QDataStream out(&f);
    out.setVersion(QDataStream::Qt_5_4);
    out << magic;
    for (NodeFactory *f : factories) {
        f->setParentIndex(rootElement);
    }
    Node *node = getNode(rootElement);
    exportFactories(out, node);
    edgeset.clear();


    switch (magic) {
    case MAGIC_V1:
        qDebug() << "saving with MAGIC_V1";
        exportNode(out, node);
        break;
    case MAGIC_V2:
        qDebug() << "saving with MAGIC_V2";
        if( node->parent() != nullptr ) {
            out << 1;
            exportNode(out, node);
            qDebug() << "saving 1 node";
        }else{
            out << node->childCount();
            for( Node *n: node->childNodes() ){
                exportNode(out, n);
            }
            qDebug() << "saving "<<node->childCount()<<" nodes";
        }
        break;
    default:
        break;
    }

    exportEdges(out, rootElement);
    f.flush();
    f.close();
    emit saved(node->treeSize(), edgeset.size(), f.size());
}

/**
 * @brief Загружает модель из файла или ресурса
 * @param source имя файла или ресурса
 *
 * Загружает модель из файлв в корневой узел. Старые данные модели уничтожаются.
 */
void GraphModel::loadModel(const QUrl &source)
{
    beginResetModel();
    //    blockSignals(true);
    //    clear();
    mute = true;

    initModel();
    importModelFile(source, QModelIndex(), "", true);
    //    if( t )
    //        initModel(n);

    mute = false;
    //    blockSignals(false);

    endResetModel();
}

/**
 * @brief Добавляет корневой узел указанной модели в parent
 * @param source URL файла или ресурса
 * @param parent индекс родительского узла
 * @param nodeId идентификатор узла (опционально)
 * @return корневой узел загруженной модели
 *
 * Добавляет в узел \c parent корневой узел модели, загруженной из файла или ресурса \c source.
 * Если не указывать \c nodeId, будет использовано имя корневого узла, заданное в модели.
 * Если такое имя не уникально (неважно, задано оно принудительно с помощью \c nodeId или получено из модели), к нему добавляется цифровой суффикс _1, _2 и так далее.
 */
Node *GraphModel::importModelFile(const QUrl &source, const QModelIndex &parent, QString nodeId, bool open)
{
    QString fileId;
    QFile f(urlToFilename(source, &fileId));


    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Error loading" << f.fileName() << ": unable to open file" << QDir::currentPath();
        return nullptr;
    }
    QDataStream in(&f);
    in.setVersion(QDataStream::Qt_5_4);
    int magic;
    in >> magic;

    if (magic != MAGIC_V1 && magic != MAGIC_V2) {
        qWarning() << "Error loading" << f.fileName() << ": unknown file format";
        f.close();
        open = true;
        return nullptr;
    }
    nodeTypeTable = importFactories(in);
    if (in.status() != QDataStream::Ok) {
        qWarning() << "Error loading" << f.fileName() << ": file corrupted1";
        f.close();
        return nullptr;
    }
    Node *parentNode = getNode(parent);
    Node *ret;
    for (NodeFactory *f : factories) {
        f->setParentIndex(parent);
    }
    //    qDebug() << "importing" << filename << "to" << getNodeId(parentNode);
    if (nodeId.isEmpty())
        nodeId = getUniqueNodeId(fileId, parentNode);
    else
        nodeId = getUniqueNodeId(nodeId, parentNode);
    switch (magic) {
    case MAGIC_V1:
        ret = importV1(in, parent, parentNode, nodeId, open);
        break;
    case MAGIC_V2:
        ret = importV2(in, parent, parentNode, open);
        break;
    default:
        qFatal("Import model: WAT?!");
        break;
    }
    if (in.status() != QDataStream::Ok) {
        qWarning() << "Error importing file" << f.fileName() << ": file corrupted2";
    }
    f.close();

    return ret;
}


Node *GraphModel::importV1(QDataStream &in, const QModelIndex &parent, Node *parentNode, QString nodeId, bool open)
{


    /**/


    if (!mute)
        beginInsertRows(parent, parentNode->childCount(), parentNode->childCount());

    QScopedPointer<Node> newNode(importNode(in, parentNode, nodeId));

    if (!mute)
        endInsertRows();
    //    blockSignals(false);
    if (!newNode) {
        in.setStatus(QDataStream::ReadCorruptData);
        return nullptr;
    }
    QModelIndex newIdx = getNodeIndex(newNode.data());
    parentNode->fixIt();
    newNode->fixIt(true);
    importEdges(in, newIdx);
    if (!mute) {
        notifyTreeIdChange(newNode.data());
        notify(parent, { ChildCountRole });
    }

    if( open ) {
        initModel(newNode.take());
        return _root;
    }
    newNode->fixIt(true);
    return newNode.take();


    /**/
}

Node *GraphModel::importV2(QDataStream &in, const QModelIndex &parent, Node *parentNode, bool open)
{
    int count;
    in >> count;

    if (!mute)
        beginInsertRows(parent, parentNode->childCount(), parentNode->childCount() + count);
    qDebug() << "importing " << count << "nodes";
    while (count-- && in.status() == QDataStream::Ok) {
        importNode( in, parentNode );
    }
    if (!mute)
        endInsertRows();
    QModelIndex newIdx = getNodeIndex(parentNode);

    parentNode->fixIt();
    //    newNode->fixIt(true);
    importEdges(in, newIdx);
    if (!mute) {
        notifyTreeIdChange(parentNode);
        notify(parent, { ChildCountRole });
    }
    parentNode->fixIt(true);
    return parentNode;//getNode(parent);



}

/**
 * @copydoc GraphModel::importModelFile
 * Для вызова из QML. Возвращает ссылку (NodeHandle) соответствующего типа.
 */
QJSValue GraphModel::importFile(const QUrl &source, const QModelIndex &parent, QString nodeId)
{
    auto ret = qmlEngine(this)->toScriptValue(getHandle(importModelFile(source,  parent, nodeId)));
    return ret;
}

/**
 * @brief Удаляет узел по индексу \c index
 *
 * При удалении узлов так же удаляются все дочерние узлы и их связи.
 * @sa GraphModel::removeNodes
 */
void GraphModel::removeNode(const QModelIndex &index)
{
    if (index.isValid())
        removeRow(index.row(), index.parent());
}

/**
 * @deprecated
 * @brief Удаляет узлы по списку
 *
 * @sa void GraphModel::removeNodes(QItemSelection selection)
 */
void GraphModel::removeNodes(QModelIndexList indexList)
{
    QList<Node*> nodes;
    for (auto idx : indexList) {
        nodes.append(getNode(idx));
    }
    qDebug() << "removing" << nodes.size() << "nodes";
    for (Node *node : nodes) {
        QModelIndex parent = getNodeIndex(node->parent());
        removeRow(node->row(), parent);
    }
}
/**
 * @brief Удаляет узлы по выделению
 * @param selection выделенные узлы
 *
 * Удаляет узлы из модели, заданные \c QItemSelection, например, с помощью инструмента группового выделения в виде.
 */
void GraphModel::removeNodes(QItemSelection selection)
{
    for (QItemSelectionRange range : selection) {
        qDebug() << "removing range" << range.height() << "rows";
        removeRows(range.top(), range.height(), range.parent());
    }
}
/**
 * @brief Удаляет дочерний узел по номеру
 * @param n номер дочернего узла
 * @param parent родительский узел
 * @sa void GraphModel::removeNode(const QModelIndex &index)
 */
void GraphModel::removeNode(int n, const QModelIndex &parent)
{
    removeRow(n, parent);
}

QVariant GraphModel::moveNode(const QModelIndex &nodeIndex, const QModelIndex &destParentIndex, int destRow)
{
    Node *n = getNode(nodeIndex);
    QModelIndex srcParent = nodeIndex.parent();
    if (destRow < 0) {
        destRow = rowCount(destParentIndex);
        if (srcParent == destParentIndex)
            --destRow;
    }
    if (moveRow(srcParent, nodeIndex.row(), destParentIndex, destRow))
        return getHandle(n);
    return QVariant();
}

void GraphModel::removeProxy(const Node *n) const
{
    if (proxies.contains(n)) {
        proxies.value(n)->deleteLater();
        proxies.remove(n);
    }
}

QWeakPointer<Edge> GraphModel::createEdge(const QModelIndex &start, const QModelIndex &end, const QStringList &tags, const QVariant &data)
{
    if (!start.isValid() || !end.isValid()) {
        qWarning() << "Invalid index" << start << end << data.toString();
        return QWeakPointer<Edge>();
    }
    Node *ns = getNode(start);
    Node *ne = getNode(end);

    for (auto e : ns->outEdges()) {
        if (e->endNode() == ne) {
            qWarning() << getNodeId(ns) << "and" << getNodeId(ne) << "already connected" << e->tags();
            return e;
        }
    }
    //    debugPrint << "adding edge from" << getMRL(start) << "to" << getMRL(end) << "tag" << tag;
    QSharedPointer<Edge> edge = QSharedPointer<Edge>::create();
    edge->setData(data);
    for (QString tag : tags) {
        uint h = hashTag(tag);
        if (!edgeTags.contains(h))
            edgeTags[h] = tag;
        edge->addTag(tag);
    }
    edge->setStartNode(ns);
    edge->setEndNode(ne);
    ns->addOutEdge(edge);
    ne->addInEdge(edge);

    emit edgeAdded( ns, ne, edge.data() );
    return edge;
}

Node* GraphModel::resolveNode(const QList<int> &addr) const
{
    auto it = addr.cbegin();
    Node *n = const_cast<Node*>(_root);
    while (n && it != addr.cend())
        n = n->childNodes().value(*it++);

    return n;
}

QModelIndex GraphModel::index(const QList<int> &addr) const
{
    auto it = addr.cend();
    QModelIndex idx = QModelIndex();
    while (it-- != addr.cbegin()) {
        idx = childIndex(*it, idx);
    }
    return idx;
}
/**
 * @brief Конструктор.
 *
 * При создании модели из C++ необходимо вызывать метод componentComplete()
 */
GraphModel::GraphModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    connect(this, &GraphModel::requestInitCollection, this, &GraphModel::onRequestCollectionInit, Qt::QueuedConnection);
    connect(this, &GraphModel::requestDeferredMetaNodeMember, this, &GraphModel::onRequestDeferredMetaNodeMember, Qt::QueuedConnection);
    clear();

}

GraphModel::~GraphModel()
{}

/*!
    \fn void GraphModel::initModel(Node *root)
    \brief Инициализация модели.
*/
void GraphModel::initModel(Node *root)
{
    if (_root) {
        for (Node *n:_root->m_childNodes) {
            if (n != root) {
                removeNodeId(n);
                delete n;
            }
        }
        _root->reset();
        delete _root;
    }
    if (root) {
        node2nid.remove(_root); // it's safe!
        _root = root;
        _root->m_parent = nullptr;
    } else {
        node2nid.clear();
        _root = new RootNode();
        _root->m_childNodes.reserve(4);
    }
    node2nid[_root].id = "";
    //    qDebug() << "model initialized" << mute;
}

QHash<int, QByteArray> GraphModel::extractRoles(int metaTypeId, const QMetaObject *mo, QHash<int, QByteArray> *propertyTypes, QString rolePrefix)
{
    QHash<int, QByteArray> ret;
    if (rolePrefix.isEmpty())
        rolePrefix = QString(QMetaType::typeName(metaTypeId)).toLower();
    for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
        QMetaProperty p = mo->property(i);
        int role = getRole(metaTypeId, i - mo->propertyOffset());
        if (propertyTypes)
            propertyTypes->insert(role, p.typeName());
        QByteArray roleName = QString("%1$%2").arg(rolePrefix).arg(p.name()).toLocal8Bit();
        ret[role] = roleName;
    }
    return ret;
}

QHash<int, QByteArray> GraphModel::roleNames() const
{
    //    qDebug() << "requested rolenames";
    QHash<int, QByteArray> ptypes;
    QHash<int, QByteArray> ret;
    if (m_status != Ready)
        return ret;
    ret[RowRole] = "rowIndex";
    ret[AddressRole] = "address";
    ret[DataRole] = "data";
    ret[MRLRole] = "mrl";
    ret[GraphIndexRole] = "graphIndex";
    ret[HandleRole] = "handle";
    ret[ChildCountRole] = "childrenCount";
    ret[ChildrenRole] = "childrenModel";
    ret[InCountRole] = "inCount";
    ret[OutCountRole] = "outCount";
    ret[NodeTypeRole] = "typeName";
    ret[NodeIdRole] = "nodeId";
    ret[AgentRole] = "object";
    //    int w = 0;
    //    for (int t = LastNodeRole; t > 0; ++w, t >>= 1 ) {}
    //    int fRole = 1 << w;

    auto it = node2handle.cbegin();
    while (it != node2handle.end()) {
        int htid = it.value();
        auto mo = QMetaType::metaObjectForType(htid);
        Q_ASSERT_X(mo != nullptr, "roleNames", "type must have Q_OBJECT or Q_GADGET macro");
        ret.unite(extractRoles(it.key(), mo, &ptypes, QString(QMetaType::typeName(it.key())).toLower()));
        //        for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
        //            QMetaProperty p = mo->property(i);
        //            int role = getRole(it.key(), i - mo->propertyOffset());
        //            ptypes[role] = p.typeName();
        //            QByteArray roleName = QString("%1$%2").arg().arg(p.name()).toLocal8Bit();
        //            //            ret[fRole++] = roleName;
        //            ret[role] = roleName;
        //            //            qDebug() << "Added role" << roleName << "id" << role;
        //        }

        ++it;
    }
    if (agentData.build(ret, ptypes, this)) {
        for (auto it = agents.begin(); it != agents.end(); ++it) {
            if (!it.value().isNull()) {
                qDebug() << "deleted agent for node" << getNodeId(it.key());
                it.value()->deleteLater();
            }
        }
        agents.clear();
    }
    return ret;
}

QModelIndex GraphModel::index(int row, int column, const QModelIndex &parent) const
{
    Node *p = getNode(parent);
    Q_ASSERT(p != nullptr);

    if (row >= 0 && row < p->childNodes().size()) {
        Node *n = p->childNodes().at(row);
        return createIndex(row, 0, n);
    }
    qWarning() << "requested non-existent index" << row << column << parent << p->childCount() << getMRL(parent);
    return QModelIndex();
}

QMap<int, QVariant> GraphModel::itemData(const QModelIndex &index) const
{
    Node *n = getNode(index);
    Q_ASSERT(n != nullptr);
    QMap<int, QVariant> ret;
    ret[NodeIdRole] = getNodeId(n);
    ret[DataRole] = n->data();
    //    ret[ChildCountRole]
    qDebug() << "Requested itemdata" << index << ret;
    return ret;
}

QModelIndex GraphModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    Node *n = getNode(child);
    Q_ASSERT(n != nullptr);
    if (n == _root) {
        return QModelIndex();
    }
    if (!n->parent()) {
        qWarning() << "Deleted Node index" << child;
        return QModelIndex();
    }

    if (n->parent() == _root)
        return QModelIndex();
    Node *p = n->parent();
    if (!p->parent()) {
        qDebug() << "Fehler";
        return QModelIndex();
    }
    return createIndex(p->row(), 0, p);
}

QModelIndex GraphModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (column != 0 || row < 0)
        return QModelIndex();
    Node *n = getNode(idx);
    Node *p = n->parent();
    if (p && row < p->childCount())
        return getNodeIndex(p->childNodes().at(row));
    return QModelIndex();
}

int GraphModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return _root->childCount();
    Node *n = getNode(parent);
    return n->childNodes().size();
}

int GraphModel::columnCount(const QModelIndex &idx) const
{
    if (idx.isValid()) {
        if (idx.internalPointer() != nullptr && idx.model() == this)
            return 1;
        return 0;
    }
    return 1;
}

QVariant GraphModel::data(const QModelIndex &index, int role) const
{
    Node *n = getNode(index);
    Q_ASSERT(n != nullptr);
    if (role > LastNodeRole) {
        int ntype;
        int prop_idx;
        if (!roleToNodeProperty(role, &ntype, &prop_idx)) {
            qWarning() << "unable to get data by role" << role;
            return QVariant();
        }
        int nodeType = n->typeId();
        if (ntype != nodeType) {
            qWarning() << "getData for role" << role << "error: unable to read properties of" << QString(QMetaType::typeName(ntype)) << "on" << QString(QMetaType::typeName(nodeType));
            return QVariant();
        }
        int h_tid = node2handle.value(ntype);
        //        qDebug() << "Requested handle property" << hPropId << role << QMetaType::typeName(h_tid);
        auto mo = QMetaType::metaObjectForType(h_tid);
        QScopedPointer<NodeHandle> handle(handleForIndex(index, h_tid));
        //        qDebug() << "reading property" << hPropId << mo->property(hPropId).name() << mo->property(hPropId).typeName() << "on" << mo->className();
        return mo->property(prop_idx + mo->propertyOffset()).readOnGadget(handle.data());
        //        return QVariant();
        //        return mo->property()
    }
    //    qDebug() << "Query data" << address(index) << roleNames().value(role);
    switch (role) {
    case RowRole:
        if (n->parent())
            return n->row();
        return -1;
    case AddressRole:
        return QVariant::fromValue(iPath(index));
    case DataRole:
        return n->data();
    case MRLRole:
        return getMRL(index);
    case GraphIndexRole: {
        return QVariant::fromValue(GraphIndex(index));
        //        QVariant var = QVariant::fromValue(ret);
        //        qDebug() << var.typeName();
        //        return var;
    }
    case HandleRole:
        return getHandle(index);
    case ChildCountRole:
        return n->childNodes().size();
    case OutCountRole:
        return n->outEdges().size();
    case InCountRole:
        return n->inEdges().size();
    case NodeTypeRole:
        return QString(QMetaType::typeName(n->typeId()));
    case NodeIdRole:
        return getNodeId(n);
    case AgentRole:
        return QVariant::fromValue(get(index));
    case ChildrenRole:
        return QVariant::fromValue(getSubModel(index));
    default:
        break;
    }
    return QVariant();
}

bool GraphModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Node *n = getNode(index);
    Q_ASSERT(n != nullptr);
    if (role > LastNodeRole) {
        int ntype;
        int prop_idx;
        if (!roleToNodeProperty(role, &ntype, &prop_idx)) {
            qWarning() << "unable to set data by role" << role;
            return false;
        }
        int nodeType = n->typeId();
        if (ntype != nodeType) {
            qWarning() << "Role" << role << "is for node type" << QMetaType::typeName(ntype) << "provided node type" << QMetaType::typeName(nodeType);
            return false;
        }
        int h_tid = node2handle.value(ntype);
        auto mo = QMetaType::metaObjectForType(h_tid); // TODO: cache cache handle metaobjects in factory
        QScopedPointer<NodeHandle> handle(handleForIndex(index, h_tid));
        prop_idx += mo->propertyOffset();
        QMetaProperty prop = mo->property(prop_idx);
        bool result = prop.writeOnGadget(handle.data(), value);
        emit dataChanged(index, index, { role });
//        qDebug() << "write property" << prop_idx << prop.name() << value << result << handle->mrl();
        return result;
    }
    return false;
}

void GraphModel::reparentNode(const Node *node, const Node *oldParent, const Node *newParent)
{
    if (node2nid.contains(node) && node->parent() == oldParent) {
        NId &old_pid = node2nid[oldParent];
        NId &new_pid = node2nid[newParent];
        NId &id = node2nid[node];
        old_pid.children.remove(id.id);
        new_pid.children.insert(id.id, node);
        //        debugPrint << "reparent node" << id.id << "from" << old_pid.id << "to" << new_pid.id << node->row();
    }
}

void GraphModel::moveSelection(const QItemSelection &selection, const QModelIndex &destParentIndex, int destIndex)
{
    if (destIndex < 0)
        destIndex = rowCount(destParentIndex);
    for (QItemSelectionRange range : selection) {
        qDebug() << "moving range size" << range.height() << "at" << getNodeId(range.parent()) << "to" << getNodeId(destParentIndex) << "at" << destIndex;
        moveRows(range.parent(), range.top(), range.height(), destParentIndex, destIndex);
        destIndex += range.height();
    }
}

void GraphModel::moveNode(Node *node, Node *destParent, int destIndex)
{
    QModelIndex sourceIdx = getNodeIndex(node->parent());
    QModelIndex destIdx = getNodeIndex(destParent);
    int src = node->row();
    if (destIndex < 0)
        destIndex = destParent->childCount();
    moveRow(sourceIdx, src, destIdx, destIndex);
    bool insert = destIndex > 0;
    return;
    //    bool canmove = beginMoveRows(parentIdx, src, src, destParentIdx, destIndex);
    //    if (!canmove) {
    //        qWarning() << "can not move node" << getMRL(getNodeIndex(node)) << "to new parent" << getMRL(destParentIdx) << "index" << destIndex;
    //        return;
    //    }
//    if (node->parent() == destParent) {
//        beginMoveRows(getNodeIndex(node->parent()), src, src, getNodeIndex(destParent), destIndex);
//        destIndex--;
//        node->parent()->moveChild(src, destIndex);
//        endMoveRows();
//        qDebug() << "moved inside parent from" << src << "to" << destIndex;
//        return;
//    }
//    QModelIndex parentIdx = getNodeIndex(node->parent());
//    qDebug() << "removing from parent" << getMRL(parentIdx) << "row" << src;
//    beginRemoveRows(parentIdx, src, src);

//    QModelIndex destParentIdx = getNodeIndex(destParent);
//    beginInsertRows(destParentIdx, destIndex, destIndex);
//    reparentNode(node, node->parent(), destParent);
//    //    node->setParent();
//    if (insert) {
//        qDebug() << "inserting" << destIndex << destParent->childCount();
//        destParent->insertChild(destIndex, node);
//    } else if (destIndex == destParent->childCount()) {
//        qDebug() << "appending" << destIndex << destParent->childCount();
//        destParent->appendChild(node);
//    } else {
//        qDebug() << "prepending" << destIndex << destParent->childCount();
//        destParent->prependChild(node);
//    }
//    //    endMoveRows();
//    qDebug() << "new row id" << node->row();
//    endInsertRows();
//    endRemoveRows();
//    qDebug() << "inserting to parent" << getMRL(destParentIdx) << "row" << destIndex << destParentIdx;


//    notify(getNodeIndex(node), { GraphModel::MRLRole, GraphModel::HandleRole, GraphModel::AddressRole });
//    notify(getNodeIndex(destParent), { GraphModel::ChildCountRole, GraphModel::HandleRole });
//    notifyTreeIdChange(node);
}

/*!
 \brief Возвращает индекс узла \a idx в дереве в виде "0.1.2.3". Точками разделены индексы элемента, начиная с корневого узла.
*/
QString GraphModel::address(QModelIndex idx) const
{
    QStringList ret;
    for (int i:iPath(idx)) {
        ret.append(QString::number(i));
    }
    return ret.join(".");
}

QList<int> GraphModel::iPath(QModelIndex idx) const
{
    return iPathRelative(idx);
}

QModelIndex GraphModel::childIndex(int n, const QModelIndex &parent) const
{
    return index(n, 0, parent);
}

QVariant GraphModel::getRootHandle() const
{
    return getHandle(_root);
}

QVariant GraphModel::getHandle(const QString &mrl) const
{
    if (mrl.isEmpty())
        return QVariant();

    //    qDebug() << "return" << QMetaType::typeName(nid.node->typeId()) << nid.node->row();
    return getHandle(getNode(mrl));
}

QVariant GraphModel::getHandle(const QModelIndex &idx) const
{
    if (!idx.isValid())
        return QVariant::fromValue(NodeHandle());
    Node *n = getNode(idx);
    if (!n)
        return QVariant();
    int h_tid = node2handle.value(n->typeId());
    QScopedPointer<NodeHandle> h(handleForIndex(idx, h_tid));
    QVariant ret(h_tid, h.data());
    return ret;
}

QVariant GraphModel::getHandle(QList<int> path) const
{
    auto idx = index(path);
    return getHandle(idx);
}



bool GraphModel::initCollection(Collection2 *collection, Type *type)
{
    const QMetaObject *mo = QMetaType::metaObjectForType(type->metaTypeId());
    if (!mo) {
        qWarning() << "no metaobject found for type" << type->metaTypeId() << type->className();
        return false;
    }
    QModelIndex idx = getNodeIndex(collection);
    collection->initModel(idx, mo, this);
    proxies.insert(collection, QPointer<NodeChildrenModel>(collection->cmodel()));
    return true;
}

QQmlListProperty<QObject> GraphModel::qmlChildren() {
    return QQmlListProperty<QObject>(this, 0,
                                     &GraphModel::qmlChildrenAppend,
                                     &GraphModel::qmlChildrenCount,
                                     &GraphModel::qmlChildrenAt,
                                     QQmlListProperty<QObject>::ClearFunction());
}

void GraphModel::qmlChildrenAppend(QQmlListProperty<QObject> *list, QObject *object)
{
    GraphModel *o = qobject_cast<GraphModel*>(list->object);
    object->setParent(o);
    emit o->childrenChanged();
    NodeFactory *f = qobject_cast<NodeFactory*>(object);
    if (f && !o->factories.contains(f->nodeMetatypeId())) {
        o->factories.insert(f->nodeMetatypeId(), f);
        emit o->nodeFactoriesChanged();
        return;
    }
    //    AbstractModule *mod = qobject_cast<AbstractModule*>(object);
    NodeChildrenModel *m = qobject_cast<NodeChildrenModel*>(object);
    if (m) {
        //        o->m_proxies.insert()
        m->setModel(o);
        return;
    }
    ModelWatcher *w = qobject_cast<ModelWatcher*>(object);
    if (w) {
        w->setModel(o);
        return;
    }
}

int GraphModel::qmlChildrenCount(QQmlListProperty<QObject> *list)
{
    GraphModel *o = qobject_cast<GraphModel*>(list->object);
    return o->children().size();
}

QObject *GraphModel::qmlChildrenAt(QQmlListProperty<QObject> *list, int index)
{
    GraphModel *o = qobject_cast<GraphModel*>(list->object);
    return o->children().at(index);
}

int getRole(int nodeType, int propertyId)
{
    return ((nodeType - QMetaType::User) << 8) + propertyId;
}

bool roleToNodeProperty(int role, int *nodeTypeId, int *propertyId)
{
    int ntyp = (role >> 8);
    if (!ntyp)
        return false;
    ntyp += 1024;
    if (nodeTypeId)
        *nodeTypeId = ntyp;
    if (propertyId)
        *propertyId = role & 0xff;
    return true;
}

NodeChildrenModel *GraphModel::getSubModel(QModelIndex idx) const
{
    Node *node = getNode(idx);
    if (!node)
        return nullptr;
    QPointer<NodeChildrenModel> &h = proxies[node];
    if (h.isNull()) {
        //    debugPrint << "Create new submodel for" << address(idx);
        h = new NodeChildrenModel();
        h->setRootIndex(idx);
        qmlEngine(this)->setObjectOwnership(h.data(), QQmlEngine::JavaScriptOwnership);
    }
    return h.data();
}

NodeChildrenModel *GraphModel::getSubModel(const QString &mrl) const
{
    QVariant h = getHandle(mrl);
    if (h.isValid())
        return getSubModel(h.value<NodeHandle>().modelIndex());
    return nullptr;
}

ModelAgent *GraphModel::get(const QModelIndex &idx) const
{
    Node *node = getNode(idx);
    if (!node)
        return nullptr;
    QPointer<ModelAgent> &h = agents[node];
    if (h.isNull()) {
        h = new ModelAgent(idx, agentData);
        //                qDebug() << "created new ModelObject for" << getNodeId(node);
        //        qmlEngine(this)->setObjectOwnership(h.data(), QQmlEngine::CppOwnership);
        qmlEngine(this)->setObjectOwnership(h.data(), QQmlEngine::JavaScriptOwnership);
    }
    return h.data();
}
/**
 * @brief Устанавливает имя/идентификатор узла.
 *
 * Опциональный параметр \c parent используется, когда необходимо установить имя для узла,
 * который ещё не был добавлен в модель.
 * @param node
 * @param id
 * @param parent
 */
void GraphModel::setNodeId(const Node *node, QString id, const Node *parent)
{
    //        qDebug() << "assigning ID" << id << "to node" << address(getNodeIndex(node));
    if (id.isEmpty())
        return;

    const Node *p = parent ? parent : node->parent();
    if (!p) {
        qWarning() << "Node has no parent! Unable to set id" << id;
        return;
    }
    if (!node2nid.contains(p)) {
        if (!p->parent()) {
            qDebug() << "parent of node" << node << "has no id: root";
            node2nid[p].id = QString("Node%1").arg(QString::number((quintptr)node, 16).prepend("0x"));
        } else {
            qDebug() << "parent of node" << address(getNodeIndex(node)) << "has no id: creating implicit" << p->row();
            setNodeId(p, QString::number(p->row()));
        }
        //        debugPrint << "unable to set id" << id << "for Node" << address(getNodeIndex(node)) << "— its parent has no id";
        //        return;
    }
    NId &node_id = node2nid[node];
    NId &parent_id = node2nid[p];

    if (parent_id.children.contains(id)) {
        if (parent_id.children.value(id) != node)
            qWarning() << "node" << parent_id.id << "already has child" << id;
        return;
    }
    if (!node_id.id.isEmpty()) {
        parent_id.children.remove(node_id.id);
    }
    parent_id.children.insert(id, node);
    node_id.id = id;
    //    qDebug() << "parent" << parent_id.id << "child ids" << parent_id.children.keys();

    QModelIndex nidx = getNodeIndex(node);
    if (mute || parent)
        return;
    notify(nidx, {NodeIdRole, HandleRole, MRLRole} );
    for (const Node *n : node_id.children.values()) {
        notifyTreeIdChange(n);
    }
}

QString GraphModel::getNodeId(const Node *node) const
{
    return node2nid.value(node).id;
}

QString GraphModel::getMRL(QModelIndex idx) const
{
    QStringList ret;
    Node *n = getNode(idx);
    while (n) {
        if (node2nid.contains(n))
            ret.prepend(node2nid.value(n).id);
        else
            ret.prepend(QString::number(n->row()));
        n = n->parent();
    }
    return ret.join("/");
}

bool GraphModel::hasId(Node *node) const
{
    if (!node)
        return false;
    return node2nid.contains(node);
}
/**
 * @brief Уведомляет модель об изменениях данных
 *
 * Следует использовать данный метод, а не использовать сигнал dataChanged. Этот метод учитывает режим работы модели.
 *
 * @param index индекс узла
 * @param roles список изменившихся ролей
 */
void GraphModel::notify(const QModelIndex &index, const QVector<int> &roles)
{
    if (m_status != Ready) {
        if (index == incubatedNode) {
            //            qDebug() << "suppressed signal";
            return;
        }
    }
    emit dataChanged(index, index, roles);
}

NodeFactory *GraphModel::factory(Node *node) const
{
    return factories.value(node->typeId());
}

NodeFactory *GraphModel::factory(int typeId) const
{
    return factories.value(typeId);
}

QVariantList GraphModel::nodeFactories() const
{
    QVariantList ret;
    QHash<int, NodeFactory*>::const_iterator it = factories.cbegin();
    while (it != factories.cend()) {
        QVariantMap m;
        m["type"] = QString(QMetaType::typeName(it.key()));
        m["factory"] = QVariant::fromValue(it.value());
        ret.append(m);
        ++it;
    }
    return ret;
}
/**
 * @brief clear очищает модель
 */
void GraphModel::clear()
{
    beginResetModel();
    initModel();
    endResetModel();
}


void GraphModel::addEdge(int indexFrom, int indexTo, QString tag, QVariant data,  NodeHandle parentFrom, NodeHandle parentTo)
{
    QPersistentModelIndex st(index(indexFrom, 0, parentFrom.modelIndex()));
    QPersistentModelIndex e(index(indexTo, 0, parentTo.modelIndex()));
    createEdge(st, e, tag.split("|") , data);
    if (mute)
        return;
    notify(st, { HandleRole, OutCountRole });
    notify(e, { HandleRole, InCountRole });
}

void GraphModel::addEdge(const NodeHandle &srcNode, const NodeHandle &destNode, QString tag, QVariant data)
{
    //    qDebug() << "adding edge" << srcNode.nodeId() << destNode.nodeId();
    addEdge(srcNode.modelIndex(), destNode.modelIndex(), tag, data);
}

//void GraphModel::dumpChildren(const QModelIndex &parent, bool recursive, int first, int last) const
//{
//    Node *node = getNode(parent);
//    qDebug() << "dump children" << first << last << node << getNodeId(node) << node->childCount();
//    int d = node->depth() + 1;
//    if (last < 0)
//        last = node->childCount();
//    for (int i = first; i < last; ++ i) {
//        Node *n = node->childNodes().at(i);
//        QString line;
//        QTextStream s(&line);
//        s.setFieldAlignment(QTextStream::AlignLeft);
//        s << QByteArray(d * 2, ' ') << qSetFieldWidth(30) << getNodeId(n) << qSetFieldWidth(5) << n->row() << n->childCount();
//        qDebug() << qPrintable(line);
//        if (recursive && n->childCount() > 0) {
//            dumpChildren(getNodeIndex(n), recursive);
//        }
//    }
//}

void GraphModel::setStatus(GraphModel::ModelState status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged(status);
}

void GraphModel::addEdge(const QModelIndex &srcNode, const QModelIndex &destNode, QString tag, QVariant data)
{
    QWeakPointer<Edge> edge = createEdge(srcNode, destNode, tag.split('|'), data);
    if (mute)
        return;
    emit outEdgeAdded(srcNode, edge);
    emit dataChanged(srcNode, srcNode);
    emit dataChanged(destNode, destNode);
}

void GraphModel::beginRemoveEdge(uint uid)
{
    destroyingEdges.insert(uid);
    emit edgeAboutToBeRemoved(uid);
}

void GraphModel::endRemoveEdges()
{
    emit edgesRemoved(destroyingEdges.toList());
    destroyingEdges.clear();
}

QModelIndex GraphModel::appendNode(Node *node, const QModelIndex &parent)
{
    //        qDebug() << "adding node" << QMetaType::typeName(node->typeId()) << "to parent" << getNodeId(parent);
    Q_ASSERT(node->parent() == nullptr);
    Node *p = getNode(parent);
    const int cnt = p->childCount();
    beginInsertRows(parent, cnt, cnt);
    p->appendChild(node);
    endInsertRows();
    auto idx = createIndex(cnt, 0, node);
    //    factory<Node*>();
    notify(parent, { ChildCountRole });
    return idx;
}

QModelIndex GraphModel::prependNode(Node *node, const QModelIndex &parent)
{
    Q_ASSERT(node->parent() == nullptr);
    Node *p = getNode(parent);
    //    qDebug() << "Adding node" << QMetaType::typeName(node->dataType());
    beginInsertRows(parent, 0, 0);
    p->prependChild(node);
    endInsertRows();
    auto idx = createIndex(0, 0, node);
    notify(parent, { ChildCountRole });
    return idx;
}

QModelIndex GraphModel::insertNode(int pos, Node *node, QModelIndex parent)
{
    Q_ASSERT(node->parent() == nullptr);
    Node *p = getNode(parent);
    const int cnt = p->childCount();
    if (pos > cnt)
        pos = cnt;
    if (pos < 0)
        pos = 0;

    beginInsertRows(parent, pos, pos);
    p->insertChild(pos, node);
    endInsertRows();
    auto idx = createIndex(pos, 0, node);
    notify(parent, { ChildCountRole });
    return idx;
}

QModelIndex GraphModel::beginAppendNode(Node *node, const QModelIndex &parent)
{
    Q_ASSERT(node->parent() == nullptr);
    Node *p = getNode(parent);
    const int cnt = p->childCount();
    beginInsertRows(parent, cnt, cnt);
    p->appendChild(node);
    auto idx = createIndex(cnt, 0, node);
    incubatedNode = idx;
    setStatus(CreatingNode);
    return idx;
    //    emit dataChanged(parent, parent, { ChildCountRole, HandleRole });
}

QModelIndex GraphModel::beginPrependNode(Node *node, const QModelIndex &parent)
{
    Q_ASSERT(node->parent() == nullptr);
    Node *p = getNode(parent);
    beginInsertRows(parent, 0, 0);
    p->prependChild(node);
    auto idx = createIndex(0, 0, node);
    incubatedNode = idx;
    setStatus(CreatingNode);
    return idx;
}

QModelIndex GraphModel::beginInsertNode(int pos, Node *node, QModelIndex parent)
{
    Q_ASSERT(node->parent() == nullptr);
    Node *p = getNode(parent);
    const int cnt = p->childCount();
    if (pos > cnt)
        pos = cnt;
    if (pos < 0)
        pos = 0;

    beginInsertRows(parent, pos, pos);
    p->insertChild(pos, node);
    auto idx = createIndex(pos, 0, node);
    incubatedNode = idx;
    setStatus(CreatingNode);
    return idx;
}
/**
 * @brief Завершает транзакцию добавления узла.
 */
void GraphModel::commit()
{
    if (m_status != CreatingNode) {
        qWarning() << "Model was not in CreatingNode mode";
        return;
    }
    setStatus(Ready);
    endInsertRows();
    QModelIndex parent = incubatedNode.parent();
    emit dataChanged(parent, parent, { ChildCountRole });
    incubatedNode = QModelIndex();
}
/**
 * @brief Текущий режим модели
 */
GraphModel::ModelState GraphModel::status() const
{
    return m_status;
}

void GraphModel::onRequestCollectionInit(Collection *collection, bool stored)
{
    qDebug() << "onRequestCollectionInit...";
    for( auto e: collection->outEdges() ) {
        if( e->hasTag("collection") && e->hasTag("type") ) {
            Type* t = dynamic_cast<Type*>( e->endNode() );
            if( t != nullptr ) {
                qDebug() << "Collection has edge pointing to type...";
                if( t->metaTypeId() != QMetaType::UnknownType && QMetaType::isRegistered( t->metaTypeId() ) && QMetaType::metaObjectForType( t->metaTypeId()) != nullptr ) {
                    qDebug() << "...registered type, initializing collection";
                    collection->m_metaobject = const_cast<QMetaObject*>( QMetaType::metaObjectForType( t->metaTypeId() ) );
                    collection->m_cmodel = QSharedPointer<CollectionModel>( new CollectionModel( getNodeIndex(collection), collection->m_metaobject ), &CollectionModel::deleteLater );
                    collection->typeRegistered(true);

                    auto index = getNodeIndex(collection);
                    notify( index, { GraphModel::NodeRoles::HandleRole } );
                    notify( index, { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_className) });
                    notify( index, { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_metaTypeId) });
                    if( !stored ) //collection->restore();
                        getHandle( index ).value<CollectionHandle>().restore();

                } else {
                    qDebug() << "...not a registered type";
                    collection->m_cmodel.clear();
                    collection->m_metaobject = nullptr;
                }
                break;
            }
        }else{
            qDebug() << "Collection has no Collection to Type edges. skipping...";
        }
    }
    qDebug() << "...onRequestCollectionInit done";
}
/**
 * @brief Возвращает указатель на узел по индексу
 *
 * Если индекс не валидный, возвращается указатель на корневой узел.
 */
Node *GraphModel::getNode(const QModelIndex &idx) const
{
    if (idx.isValid())
        return reinterpret_cast<Node*>(idx.internalPointer());
    return const_cast<Node*>(_root);
}
/**
 * @brief Возвращает указатель на узел по адресу \c mrl
 *
 * Если узла с таким mrl в модели нет, возвращается нулевой указатель.
 */
Node *GraphModel::getNode(const QString &mrl) const
{
    QStringList path = mrl.split('/', QString::SkipEmptyParts);
    NId nid = node2nid.value(_root);
    const Node *node = _root;
    while (!path.isEmpty()) {
        QString pe = path.takeFirst();
        node = nid.children.value(pe, nullptr);
        if (node)
            nid = node2nid.value(nid.children[pe]);
        else
            return nullptr;
    }
    return const_cast<Node*>(node);
}

Node *GraphModel::getNode(const Node *parent, const QString &id) const
{
    if (node2nid.contains(parent) && node2nid.value(parent).children.contains(id))
        return const_cast<Node*>(node2nid.value(parent).children.value(id));

    return const_cast<Node*>(_root);
}
/**
 * @brief Присваивает имя узлу
 *
 * Если узлу был присвоен идентификатор, то он заменяется. Всем дочерним узлам рассылается уведомление об изменении их адреса.
 */
void GraphModel::setNodeId(const QModelIndex &idx, QString id)
{
    if (!idx.isValid()) {
        return;
    }
    Node *n = getNode(idx);
    setNodeId(n, id);
}
/**
 * @brief Возвращает ссылку для узла
 * @param node указатель на узел
 * @return \c QVariant, содержащий NodeHandle соответствующего типа
 */
QVariant GraphModel::getHandle(const Node *node) const
{
    if (!node)
        return QVariant();
    if (node == _root)
        return getHandle(createIndex(0, 0, _root));
    return getHandle(getNodeIndex(node));
}

//void GraphModel::onRequestDeferredMetaNodeMember(MetaNode *node, const QModelIndex &idx)
//{
//    Node *c = getNode(idx);
//    QModelIndex p = getNodeIndex(node);
//    debugPrint << "Adding" << GraphModel::address(getNodeIndex(c)) << QMetaType::typeName(c->typeId()) << "to metanode" << GraphModel::address(getNodeIndex(node));
//    appendNode(c, p);
//}
void GraphModel::onRequestDeferredMetaNodeMember(MetaNode *node, const QList<int>& addr, QPersistentModelIndex rootindex)
{
    //    auto mnindex = getNodeIndex(node);
    //    Node *c = getNode(iPathToIndex(addr, rootindex));
    QModelIndex p = getNodeIndex(node);
    //    debugPrint << "Adding" << GraphModel::address(getNodeIndex(c)) << QMetaType::typeName(c->typeId()) << "to metanode" << GraphModel::address(getNodeIndex(node));
    //    node->appendMember(c);
    //    node->appendChild( QPersistentModelIndex(iPathToIndex(addr, rootindex) ) );
    if (!mute)
        emit dataChanged( p, p, { HandleRole } );
    //    appendNode(c, p);
}

NodeHandle *GraphModel::handleForIndex(const QModelIndex &index, int handleMetaTypeId) const
{
    NodeHandle* h = reinterpret_cast<NodeHandle*>(QMetaType::create(handleMetaTypeId));
    h->d = QPersistentModelIndex(index);
    return h;
}
/**
 * @brief Удаляет идентифиактор узла
 *
 * Если узел содержит дочерние узлы, они так же становятся безымянными. Все они получают уведомление об изменении имени и адреса.
 * @param node
 */
void GraphModel::removeNodeId(const Node *node)
{
    if (!node2nid.contains(node))
        return;
    for (const Node *n : node2nid.value(node).children.values()) {
        QModelIndex idx = getNodeIndex(n);
        if (!mute)
            notify(idx, { NodeIdRole, HandleRole, MRLRole });
        removeNodeId(n);
    }
    node2nid.remove(node);
}

void GraphModel::notifyTreeIdChange(const Node *node)
{
    QModelIndex idx = getNodeIndex(node);
    notify(idx, { MRLRole, HandleRole });
    NId id = node2nid.value(node);
    for (const Node *n : id.children.values())
        notifyTreeIdChange(n);
}

void GraphModel::exportFactories(QDataStream &out, Node *node) const
{
    QSet<int> nodetypes = collectTypes(node);
    out << nodetypes.size();
    for (int ntype : nodetypes) {
        if (!factories.contains(ntype)) {
            qDebug() << "Factory" << QMetaType::typeName(ntype) << "does not exist";
        } else {
            out << ntype << QByteArray(QMetaType::typeName(ntype)) << QByteArray(factories[ntype]->metaObject()->className());
        }
        //        auto mo =
    }
}

/**
 * @brief Определяет набор типов узлов в поддереве \c rootNode
 */
QSet<int> GraphModel::collectTypes(Node *rootNode) const
{
    QSet<int> ret;
    ret.insert(rootNode->typeId());
    for (Node *child : rootNode->m_childNodes) {
        ret.unite(collectTypes(child));
    }
    return ret;
}

/**
 * @brief Возвращает уникальное имя узла среди дочерних \c parentNode.
 *
 * Позволяет избежать дубликатов имён при импорте и редактировании модели, добавляя не уникальным идентификаторам суффикс.
 *
 * @param desiredId желаемое имя узла
 * @param parentNode родительский узел
 */
QString GraphModel::getUniqueNodeId(QString desiredId, const Node *parentNode) const
{
    if (!node2nid.contains(parentNode))
        return desiredId;
    const NId &id = node2nid[parentNode];
    if (!id.children.contains(desiredId))
        return desiredId;
    int i = 1;
    QString ret;
    do {
        ret = QString("%1_%2").arg(desiredId).arg(i++);
    } while(id.children.contains(ret));
    return ret;
}

/**
 * @brief Преобразует \c QUrl в имя файла
 *
 * Учитывается, что в \c url может передаваться как ссылка на файл (\a file://), так и на ресурс (\a qrc://).
 * В опциональном аргументе \c file возвращается короткое имя файла без пути и расширения.
 * @param url
 * @param file короткое имя файла
 * @return имя файла, пригодное для \c QFile
 */
QString GraphModel::urlToFilename(const QUrl &url, QString *file) const
{
    QString fname;
    if (url.scheme() == "qrc")
        fname = url.path().prepend(":");
    else if (url.scheme() == "file")
        fname = url.toLocalFile();
    else {
        fname = url.toString();
        //        qDebug() << "can't resolve filename from url" << qPrintable(url.toString());
    }
    if (file) {
        *file = url.fileName().section('.', 0, -2);
    }
    return fname;
}

QHash<int, int> GraphModel::importFactories(QDataStream &in) const
{
    QHash<int, int> ret;
    QHash<QByteArray, int> revHash;
    QHash<int, NodeFactory*>::const_iterator it = factories.cbegin();
    while (it != factories.cend()) {
        revHash.insert(QByteArray(QMetaType::typeName(it.value()->nodeMetatypeId())), it.key());
        ++it;
    }
    int c;
    in >> c;
    while (c--) {
        int file_tid;
        QByteArray nodeType;
        QByteArray factoryType;
        in >> file_tid >> nodeType >> factoryType;
        if (!revHash.contains(nodeType)) {
            in.setStatus(QDataStream::ReadCorruptData);
            qWarning() << "unknown node type" << nodeType << "factory" << factoryType;
            return ret;
        }
        ret.insert(file_tid, revHash.value(nodeType));
    }
    //    QDebug dbg = qDebug().maybeSpace().noquote();
    //    dbg << "imported" << ret.size() << "node types:";
    //    auto ii = ret.cbegin();
    //    while (ii != ret.cend()) {
    //        dbg << QString("[%1]").arg(QMetaType::typeName(ii.value()));
    //        ++ii;
    //    }
    return ret;
}

void GraphModel::exportNode(QDataStream &out, Node *node) const
{
    NodeFactory *f = factories.value(node->typeId());
    Q_ASSERT(f != nullptr);
    out << node->typeId();
    for (const QSharedPointer<Edge> &oe : node->outEdges())
        edgeset.insert(oe.data());
    f->exportNode(out, node);
}

QList<Edge *> GraphModel::exportEdges(QDataStream &out, const QModelIndex &root) const
{
    QList<Edge *> brokenLinks;
    out << edgeset.size();
    for (Edge *e : edgeset) {
        bool ok = true;
        QList<int> st = iPathRelative(getNodeIndex(e->startNode()), root, &ok);
        if (!ok)
            brokenLinks.append(e);
        out << ok << st;
        QList<int> end = iPathRelative(getNodeIndex(e->endNode()), root, &ok);
        if (!ok)
            brokenLinks.append(e);
        out << ok << end;
        out << e->tag();
        out << e->data();
        out << QSet<int>();
    }
    return brokenLinks;
}

QList<int> GraphModel::iPathRelative(QModelIndex idx, const QModelIndex &root, bool *valid) const
{
    QList<int> ret;
    while (idx.isValid() && idx != root) {
        ret.prepend(idx.row());
        idx = idx.parent();
    }
    if (valid && root.isValid())
        *valid = idx == root;
    return ret;
}

QModelIndex GraphModel::iPathToIndex(QList<int> addr, const QModelIndex &root) const
{
    QModelIndex idx = root;
    while (!addr.isEmpty()) {
        idx = childIndex(addr.takeFirst(), idx);
        if (!idx.isValid())
            return idx;
    }
    return idx;
}

Node *GraphModel::importNode(QDataStream &in, Node *parent, QString nodeId)
{
    int tid;
    in >> tid;
    if (!nodeTypeTable.contains(tid)) {
        in.setStatus(QDataStream::ReadCorruptData);
        qWarning() << "unknown node type" << tid ;
        return nullptr;
    }
    NodeFactory *f = factories.value(nodeTypeTable.value(tid));
    Q_ASSERT(f != nullptr);
    QScopedPointer<Node> node(f->createNode());
    parent->appendChild(node.data());
    if (!nodeId.isEmpty()) {
        setNodeId(node.data(), nodeId, parent);
    }
    //        appendNode(node, f->rootIndex());
    f->importNode(in, node.data());
    if (in.status() == QDataStream::Ok)
        return node.take();
    return nullptr;
}

QModelIndexList GraphModel::importEdges(QDataStream &in, const QModelIndex &root)
{
    QModelIndexList ret;
    int c;
    in >> c;
    //    qDebug() << "importing" << c << "edges to root" << getMRL(root);
    while(c--) {
        bool okst;
        bool okend;
        QList<int> st;
        QList<int> end;
        in >> okst >> st;
        in >> okend >> end;
        QString tag;
        QVariant data;
        QSet<int> types;
        in >> tag;
        in >> data;
        in >> types;
        if (okst && okend) {
            QModelIndex src = iPathToIndex(st, root);
            QModelIndex dst = iPathToIndex(end, root);
            //            qDebug() << "st" << st << "end" << end << resolveNode(st);
            addEdge(src, dst, tag, data);
        } else {
            if (!okst)
                ret.append(iPathToIndex(st));
            if (!okend)
                ret.append(iPathToIndex(end));
        }
    }
    return ret;
}

QJsonDocument GraphModel::dump(const QModelIndex &index)
{
    QJsonObject doc;
    edgeset.clear();
    Node *r = getNode(index);
    doc["root"] = dumpNode(r);
    QJsonArray edges;
    for (auto e : edgeset) {
        QJsonObject src;
        QJsonObject dst;
        Node *st = e->startNode();
        Node *end = e->endNode();
        src["mrl"] = getMRL(getNodeIndex(st));
        dst["mrl"] = getMRL(getNodeIndex(end));
        src["addr"] = address(getNodeIndex(st));
        dst["addr"] = address(getNodeIndex(end));
        QJsonObject edge;
        edge["startNode"] = src;
        edge["endNode"] = dst;
        QJsonArray tags;
        for (QString tag : e->tags()) {
            tags.append(tag);
        }
        edge["tags"] = tags;
        edges.append(edge);
    }
    doc["edges"] = edges;
    return QJsonDocument(doc);

}

QJsonValue GraphModel::dumpNode(const Node *n)
{
    if (!n) {
        return QJsonValue();
    }
    QJsonObject o;
    o["id"] = getNodeId(n);
    o["addr"] = address(getNodeIndex(n));
    o["type"] = QString(QMetaType::typeName(n->typeId()));
    o["data"] = QJsonValue::fromVariant(n->data());
    QJsonArray children;
    for (const Node *node : n->childNodes()) {
        children.insert(node->row(), dumpNode(node));
    }
    o["children"] = children;
    for (auto oe : n->outEdges()) {
        edgeset.insert(oe.data());
    }
    return o;
}
/**
 * @brief Возвращает список имён дочерних узлов
 * @param parent индекс родительского узла
 */
QVariantMap GraphModel::namedChildren(const QModelIndex &parent) const
{
    Node *node = getNode(parent);
    NId nid = node2nid.value(node);
    QVariantMap ret;
    auto it = nid.children.cbegin();
    while (it != nid.children.cend()) {
        ret.insert(it.key(), getHandle(it.value()));
        it++;
    }
    return ret;
}

bool GraphModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    Node *n = getNode(parent);
    NId &p = node2nid[n];
    for (int i = row; i < row + count; ++i) {
        Node *node = n->childNodes().at(i);
        if (node2nid.contains(node)) {
            p.children.remove(node2nid[node].id);
            node2nid.remove(node);
        }

        //        if (agents.contains(node)) {
        //            if (!agents[node].isNull()) {
        //                agents[node]->deleteLater();
        //            }
        //            agents.remove(node);
        //        }
    }
    qDebug() << "remove" << count << "rows from" << row << "in" << getNodeId(parent);

    n->removeChildren(row, count, this);

    endRemoveRows();
    return true;
}

bool GraphModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationRow)
{
    Node *oldParent = getNode(sourceParent);
    Node *newParent = getNode(destinationParent);

    if(destinationRow > newParent->childCount())
        destinationRow = newParent->childCount();
    if (sourceRow + count > oldParent->childCount()) {
        qDebug() << "too large range" << sourceRow << ":" << sourceRow + count - 1 << "childcount" << oldParent->childCount();
        return false;
    }
    if (isParentOf(destinationParent, index(sourceRow, 0, sourceParent))) {
        qDebug() << "cannot move to own child" << getMRL(destinationParent) << getMRL(sourceParent);
        return false;
    }
    qDebug() << "moving" << count << "nodes from" << getMRL(sourceParent) << "at" << sourceRow << "to" << getMRL(destinationParent) << "at" << destinationRow << newParent->childCount();
    NId &old_pid = node2nid[oldParent];
    NId &new_pid = node2nid[newParent];

    if (oldParent != newParent) {
        beginRemoveRows(sourceParent, sourceRow, sourceRow + count - 1);
//        beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationRow);
        int c = count;
        QList<Node*> nlist;
        while (c--) {
            Node *n = oldParent->takeChild(sourceRow);
//            if (agents.contains(n)) {
//                agents[n]->invalidate();
//            }
//            if (proxies.contains(n)) {
//                proxies[n]->deleteLater();
//                proxies.remove(n);
//            }
            n->m_parent = newParent;
            auto it = node2nid.find(n);
            if (it != node2nid.end()) {
                auto it2 = old_pid.children.find(it->id);
                QString id = getUniqueNodeId(it->id, newParent);
                old_pid.children.erase(it2);
                new_pid.children[id] = n;
            }
            nlist.append(n);
        }
        endRemoveRows();
        beginInsertRows(destinationParent, destinationRow, destinationRow + count - 1);
        if (destinationRow == newParent->childCount())
            newParent->m_childNodes.append(nlist);
        else for (Node *n : nlist) {
            newParent->m_childNodes.insert(destinationRow++, n);
        }
        newParent->fixIt();
        oldParent->fixIt();
        endInsertRows();
        for (Node *n : nlist) {
            QModelIndex idx = getNodeIndex(n);
            if (agents.contains(n)) {
                agents[n]->setModelIndex(idx);
            }
            notify(idx, { HandleRole, MRLRole, AddressRole, RowRole });
        }
        for (Node *n : newParent->childNodes()) {
            notify(getNodeIndex(n), { AddressRole, RowRole });
        }
        for (Node *n : oldParent->childNodes()) {
            notify(getNodeIndex(n), { AddressRole, RowRole });
        }
//        endMoveRows();
        notify(sourceParent, { HandleRole, ChildCountRole });
        notify(destinationParent, { HandleRole, ChildCountRole });

    } else {
//        qDebug() << "moving" << count << "rows from" << sourceRow << "to" << destinationRow << "in" << getNodeId(newParent);
        if (destinationRow <= (sourceRow + count) && destinationRow >= sourceRow)
            return false;
        beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationRow);
        oldParent->moveChildren(sourceRow, count, destinationRow);
        endMoveRows();
//        int i = 0;
        for (Node *n : newParent->childNodes()) {
            QModelIndex idx = getNodeIndex(n);
            notify(idx, { HandleRole, MRLRole, AddressRole, RowRole });
        }
    }

    return true;
}

void GraphModel::classBegin() {}

void GraphModel::componentComplete()
{
    //    auto e = qmlEngine(this);
    QQmlContext *ctx = qmlContext(this);
    qmlId = ctx->nameForObject(this);
    if (!qmlId.isEmpty())
        graphModels.insert(qmlId, this);

    setStatus(Ready);
}


QStringList GraphModel::mimeTypes() const
{
    QStringList ret = QAbstractItemModel::mimeTypes();
    ret.append("text/plain");
    return ret;
}

QMimeData *GraphModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug() << "asked for mimedata";
    QMimeData *ret = new QMimeData;
    if (indexes.size() > 1)
        ret->setText(QString("%1 nodes").arg(indexes.size()));
    else if (indexes.size() == 1)
        ret->setText(QString("node %1 cpp mime").arg(getNodeId(indexes.first())));
    else
        ret->setText("empty");
    return ret;
}

bool GraphModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    qDebug() << "Asked for can drop mimedata" << data->formats() << getMRL(index(row, column, parent));
    return true;
}

bool GraphModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    qDebug() << "dropped mimedata" << data->formats() << getMRL(index(row, column, parent));
    return true;
}

Qt::DropActions GraphModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::LinkAction | Qt::MoveAction;
}

Qt::DropActions GraphModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::LinkAction | Qt::MoveAction;
}
