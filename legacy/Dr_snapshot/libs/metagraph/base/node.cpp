#include "graphmodel.h"
#include "node.h"
#include "edge.h"
#include <QDataStream>
#include <QDebug>
#include <algorithm>
#include <private/qqmllistmodel_p.h>
#include <QQmlEngine>
#include "qqml.h"

#include <QAbstractItemModel>
#include <QList>
#include <QSharedPointer>


#define metatypeName(X) QMetaType::typeName(X)

/*!
    \group Nodes

    \title Nodes etc
    Description of group
*/

/*!
    \class Node
    \inmodule MetaGraph
    \brief Node
    \ingroup Nodes

    Fix me
*/

void Node::checkAlloc()
{
    if (childCount() >= allocSize - 1) {
        //        debugPrint << "realloc:" << allocSize << childCount() << QMetaType::typeName(typeId());
        allocSize *= 2;
        _childNodes().reserve(allocSize);
        fixIt();
    }
}

Node::Node() { m_childNodes.reserve(4); }


/*!
    \fn Node::~Node()
    Деструктор. Удаляет всех детей.
*/
Node::~Node()
{
//    qDebug() << "Destroyed node";
    if (childCount() > 0)
        qDeleteAll(_childNodes());
    reset();
}

QList<Edge *> Node::findInEdges(QStringList tagFilter, TagFilter mode) const
{
    QList<Edge *> ret;
    for (auto e : m_inEdges) {
        if (e.isNull())
            continue;
        if ((mode == MatchAll && e.data()->hasAllTags(tagFilter))
                || (mode == MatchAny && e.data()->hasAnyTag(tagFilter))
                || tagFilter.isEmpty())
            ret.append(e.data());
    }
    //    qDebug() << ret << m_inEdges;
    return ret;
}

QList<Node *> Node::findOutNodes(QStringList tagFilter, TagFilter mode) const
{
    QList<Node *> ret;
    for (auto e : m_outEdges) {
        if (e.isNull())
            continue;
        if ((mode == MatchAll && e.data()->hasAllTags(tagFilter))
                || (mode == MatchAny && e.data()->hasAnyTag(tagFilter))
                || tagFilter.isEmpty())
            ret.append(e.data()->endNode());
    }
    //    qDebug() << ret << m_outEdges;
    return ret;
}

QList<Node *> Node::findNeighborNodes(QStringList tagFilter, Node::TagFilter mode) const
{
    return findInNodes(tagFilter, mode) << findOutNodes(tagFilter, mode);
}

QList<Node *> Node::findInNodes(QStringList tagFilter, TagFilter mode) const
{
    QList<Node *> ret;
    for (auto e : m_inEdges) {
        if (e.isNull())
            continue;
        if ((mode == MatchAll && e.data()->hasAllTags(tagFilter))
                || (mode == MatchAny && e.data()->hasAnyTag(tagFilter))
                || tagFilter.isEmpty())
            ret.append(e.data()->startNode());
    }
    //    qDebug() << ret << m_inEdges;
    return ret;
}

QList<Edge *> Node::findEdges(QStringList tagFilter, Node::TagFilter mode) const
{
    return findInEdges(tagFilter, mode) << findOutEdges(tagFilter, mode);

}

QList<Edge *> Node::findOutEdges(QStringList tagFilter, TagFilter mode) const
{
    QList<Edge *> ret;
    for (auto e : m_outEdges) {
        if (e.isNull())
            continue;
        if ((mode == MatchAll && e.data()->hasAllTags(tagFilter))
                || (mode == MatchAny && e.data()->hasAnyTag(tagFilter))
                || tagFilter.isEmpty())
            ret.append(e.data());
    }
    //    qDebug() << ret << m_outEdges;
    return ret;
}

void Node::removeInEdge(Edge *edge)
{
    m_inEdges.erase(std::remove_if(m_inEdges.begin(),
                                   m_inEdges.end(),
                                   [edge](const QWeakPointer<Edge> &e) { return edge == e.data(); } ),
                    m_inEdges.end() );
}

void Node::removeOutEdge(Edge *edge)
{
    m_outEdges.erase(std::remove_if(m_outEdges.begin(),
                                    m_outEdges.end(),
                                    [edge](const QSharedPointer<Edge> &e) { return edge == e.data(); } ),
                     m_outEdges.end() );
}

bool Node::isAncestorOf(const Node *other, bool onlyDirect) const
{
    const Node *p = other->parent();
    while (p) {
        if (p == this)
            return true;
        p = p->parent();
        if (onlyDirect)
            break;
    }
    return false;
}

