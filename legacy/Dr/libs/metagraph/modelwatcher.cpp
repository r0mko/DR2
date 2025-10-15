#include "graphmodel.h"
#include "modelwatcher.h"

#include <QQmlEngine>
#include "qqml.h"

ModelWatcher::ModelWatcher(QObject *parent) : QObject(parent)
{}

GraphModel *ModelWatcher::model() const
{
    return m_model;
}

QString ModelWatcher::mrl() const
{
    return m_mrl;
}

//ModelAgent *ModelWatcher::modelData() const
//{
//    return m_model->get(i);
//}

bool ModelWatcher::linked() const
{
    return !mrlWatcher;
}

QModelIndex ModelWatcher::modelIndex() const
{
    return i;
}

QVariantList ModelWatcher::children() const
{
    Node *node = model()->getNode(i);
    if (!node)
        return QVariantList();
    QVariantList ret;
    for (Node *n : node->childNodes()) {
        ret.append(model()->getHandle(n));
    }
    return ret;
}

void ModelWatcher::componentComplete()
{
    if (!m_mrl.isEmpty() && !i.isValid())
        resolveMrl();
    connect(m_namedChildren, &QQmlPropertyMap::valueChanged, [this](const QString & key, const QVariant & value) {
//        qDebug() << "value changed" << key << value;
    });

}

QJSValue ModelWatcher::handle() const
{
    QQmlEngine *e = qmlEngine(this);
    if(!e)
        return QJSValue();
    QJSValue ret = e->toScriptValue(m_model->getHandle(i));
    return ret;
}

QQmlPropertyMap *ModelWatcher::namedChildren() const
{
    return m_namedChildren;
}

NodeChildrenModel *ModelWatcher::childrenModel()
{
    if (m_childrenModel) {
        m_childrenModel = new NodeChildrenModel(this);
        m_childrenModel->setRootIndex(i);
    }
    return m_childrenModel;
}

QStringList ModelWatcher::childIds() const
{
    return m_namedChildren->keys();
}

void ModelWatcher::setModel(GraphModel *model)
{
    if (m_model == model)
        return;

    m_model = model;
    modelChange(m_model);

    emit modelChanged(model);
}

void ModelWatcher::setMrl(QString mrl)
{
    if (m_mrl == mrl)
        return;

    m_mrl = mrl;
    emit mrlChanged();
    resolveMrl();
}

void ModelWatcher::setModelIndex(QModelIndex index)
{
    if (i == index)
        return;

    i = index;

    if (i.model() != static_cast<const QAbstractItemModel*>(m_model)) {
        m_model = const_cast<GraphModel*>(static_cast<const GraphModel*>(index.model()));
        emit modelChanged(m_model);
    }
    emit modelIndexChanged(index);
    emit handleChanged();
    QString mrl = m_model->getMRL(i);
    if (m_mrl != mrl) {
        m_mrl = mrl;
        emit mrlChanged();
    }
    connect(m_model, &QAbstractItemModel::dataChanged, this, &ModelWatcher::onDataChanged);
    //    connect(model, &QAbstractItemModel::rowsAboutToBeMoved, this, &ModelAgentBase::onRowsAboutToBeMoved);
    //    connect(model, &QAbstractItemModel::rowsMoved, this, &ModelAgentBase::onRowsMoved);
    connect(m_model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ModelWatcher::onRowsAboutToBeRemoved);
    connect(m_model, &QAbstractItemModel::rowsRemoved, this, &ModelWatcher::onRowsRemoved);
    //    connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this, &ModelAgentBase::onRowsAboutToBeInserted);
    connect(m_model, &QAbstractItemModel::rowsInserted, this, &ModelWatcher::onRowsInserted);
    connect(m_model, &QAbstractItemModel::modelAboutToBeReset, this, &ModelWatcher::onModelAboutToBeReset);
    connect(m_model, &QAbstractItemModel::modelReset, this, &ModelWatcher::onModelReset);
    if (m_namedChildren)
        m_namedChildren->deleteLater();
    m_namedChildren = new QQmlPropertyMap(this);
    emit namedChildrenChanged();
    initNamedChildren();

}

void ModelWatcher::childrenDataChange(int first, int last, const QVector<int> &roles)
{
    Node *node = m_model->getNode(i);
    if (roles.contains(GraphModel::HandleRole)) {
        for (int i = first; i <= last; ++i) {
            Node *c = node->childNodes().at(i);
            QString id = m_model->getNodeId(c);
            QVariant value = m_model->getHandle(c);
            m_namedChildren->insert(id, value);
            emit m_namedChildren->valueChanged(id, value);
        }
    }
    if (roles.contains(GraphModel::NodeIdRole)) {
        const NId &nid = m_model->node2nid.value(node);
        auto it = nid.children.cbegin();
        while (it != nid.children.cend()) {
            QVariant value = m_model->getHandle(it.value());
            if (!m_namedChildren->contains(it.key())) {
                m_namedChildren->insert(it.key(), value);
            }
            ++it;
        }
//        for (QString key : m_namedChildren->keys()) {
//            if (!nid.children.contains(key)) {
//                m_namedChildren->clear(key);
////                qDebug() << "clearing key" << key << nid.children.keys();
//            }
//        }
        for (int i = first; i <= last; ++i) {
            Node *c = node->childNodes().at(i);
            QString id = m_model->getNodeId(c);
            QVariant value = m_model->getHandle(c);
            m_namedChildren->insert(id, value);
            emit m_namedChildren->valueChanged(id, value);

        }
    }

    if (roles.isEmpty()) {
        qWarning() << "Empty roles!";
    }
    emit childIdsChanged();

}

