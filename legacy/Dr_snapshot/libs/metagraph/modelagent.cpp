#include "graphmodel.h"
#include "modelagent.h"
#include "propertybuilder.h"
#include "qqml.h"
#include <private/qmetaobjectbuilder_p.h>
#include "edge.h"
#include <QQmlEngine>

ModelAgent::ModelAgent(const QModelIndex &idx, const proto &prototype, QObject *parent) :
    DynamicObjectExtension(parent),
    _proto(&prototype)
{
    g_model = const_cast<GraphModel*>(qobject_cast<const GraphModel*>(idx.model()));
    if (!idx.isValid()) {
        qWarning() << "Constructing ModelObject with invalid QModelIndex!";
        m_metaObject = const_cast<QMetaObject*>(&QObject::staticMetaObject);
        return;
    }
    setModelIndex(idx);
    setMetaObject(_proto->metaObject);
}

void ModelAgent::dataChange(const QVector<int> &roles)
{
    if (!roles.isEmpty()){
        for (int role : roles) {
//                    qDebug() << "emits signal" << role << model()->roleNames().value(role);
            emitSignal(_proto->role2signal.value(role));
        }
    } else {
        for (int i : _proto->role2signal.values()) {
//                    qDebug() << "emits signal" << i << model()->roleNames().value(i);
            emitSignal(i);
        }
    }
}

void ModelAgent::invalidate()
{
    if (d.model())
        d.model()->disconnect(0, this, 0);
    d = QModelIndex();
    qDebug() << "agent invalidated";
}

const QMetaObject *ModelAgent::metaObject() const
{
    if (!_proto)
        return &ModelAgentBase::staticMetaObject;
    if (!_proto->metaObject)
        return &ModelAgentBase::staticMetaObject;
//    qDebug() << "_proto.metaObject" << _proto->metaObject;
    return _proto->metaObject;
}

void ModelAgent::setProperty(int id, QVariant value)
{
    if (d.isValid()) {
        int role = _proto->getRole(id);
        if (!const_cast<QAbstractItemModel*>(d.model())->setData(d, value, role))
            qWarning() << "Failed to set data" << value << role;
    }
}

QVariant ModelAgent::getProperty(int id) const
{
    return d.data(_proto->getRole(id));
}

void ModelAgent::connectNotify(const QMetaMethod &)
{
    connects++;
}

void ModelAgent::disconnectNotify(const QMetaMethod &)
{
    connects--;
    if (!connects) {
//        qDebug() << "last receiver disconnected";
//        disconnect(binding2);
        //        deleteLater();
    }
}



void ModelAgent::proto::addMapping(int role, int propertyIndex, int notifierIndex) const
{
    role2signal.insert(role, notifierIndex);
    prop2role.insert(propertyIndex, role);
}

bool ModelAgent::proto::build(QHash<int, QByteArray> roleNames, QHash<int, QByteArray> roleTypes, const QAbstractItemModel *model, bool strictTypes) const
{
    if (metaObject) {
        bool ok = true;
        for (auto it = roleNames.cbegin(); it != roleNames.cend(); ++it) {
            int pidx = metaObject->indexOfProperty(it.value().constData());
            if (pidx >= 0) {
                QMetaProperty prop = metaObject->property(pidx);
                if (it.key() <= GraphModel::LastNodeRole || prop.typeName() == roleTypes.value(it.key()))
                    continue;
            }
            ok = false;
            break;
        }
        if (ok) {
//            qDebug() << "metaobject was not updated since last build";
            // metaobject was not updated since last build
            return false;
        }
    }

    QMetaObjectBuilder b;
//    b.setSuperClass(&QObject::staticMetaObject);
    QHash<int, QByteArray>::const_iterator it = roleNames.cbegin();
    b.setClassName(QByteArray(model->metaObject()->className()).append("Agent"));
    b.setSuperClass(&ModelAgentBase::staticMetaObject);
    b.setStaticMetacallFunction(&proto::qt_static_metacall);
    while (it != roleNames.cend() ) {
        QMetaPropertyBuilder p;
        if (strictTypes) {
            QVariant v = model->data(QModelIndex(), it.key());
//            QVariant v;
            p = b.addProperty(it.value(), v.typeName());
        } else {
            if (it.key() > GraphModel::LastNodeRole) {
                p = b.addProperty(it.value(), roleTypes[it.key()]);
            } else {
                p = b.addProperty(it.value(), "QVariant");
            }
        }
        p.setReadable(true);
        p.setWritable(true);
        p.setStdCppSet(false);
        p.setFinal(true);
        p.setNotifySignal(b.addSignal(PropertyBuilder::getNotifierSignature(it.value())));
        int role = it.key();
        int propertyIndex = p.index() + b.superClass()->propertyCount();
        int notifierIndex = p.notifySignal().index() + b.superClass()->methodCount();
        addMapping(role, propertyIndex, notifierIndex);

        // , int propertyIndex, int notifierIndex
//        qDebug() << "Add mapping role" << role << "property index" << propertyIndex << "property name" << p.name();
        ++it;
    }
    metaObject = b.toMetaObject();
//    qDebug() << "built agent data" << metaObject->className();
    return true;

}

