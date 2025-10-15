#include "edge.h"
#include "node.h"
#include "graphmodel.h"
#include <QDataStream>
#include <QQmlEngine>
#include "qqml.h"
#include <QDebug>

/*!
    \group Edges

    \title Edges etc

    Group of Edges
*/

/*!
    \class Edge
    \inmodule MetaGraph
    \brief Edge
    \ingroup Edges

    Fix me
*/

inline bool extractIndex(const QModelIndex &idx, Node **node, GraphModel **model = nullptr)
{
    if (!idx.isValid())
        return false;
    GraphModel *gm = const_cast<GraphModel*>(qobject_cast<const GraphModel*>(idx.model()));
    if (!gm)
        return false;
    if (model)
        *model = gm;
    *node = gm->getNode(idx);
    return true;
}

QVariant EdgeHandle::data() const
{
    checkPtr(QVariant());
    return d->data();
}

QString EdgeHandle::tag() const
{
    checkPtr(QString());
    return d->tag();
}

QJSValue EdgeHandle::startNode() const
{
    checkPtr(QJSValue());
    if (d->startNode() == nullptr)
        return QJSValue();

    QQmlEngine *e = qmlEngine(m_model);
    if (!e)
        return QJSValue();
    return e->toScriptValue(startNodeHandle());
}

QJSValue EdgeHandle::endNode() const
{
    checkPtr(QJSValue());
    if (d->endNode() == nullptr)
        return QJSValue();

    QQmlEngine *e = qmlEngine(m_model);
    if (!e)
        return QJSValue();
    return e->toScriptValue(endNodeHandle());
//    checkPtr(NodeHandle());
    //    return NodeHandle(m_model->getNodeIndex(d->endNode()));
}

QVariant EdgeHandle::startNodeHandle() const
{
    checkPtr(QVariant());
    return m_model->getHandle(d->startNode());
}

QVariant EdgeHandle::endNodeHandle() const
{
    checkPtr(QVariant());
    return m_model->getHandle(d->endNode());
}

void EdgeHandle::setData(QVariant data)
{
    checkPtr();
    d->setData(data);
    notifyModel();
}

uint EdgeHandle::uid() const
{
    checkPtr(0xffffffff);
    return d->getUid();
}

void EdgeHandle::destroy()
{
    checkPtr();
    Node *st = d->startNode();
    Node *end = d->endNode();
    if (end) {
        QModelIndex idx = m_model->getNodeIndex(d->endNode());
        end->removeInEdge(d);
        emit m_model->dataChanged(idx, idx, { GraphModel::InCountRole, GraphModel::HandleRole });
    }
    if (st) {
        QModelIndex idx = m_model->getNodeIndex(d->startNode());
        st->removeOutEdge(d);
        emit m_model->dataChanged(idx, idx, { GraphModel::OutCountRole, GraphModel::HandleRole });
    }


    d = nullptr;
}

bool EdgeHandle::hasTag(const QString &tag) const
{
    checkPtr(false);
    return d->hasTag(tag);
}

QVariant EdgeHandle::getData() const
{
    checkPtr(QVariant());
    return d->data();
}

QStringList EdgeHandle::tags() const
{
    QStringList ret;
    checkPtr(ret);
    return d->tags();
}

void EdgeHandle::notifyModel() const
{
    if (m_model->getNodeIndex(d->startNode()).isValid())
        emit m_model->dataChanged(m_model->getNodeIndex(d->startNode()), m_model->getNodeIndex(d->startNode()), { GraphModel::OutCountRole });
    if (m_model->getNodeIndex(d->endNode()).isValid())
        emit m_model->dataChanged(m_model->getNodeIndex(d->endNode()), m_model->getNodeIndex(d->endNode()), { GraphModel::InCountRole });
}

uint Edge::count = 0;





Edge::~Edge()
{
    qDebug() << "Edge destroyed";
}

QStringList Edge::tags() const
{
    QStringList ret;
    for (uint hash : m_tags)
        ret.append(GraphModel::edgeTags.value(hash));
    return ret;
}

QString Edge::tag() const
{
    return tags().join("|");
}

bool Edge::hasTag(const QString &tag) const
{
    return m_tags.contains(qHash(tag, MAGIC_V2));
}

bool Edge::hasAllTags(const QStringList &tags) const
{
    for (const QString &s : tags)
        if (!hasTag(s)) return false;
    return true;
}

bool Edge::hasAnyTag(const QStringList &tags) const
{
    for (const QString &s : tags)
        if (hasTag(s)) return true;
    return false;

}

void Edge::setTag(const QString &tag)
{
    m_tags.clear();
    addTag(tag);
}

void Edge::addTag(const QString &tag)
{
    uint hash = qHash(tag, MAGIC_V2);
    if (GraphModel::edgeTags.contains(hash))
        m_tags.insert(hash);
    else
        qWarning() << "unknown tag" << tag;
}

int Edge::typeId() const
{
    return SELF_TYPEID;
}
