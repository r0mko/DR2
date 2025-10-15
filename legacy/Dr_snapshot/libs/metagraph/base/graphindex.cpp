#include <graphmodel.h>
#include "graphindex.h"
#undef idxCheck

#define idxCheck(X) if (!isValid()) return X


GraphModel *GraphIndex::gmodel() const { return const_cast<GraphModel*>(static_cast<const GraphModel*>(model())); }

Node *GraphIndex::node() const { idxCheck(nullptr); return gmodel()->getNode(*this); }

QString GraphIndex::nodeId() const
{
    idxCheck(QString());
    return gmodel()->getNodeId(*this);
    //        return QPersistentModelIndex::data(GraphModel::NodeIdRole).toString();
}

QVariant GraphIndex::data() const
{
    idxCheck(QVariant());
    return node()->data();
}

GraphIndex GraphIndex::parent() const
{
    idxCheck(GraphIndex());
//    return GraphIndex(QModelIndex::parent());
    return GraphIndex(QPersistentModelIndex::parent());
}

int GraphIndex::childCount() const
{
    idxCheck(0);
    return node()->childCount();
}

int GraphIndex::rowIndex() const
{
    idxCheck(-1);
    return node()->row();
}

QVariantList GraphIndex::childNodes() const
{
    QVariantList ret;
    idxCheck(ret);
    for (Node *n : node()->childNodes()) {
        ret.append(GraphIndex(gmodel()->getNodeIndex(n)));
    }
    //    qDebug() << nodeId() << "has" << ret.size() << "child nodes";
    return ret;
}

QList<int> GraphIndex::address() const
{
    QList<int> ret;
    idxCheck(ret);
    ret = gmodel()->iPath(*this);
    return ret;
}

QString GraphIndex::mrl() const
{
    idxCheck(QString());
    return gmodel()->getMRL(*this);
}

QString GraphIndex::typeName() const
{
    idxCheck(QString());
    return QMetaType::typeName(node()->typeId());
}

void GraphIndex::setNodeId(QString nodeId)
{
    idxCheck();
    gmodel()->setNodeId(*this, nodeId);
}

void GraphIndex::setData(QVariant data)
{
    idxCheck();
    auto n = node();
    if (!n)
        return;
    n->setData(data);
//    qDebug() << "update node data" << data.toVariant();
    gmodel()->notify(*this, { GraphModel::DataRole });
}

void GraphIndex::setParent(GraphIndex &parent)
{
    qDebug() << "IMPLEMENT ME!";
}

QVariant GraphIndex::getData() const
{
    idxCheck(QVariant());
    return node()->data();
}

QVariantList GraphIndex::outEdges(QString tagFilter) const
{
    QVariantList ret;
    idxCheck(ret);
    for (const QSharedPointer<Edge> &e : node()->outEdges()) {
        if (tagFilter.isEmpty() || e.data()->hasTag(tagFilter))
            ret.append(QVariant::fromValue(EdgeHandle(e.data(), gmodel())));
    }
    return ret;
}

QVariantList GraphIndex::inEdges(QString tagFilter) const
{
    QVariantList ret;
    idxCheck(ret);
    for (const QSharedPointer<Edge> &e : node()->inEdges()) {
        if (tagFilter.isEmpty() || e.data()->hasTag(tagFilter))
            ret.append(QVariant::fromValue(EdgeHandle(e.data(), gmodel())));
    }
    return ret;
}

QVariantList GraphIndex::outNodes(QString tagFilter) const
{
    QVariantList ret;
    idxCheck(ret);
    for (const QSharedPointer<Edge> &e : node()->outEdges()) {
        if (tagFilter.isEmpty() || e.data()->hasTag(tagFilter))
            ret.append(GraphIndex(gmodel()->getNodeIndex(e.data()->endNode())));
    }
    return ret;
}

QVariantList GraphIndex::inNodes(QString tagFilter) const
{
    QVariantList ret;
    idxCheck(ret);
    for (const QSharedPointer<Edge> &e : node()->inEdges()) {
        if (tagFilter.isEmpty() || e.data()->hasTag(tagFilter))
            ret.append(GraphIndex(gmodel()->getNodeIndex(e.data()->startNode())));
    }
    return ret;
}

//GraphIndex &GraphIndex::operator=(const QModelIndex &other)
//{

//    return GraphIndex(QPersistentModelIndex::operator =(other));
//}