bool Node::isChildOf(const Node *other, bool onlyDirect) const
{
    return other->isAncestorOf(this, onlyDirect);
}

int Node::treeSize() const
{
    int ret = m_childNodes.size();
    for (Node* n : m_childNodes)
        ret += n->treeSize();
    return ret;
}
/**
 * @brief возвращает уровень иерархии от корневого узла
 * @return
 */
int Node::depth() const
{
    int ret = 0;
    Node *p = m_parent;
    while (p) {
        ret++;
        p = p->m_parent;
    }
    return ret;
}

Node *Node::takeChild(int i)
{
    Node *m = m_childNodes[i];
    m_childNodes.erase(m_childNodes.begin() + i);
//    Node *m = m_childNodes.takeAt(i);
    m->setParent(nullptr);
    return m;
}

void Node::removeChild(Node *child)
{
    for (auto it = m_childNodes.begin(); it != m_childNodes.end(); ++it) {
        if (*it == child)
            it = m_childNodes.erase(it);
    }
}

void Node::removeChild(int n)
{
    if (n >= 0 && n < m_childNodes.size()) {
        auto it = m_childNodes.begin() + n;
        delete *it;
        m_childNodes.erase(it);
    }
}

void Node::prependChild(Node *child)
{
    if (child->parent() == this)
        return;
    m_childNodes.insert(m_childNodes.begin(), child);
    child->setParent(this);
    child->setRow(0);
    checkAlloc();
}

void Node::appendChild(Node *child)
{
    if (child->parent() == this)
        return;
    m_childNodes.push_back(child);
    child->setParent(this);
    child->setRow(m_childNodes.size() - 1);
    checkAlloc();
}

void Node::insertChild(int pos, Node *child)
{
    if (child->parent() == this)
        return;
    auto it = m_childNodes.begin() + pos;
    m_childNodes.insert(it, child);
    child->setParent(this);
    child->setRow(pos);
    checkAlloc();
}

void Node::setParent(Node *parent)
{

    if(!parent) {
        m_parent = parent;
        it = stable_vector<Node*>::iterator(nullptr);
        return;
    }
    if (m_parent) {
        m_parent->removeChild(this);
        m_parent->fixIt();
    }
    m_parent = parent;
}

void Node::removeChildren(int n, int count, GraphModel *model)
{
    //    qDebug() << "remove children from" << n << "count" << count;
//    qDebug() << "removing node" << n << "count" << count << "last row id" << m_childNodes.last()->row() << "size" << m_childNodes.size();
    while(count--) {
        Node *node = m_childNodes.at(n);
        node->unlink(model);
        model->removeProxy(node);
        removeChild(n);
    }
//    if(!m_childNodes.isEmpty())
//        qDebug() << "removed n/ode" << n << "last row id" << m_childNodes.last()->row() << "size" << m_childNodes.size();
}

void Node::moveChildren(int from, int count, int to)
{
//    qDebug() << "Moving" << count << "children from" << from << "to" << to;
    if (to < from) {
//        boost::move_backward();
        int last = from + count - 1;
        auto isrc = m_childNodes.begin() + from + count - 1;
        while(count--) {
            m_childNodes.insert(m_childNodes.begin() + to, *isrc);
            isrc = m_childNodes.erase(isrc);
        }
    } else if (to > from + count) {

        auto isrc = m_childNodes.begin() + from;
        auto idst = m_childNodes.begin() + to - 1;
        while (count--) {
            m_childNodes.insert(idst, *isrc);
            isrc = m_childNodes.erase(isrc);
        }
    }
    fixIt();
}

void Node::moveChild(int from, int to)
{
    auto isrc = m_childNodes.begin() + from;
    auto idst = m_childNodes.begin() + to;
    m_childNodes.insert(idst, *isrc);
    isrc = m_childNodes.erase(isrc);
    fixIt();
}

void Node::moveChildUp(int index)
{
    if (index > 0 && index < m_childNodes.size())
        qSwap(m_childNodes[index], m_childNodes[index - 1]);
    fixIt();
}

void Node::moveChildDown(int index)
{
    if (index + 1 < m_childNodes.size())
        qSwap(m_childNodes[index], m_childNodes[index + 1]);
    fixIt();
}


bool Node::addInEdge(QSharedPointer<Edge> edge)
{
    m_inEdges.append(edge);
    return true;
}

bool Node::addOutEdge(QSharedPointer<Edge> edge)
{
    m_outEdges.append(edge);
    return true;
}

void Node::reset()
{
    m_outEdges.clear();
    m_inEdges.clear();
    m_childNodes.clear();
    m_parent = nullptr;
    it = stable_vector<Node*>::iterator(nullptr);
}