void ModelWatcher::dataChange(const QVector<int> &roles)
{
    if (roles.isEmpty()) {
        emit childrenChanged();
        emit handleChanged();
        return;
    }
    Node *node = m_model->getNode(i);
    const NId &nid = m_model->node2nid.value(node);

    for (int role : roles) {
        switch (role) {
        case GraphModel::HandleRole:
            emit handleChanged();
            break;
        case GraphModel::ChildCountRole:
        case GraphModel::ChildrenRole: {
            if (nid.children.size() > m_namedChildren->size()) {
                auto it = nid.children.cbegin();
                while (it != nid.children.cend()) {
                    if (!m_namedChildren->contains(it.key())) {
                        m_namedChildren->insert(it.key(), m_model->getHandle(it.value()));
                        emit childIdsChanged();
                    }
                    ++it;
                }
            }
            emit childrenChanged();
            break;
        }

        default:
            break;
        }
    }
    if (roles.contains(GraphModel::HandleRole)) {
        return;
    }
}

void ModelWatcher::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!i.isValid())
        return;
    if (!topLeft.isValid() || !bottomRight.isValid())
        return;
    if (topLeft.parent() == i) {
        childrenDataChange(topLeft.row(), bottomRight.row(), roles);
        return;
    }

    if (i.parent() != topLeft.parent())
        return;
    if (i.row() < topLeft.row() || i.row() > bottomRight.row())
        return;
    if (roles.isEmpty() || roles.contains(GraphModel::MRLRole)) {
        if (!checkMrl()) {
            invalidate();
            resetLink();
            return;
        }
    }
    dataChange(roles);
}

void ModelWatcher::onRowsInserted(const QModelIndex &parent, int first, int last)
{
    if (!i.isValid())
        return;
    if (parent == i) {
        emit childrenChanged();
        for (int i = first; i <= last; ++i) {
            Node *n = m_model->getNode(m_model->index(i, 0, parent));
            if (m_model->hasId(n)) {
                QString key = m_model->getNodeId(n);
                QVariant value = m_model->getHandle(n);
                m_namedChildren->insert(key, value);
                emit childIdsChanged();
                emit m_namedChildren->valueChanged(key, value);
            }
        }
    }
}

void ModelWatcher::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    if (!i.isValid())
        return;
    if (i.parent() != parent)
        return;
    if (i.row() >= first && i.row() <= last) {
        //        resetLink();
        invalidate();
        emit linkedChanged(false);
        i = QModelIndex();
    }
}

void ModelWatcher::onRowsRemoved(const QModelIndex &parent, int, int)
{
    if (!i.isValid()) {
        if (!m_mrl.isEmpty())
            resolveMrl();
        return;
    }
    if (parent == i) {
        emit childrenChanged();
    }
}

void ModelWatcher::onModelAboutToBeReset()
{

    resetLink();
//    qDebug() << "Source model going to reset";
}

void ModelWatcher::onModelReset()
{
//    qDebug() << "Source model was reset";
    invalidate();
    if (!m_mrl.isEmpty())
        resolveMrl();
}

bool ModelWatcher::checkMrl()
{
    NodeHandle h = m_model->getHandle(m_mrl).value<NodeHandle>();
    //    qDebug() << "checking mrl" << m_mrl << h.mrl();
    return h.isValid() && m_mrl == h.mrl();
}

void ModelWatcher::initNamedChildren()
{
    Node *node = m_model->getNode(i);
    const NId &nid = m_model->node2nid.value(node);
    auto it = nid.children.cbegin();
    while (it != nid.children.end()) {
        QVariant handle = model()->getHandle(it.value());
        m_namedChildren->insert(it.key(), handle);
        emit m_namedChildren->valueChanged(it.key(), handle);
        it++;
    }
    emit childIdsChanged();
}

void ModelWatcher::resolveMrl()
{
    if (!m_model) {
        connect(this, &ModelWatcher::modelChanged, this, &ModelWatcher::resolveMrl, Qt::UniqueConnection);
        return;
    }
    disconnect(this, &ModelWatcher::modelChanged, this, &ModelWatcher::resolveMrl);
    NodeHandle h = m_model->getHandle(m_mrl).value<NodeHandle>();
    //    qDebug() << "resolving mrl" << m_mrl << sentinel << h.isValid();
    if (!h.isValid()) {
        if (mrlWatcher)
            return;
        //        qDebug() << "node" << m_mrl << "not found: initializing watcher sentinel";
        m_model->disconnect(0, this, 0);
        connect(m_model, &QAbstractItemModel::modelReset, this, &ModelWatcher::resolveMrl);
        connect(m_model, &QAbstractItemModel::dataChanged, this, &ModelWatcher::resolveMrl);
        connect(m_model, &QAbstractItemModel::rowsInserted, this, &ModelWatcher::resolveMrl);
        mrlWatcher = true;
    } else {
        m_model->disconnect(0, this, 0);
        mrlWatcher = false;
        setModelIndex(h.modelIndex());
        emit linkedChanged(true);
        //        qDebug() << "node" << m_mrl << "resolved!";
    }
}

void ModelWatcher::resetLink()
{
    m_namedChildren->deleteLater();
    m_namedChildren = nullptr;
    emit namedChildrenChanged();
    emit linkedChanged(false);
    i = QModelIndex();
    m_model->disconnect(0, this, 0);
    emit handleChanged();
    if (!m_mrl.isEmpty())
        resolveMrl();
}