QVariantList ModelAgentBase::children() const
{
    idxCheck(QVariantList());
    Node *node = model()->getNode(d);
    if (!node)
        return QVariantList();
    QVariantList ret;
    for (Node *n : node->childNodes()) {
        ret.append(model()->getHandle(n));
    }
    return ret;
}

QVariantList ModelAgentBase::outEdges(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Edge* e : model()->getNode(d)->findOutEdges(tags, Node::MatchAll))
        ret.append(QVariant::fromValue(EdgeHandle(e, model())));
    return ret;
}

QVariantList ModelAgentBase::inEdges(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Edge* e : model()->getNode(d)->findInEdges(tags, Node::MatchAll))
        ret.append(QVariant::fromValue(EdgeHandle(e, model())));
    return ret;
}

QVariantList ModelAgentBase::outNodes(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Node* n : model()->getNode(d)->findOutNodes(tags, Node::MatchAll))
        ret.append(model()->getHandle(n));
    return ret;
}

QVariantList ModelAgentBase::inNodes(QString tag) const
{
    QVariantList ret;
    idxCheck(ret);
    QStringList tags;
    if (!tag.isEmpty())
        tags = tag.split("|");
    for (Node* n : model()->getNode(d)->findInNodes(tags, Node::MatchAll))
        ret.append(model()->getHandle(n));
    return ret;
}



GraphModel *ModelAgentBase::model() const
{
    return const_cast<GraphModel*>(static_cast<const GraphModel*>(d.model()));
}

void ModelAgentBase::setModelIndex(const QModelIndex &index)
{
    if (d.model())
        invalidate();
    d = index;
    auto model = index.model();
    connect(model, &QAbstractItemModel::dataChanged, this, &ModelAgentBase::onDataChanged/*, Qt::QueuedConnection*/);
//    connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this, &ModelAgentBase::onRowsAboutToBeInserted);
    connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ModelAgentBase::onRowsAboutToBeRemoved);
//    connect(model, &QAbstractItemModel::rowsAboutToBeMoved, this, &ModelAgentBase::onRowsAboutToBeMoved);
    connect(model, &QAbstractItemModel::rowsInserted, this, &ModelAgentBase::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &ModelAgentBase::onRowsRemoved);
//    connect(model, &QAbstractItemModel::rowsMoved, this, &ModelAgentBase::onRowsMoved);
//    connect(model, &QAbstractItemModel::modelReset, this, &ModelAgentBase::onModelReset);
    connect(model, &QAbstractItemModel::modelAboutToBeReset, this, &ModelAgentBase::onModelAboutToBeReset);
    emit modelIndexChanged();
}

void ModelAgentBase::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!d.isValid())
        return;
    if (d.parent() != topLeft.parent())
        return;
    if (d.row() < topLeft.row() || d.row() > bottomRight.row())
        return;
//    qDebug() << "data change" << roles << sender() << this;
    dataChange(roles);
}

void ModelAgentBase::onRowsInserted(const QModelIndex &parent, int, int)
{
    if (!d.isValid())
        return;
    if (parent == d) {
        emit childrenChanged();
    }
}

void ModelAgentBase::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    if (!d.isValid())
        return;
    if (d.parent() != parent)
        return;
    if (d.row() >= first && d.row() <= last) {
        emit isValidChanged(false);
        invalidate();
    }
}

void ModelAgentBase::onRowsRemoved(const QModelIndex &parent, int, int)
{
    if (!d.isValid())
        return;
    if (parent == d) {
        emit childrenChanged();
    }
}

void ModelAgentBase::onModelAboutToBeReset()
{
    invalidate();
}