GraphModel *Node::model(const QModelIndex &nodeIndex)
{
    return const_cast<GraphModel*>(qobject_cast<const GraphModel*>(nodeIndex.model()));
}

/*!
 * \brief Node::unlink
 * Удаляет все связи у узла и дочерникх узлов. Вызвается перед удалением узла из модели.
 * \a model
 */
void Node::unlink(GraphModel *model)
{
    for (QWeakPointer<Edge> ie : m_inEdges) {
        QSharedPointer<Edge> ie_s = ie.toStrongRef();
        if (ie_s) {
            model->beginRemoveEdge(ie_s->getUid());
            Node *st = ie_s.data()->startNode();
            if (st) {
                st->removeOutEdge(ie.data());
//                model->notify(model->getNodeIndex(ie.data()->startNode()), { GraphModel::OutCountRole });
//                emit model->edgeRemoved( st, 0 );
            }
        }
    }
    m_inEdges.clear();
    model->endRemoveEdges();

    for (QSharedPointer<Edge> oe : m_outEdges) {
        model->beginRemoveEdge(oe->getUid());
        Node *end = oe->endNode();
        if (end) {
            end->removeInEdge(oe.data());
        }
    }
    m_outEdges.clear();
    model->endRemoveEdges();
    for (Node *child : m_childNodes)
        child->unlink(model);

}

/*!
    \fn QList<Node *> Node::outNodes(const QString &edgeTag) const

    Выполняет такую-то работу
*/

QList<Node *> Node::outNodes(const QString &edgeTag) const
{
    QList<Node *> ret;
    for (const QSharedPointer<Edge> &e : m_outEdges) {
        if (!e.isNull()) {
            if (e->tag() == edgeTag) {
                Q_ASSERT(e->endNode() != nullptr);
                ret.append(e->endNode());
            }
        }
    }
    return ret;
}

QList<Node *> Node::inNodes(const QString &edgeTag) const
{
    QList<Node *> ret;
    for (const QSharedPointer<Edge> &e : m_outEdges) {
        if (!e.isNull()) {
            if (e->tag() == edgeTag) {
                Q_ASSERT(e->startNode() != nullptr);
                ret.append(e->startNode());
            }
        }
    }
    return ret;
}

void Node::clear()
{
    qDeleteAll(_childNodes());
    reset();
}



GraphModel *NodeHandle::model() const
{
    //    Q_ASSERT(d.model());
    return const_cast<GraphModel*>(static_cast<const GraphModel*>(d.model()));
}

NodeHandle::~NodeHandle()
{}

Node *NodeHandle::node() const
{
    idxCheck(nullptr);
    return model()->getNode(d);
}


QVariant NodeHandle::nodeData() const
{
    idxCheck(QVariant());
    return node()->data();
}

QVariantList NodeHandle::edges(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Edge* e : node()->findEdges(tags, Node::MatchAll))
        ret.append(QVariant::fromValue(EdgeHandle(e, model())));
    return ret;
}

QVariantList NodeHandle::neighbors(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Node* n : node()->findNeighborNodes(tags, Node::MatchAll))
        ret.append(model()->getHandle(n));
    return ret;
}

QList<int> NodeHandle::address() const
{
    QList<int> ret;
    idxCheck(ret);
    ret = model()->iPath(d);
    return ret;
}

QString NodeHandle::mrl() const
{
    idxCheck(QString());
    return model()->getMRL(d);
}

ModelAgent *NodeHandle::agent() const
{
    idxCheck(nullptr);
    return model()->get(d);
}

int NodeHandle::rowIndex() const
{
    idxCheck(-1);
//    qDebug() << "modelindex" << d.row() << "node" << node()->row() ;
    return d.row();
}

void NodeHandle::setRowIndex(int rowIndex)
{
    idxCheck();
    model()->moveRow(d.parent(), d.row(), d.parent(), rowIndex);
}

QJSValue NodeHandle::parent() const
{
    idxCheck(QJSValue());
    QQmlEngine *e = qmlEngine(model());
    if (!e)
        return QJSValue();
    GraphModel *m = model();
    const Node *n = m->getNode(d);
    if (n == m->root())
        return e->toScriptValue(NodeHandle());
    Node *p = n->parent();
    if (p == m->root())
        return e->toScriptValue(m->getRootHandle());
    return e->toScriptValue(m->getHandle(p));
}

QString NodeHandle::typeName() const
{
    idxCheck(QString());
    return QMetaType::typeName(node()->typeId());
}

int NodeHandle::typeId() const
{
    idxCheck(QMetaType::UnknownType);
    return node()->typeId();
}

int NodeHandle::childCount() const
{
    idxCheck(0);
    return node()->childCount();
}

bool NodeHandle::isValid() const
{
    idxCheck(false);
    //    qDebug() << d;
    return node()->isValid();
}

QString NodeHandle::nodeId() const
{
    idxCheck(QString());
    return model()->getNodeId(d);
}

void NodeHandle::setNodeId(QString nodeId)
{
    idxCheck();
    model()->setNodeId(d, nodeId);
}

void NodeHandle::setParent(QJSValue newParent)
{
    idxCheck();
    qDebug() << __func__ << newParent.toVariant().value<NodeHandle>().mrl();
    NodeHandle p = newParent.toVariant().value<NodeHandle>();
    if (!p.isValid()) {
        qWarning() << "invalid parent or move to root?";
        return;
    }
    Node *n = node();
    GraphModel *m = model();
//    qDebug() << "moving to new parent" << node()->findEdges();
    model()->moveNode(node(), p.node());
    d = m->getNodeIndex(n);
//    qDebug() << "moved to new parent" << node()->findEdges();

}

QVariantList NodeHandle::outEdges(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Edge* e : node()->findOutEdges(tags, Node::MatchAll))
        ret.append(QVariant::fromValue(EdgeHandle(e, model())));
    return ret;
}

QVariantList NodeHandle::inEdges(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Edge* e : node()->findInEdges(tags, Node::MatchAll))
        ret.append(QVariant::fromValue(EdgeHandle(e, model())));
    return ret;
}

QVariantList NodeHandle::outNodes(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Node* n : node()->findOutNodes(tags, Node::MatchAll))
        ret.append(model()->getHandle(n));
    return ret;
}

QVariantList NodeHandle::inNodes(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Node* n : node()->findInNodes(tags, Node::MatchAll))
        ret.append(model()->getHandle(n));
    return ret;
}

QString NodeHandle::dumpToJson() const
{
        idxCheck(QString());
    return model()->dump(d).toJson();
}

QVariantList NodeHandle::childNodes() const
{
    QVariantList ret;
    idxCheck(ret);

    for (Node *n : node()->childNodes()) {
        ret.append(model()->getHandle(n));
    }
    //    qDebug() << nodeId() << "has" << ret.size() << "child nodes";
    return ret;
}

void NodeHandle::destroy()
{
    model()->removeNode(d);
    d = QModelIndex();
}

QVariant NodeHandle::sibling(int index) const
{
    idxCheck(QVariant());
    return model()->getHandle(d.sibling(index, 0));
}

QVariant NodeHandle::nextSibling() const
{
    idxCheck(QVariant());
    return model()->getHandle(d.sibling(d.row() + 1, 0));
}

QVariant NodeHandle::prevSibling() const
{
    idxCheck(QVariant());
    return model()->getHandle(d.sibling(d.row() - 1, 0));
}

QVariant NodeHandle::findChild(QString id) const
{
    idxCheck(QVariant());
    return model()->getHandle(model()->getNode(node(), id));
}


void NodeHandle::moveUp()
{
    idxCheck();
    Node *n = node();
    if (!n->parent())
        return;
    int r = n->row();
    if (r > 0) {
        model()->beginMoveRows(d.parent(), r, r, d.parent(), r - 1);
        n->parent()->moveChildUp(r);
        model()->endMoveRows();
    }
}

void NodeHandle::moveDown()
{
    idxCheck();
    Node *n = node();
    if (!n->parent())
        return;
    int r = n->row();
    qDebug() << "moving index" << r << "to" << r + 2;
    if (r + 1 < n->parent()->childCount()) {
        model()->beginMoveRows(d.parent(), r, r, d.parent(), r + 2);
        n->parent()->moveChildDown(r);
        model()->endMoveRows();
    }
}

void NodeHandle::moveToHead()
{
    idxCheck();
    Node *n = node();
    if (!n->parent())
        return;
    int r = n->row();
    if (r > 0) {
        model()->beginMoveRows(d.parent(), r, r, d.parent(), 0);
        n->parent()->moveChild(r, 0);
        model()->endMoveRows();
    }
}

void NodeHandle::moveToTail()
{
    idxCheck();
    Node *n = node();
    if (!n->parent())
        return;
    int r = n->row();
    if (r + 1 < n->parent()->childCount()) {
        model()->beginMoveRows(d.parent(), r, r, d.parent(), n->parent()->childCount() - 1);
        n->parent()->moveChild(r, n->parent()->childCount() - 1);
        model()->endMoveRows();
    }
}

void NodeFactory::setParentIndex(const QPersistentModelIndex &root)
{
    if (m_parentIndex == root)
        return;
    m_parentIndex = root;
    emit parentHandleChanged();
}

void NodeFactory::setParentHandle(NodeHandle parent)
{
    if (parent.modelIndex() == m_parentIndex)
        return;
    setParentIndex(parent.modelIndex());
}


QVariant NodeFactory::beginCreate()
{
    if (model()->status() == GraphModel::Ready) {
        QModelIndex idx = model()->beginAppendNode(createNode(), m_parentIndex);
        return model()->getHandle(idx);
    }
    if (model()->status() == GraphModel::CreatingNode)
        qWarning() << "Unable to start transaction: another element is being constructed" << model()->incubatedNode;
    else if(model()->status() == GraphModel::Locked)
        qWarning() << "Unable to start transaction: model is locked";
    return QVariant();
}

void NodeFactory::endCreate()
{
    if (model()->status() == GraphModel::CreatingNode) {
        model()->commit();
        return;
    }
    if (model()->status() == GraphModel::Ready)
        qWarning() << "Unable to end transaction: no insert transaction is active";
}

void NodeFactory::setParent(NodeChildrenModel *subModel)
{
    if (!subModel)
        resetParent();
    else
        setParentIndex(subModel->rootIndex());
}

void NodeFactory::resetParent()
{
    m_parentIndex = QModelIndex();
    emit parentHandleChanged();
}

QVariant NodeFactory::create(const QString &nodeId)
{
    return model()->getHandle(model()->appendNode(createNode(nodeId), m_parentIndex)); // >_<
}

//void NodeFactory::initNode(Node *node, const QString &nodeId)
//{
//    const Node *pnode;
//    if (!m_parentIndex.isValid())
//        pnode = model()->root();
//    else
//        pnode = parentHandle().node();

//    QString id;
//    if (!nodeId.isEmpty())
//        id = model()->getUniqueNodeId(nodeId, pnode);
//    else
//        id = model()->getUniqueNodeId(m_nodeId, pnode);
//    model()->setNodeId(node, id, pnode);
//}

NodeFactory::NodeFactory(QObject *parent) :
    QObject(parent)
{
    m_model = qobject_cast<GraphModel*>(parent);
}

void NodeFactory::componentComplete()
{
    m_model = qobject_cast<GraphModel*>(parent());
}

void NodeFactory::setNodeId(QString nodeId)
{
    if (m_nodeId == nodeId)
        return;
    m_nodeId = nodeId;
    emit nodeIdChanged();
}

int NodeFactory::nodeMetatypeId() const
{
    return qMetaTypeId<Node>();
}

void NodeFactory::importNode(QDataStream &in, Node *node)
{
    // load id & children
    QString id;
    int c;
    in >> id >> c;
    if (!model()->hasId(node))
        model()->setNodeId(node, id);
    while (c-- && in.status() == QDataStream::Ok) {
        model()->importNode(in, node);
    }
    //    node->fixIt();
}

void NodeFactory::exportNode(QDataStream &out, Node *node) const
{
    Q_ASSERT(model() != nullptr);

    out << model()->getNodeId(node) << node->childCount();
    for (Node *n : node->childNodes()) {
        model()->exportNode(out, n);
    }
}

Node *NodeFactory::createNode(const QString &nodeId)
{
    Node *ptr = reinterpret_cast<Node*>(QMetaType::create(nodeMetatypeId()));
    setupNode(ptr);
    const Node *pnode;
    if (!m_parentIndex.isValid())
        pnode = model()->root();
    else
        pnode = parentHandle().node();
    QString id;
    if (!nodeId.isEmpty())
        id = model()->getUniqueNodeId(nodeId, pnode);
    else
        id = model()->getUniqueNodeId(m_nodeId, pnode);
    model()->setNodeId(ptr, id, pnode);
    return ptr;
}





void NodeFactory::classBegin() {}


NodeFactory::~NodeFactory() {}


QString NodeFactory::nodeTypeName() const { return QMetaType::typeName(nodeMetatypeId()); }


int Node::row() const { return it - m_parent->m_childNodes.cbegin(); } // FIXME: check iterators
//int Node::row() const { return getRowId(); /*return it - m_parent->m_childNodes.cbegin(); */} // FIXME: check iterators


int Node::getRowId() const
{
    if (!m_parent) return -1;
    int i = 0;
    for (const Node* n : m_parent->m_childNodes) {
        if (n == this)
            return i;
        ++i;
    }
    return -1;
}
