#include "scenemodel.h"
#include "guideview.h"
#include "qmetaobject.h"
#include "scene.h"
#include <QGraphicsScene>
#include <QAbstractItemModel>
#include <QJsonObject>
//#include "path.h"
SceneModel::SceneModel(QObject *parent) : QAbstractItemModel(parent)
{

}

void SceneModel::classBegin() { }



void SceneModel::componentComplete()
{
    qDebug() << "SceneModel complete" << model() << index();
    connect(m_roleBindings, &QQmlPropertyMap::valueChanged, this, &SceneModel::addRoleBinding);
    tryBind();
}

QString SceneModel::typeRole() const
{
    return m_typeRole;
}

void SceneModel::setTypeRole(QString typeRole)
{
    if (m_typeRole == typeRole)
        return;
    m_typeRole = typeRole;
    if (model())
        i_typeRole = findRole(m_typeRole, model());
    emit typeRoleChanged(typeRole);
}

QString SceneModel::valueRole() const
{
    return m_valueRole;
}

void SceneModel::setValueRole(QString valueRole)
{
    if (m_valueRole == valueRole)
        return;

    m_valueRole = valueRole;
    if (model())
        i_valueRole = findRole(m_valueRole, model());
    emit valueRoleChanged(valueRole);
}

QModelIndex SceneModel::index() const
{
    return SignalFilter::index();
}

void SceneModel::setIndex(QModelIndex index)
{
    //    qDebug() << "set index" << index << index.model();
    if (index == SignalFilter::index())
        return;
    if (initFilter(index, All)) {
        emit indexChanged(index);
        emit modelChanged();
        tryBind();
        //        regenerate();
    } else {
        qDebug() << "phailed to init filter";
    }
}

SceneModel::InstantiationMode SceneModel::mode() const
{
    return m_mode;
}

void SceneModel::setMode(SceneModel::InstantiationMode mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged(mode);
}

int SceneModel::count() const
{
    return idx2instance.size();
}

QVariant SceneModel::instance(const QModelIndex &index) const
{
    //    qDebug() << "query for instance" << index.data(mrlrole) << m_gadgets.contains(index);
    return idx2instance.value(index);
}

bool SceneModel::tryBind()
{
    if (model()) {

        i_typeRole = findRole(m_typeRole, model());
        if (i_typeRole < 0)
            qWarning() << "model has no type role" << m_typeRole << model()->roleNames().values();
        i_valueRole = findRole(m_valueRole, model());
        if (i_valueRole < 0)
            qWarning() << "model has no value role" << m_valueRole << model()->roleNames().values();
        for (QString k : m_roleBindings->keys()) {
            int r = findRole(k, model());
            if (r < 0)
                qWarning() << "model has no role" << k;
            else
                i_boundRoles.insert(r, k.toLocal8Bit());
        }
        if (i_typeRole > 0 && i_typeRole > 0) {
            disconnect(model(), &QAbstractItemModel::dataChanged, this, &SceneModel::tryBind);
            regenerate();
            return true;
        }
    }
    return false;
}

void SceneModel::clearRoleBindings()
{
    m_roleBindings->deleteLater();
    m_roleBindings = new QQmlPropertyMap(this);
    emit roleBindingsChanged();
}

//QVariant SceneModel::addItem(const QString &type, QJsonObject params)
//{
//    Q_ASSERT(m_scene != nullptr);
//    Instance i = createInstance(type.toLocal8Bit(), params);
//    VisualInstance *item = i.handle()->item();
//    if (item) {
//        i.update(params);
//        m_scene->addItem(item);
//        qDebug() << "added item" << item << "to scene" << m_scene;
//        item2instance.insert(item, i);
//    }
//    return i;
//}

QVariant SceneModel::addItem(const QString &type, const QVariantMap &params, const QVariant &parent)
{
    Q_ASSERT(m_scene != nullptr);
    Instance i = createInstance(type.toLocal8Bit(), params);

    VisualInstance *item = i.handle()->item();
    if (item) {
        item->setData(VisualInstance::QmlEngineKey, QVariant::fromValue(qmlEngine(this)));
        i.update(params);
        VisualInstance *parentItem{nullptr};

        if (parent.canConvert<VisualInstanceHandle>()) {
            VisualInstanceHandle p = parent.value<VisualInstanceHandle>();
            parentItem = p.item();
            if( parentItem == nullptr ){
                beginInsertRows(QModelIndex(), m_rootChildren.size(), m_rootChildren.size());
                auto idx = QPersistentModelIndex( createIndex( m_rootChildren.size(), 0, item ) );
                item->m_index = idx;
                m_rootChildren.append(item);
            }else{
                beginInsertRows(parentItem->m_index, parentItem->childItems().size(), parentItem->childItems().size() );
                auto idx = QPersistentModelIndex( createIndex( parentItem->childItems().size(), 0, item ) );
                item->m_index = idx;
                m_rootChildren.append(item);
            }
            item->setParentItem(parentItem);
        } else if (parent.canConvert<QGraphicsObject*>()) {
            QGraphicsObject *pobj = parent.value<QGraphicsObject*>();
            beginInsertRows(QModelIndex(), m_rootChildren.size(), m_rootChildren.size());
            auto idx = QPersistentModelIndex( createIndex( m_rootChildren.size(), 0, item ) );
            item->m_index = idx;
            m_rootChildren.append(item);
            qDebug() << "Setting parent" << pobj;
            if (pobj)
                item->setParentItem(pobj);
        } else {
            beginInsertRows(QModelIndex(), m_rootChildren.size(), m_rootChildren.size());
            auto idx = QPersistentModelIndex( createIndex( m_rootChildren.size(), 0, item ) );
            item->m_index = idx;

            m_rootChildren.append(item);
            if (parent.isValid())
                qWarning() << parent << "is not a QGraphicsItem handle!";
        }
        if (!item->parentItem())
            m_scene->addItem(item);

//        if (ret.ptr && ret.metaObject)
        item2instance.insert(item, i);
        endInsertRows();
        // TODO: also insert into model
    }
    return i;
}

void SceneModel::setScene(QGraphicsScene *scene)
{
    if (m_scene == scene)
        return;

    m_scene = scene;
    emit sceneChanged();
}

QGraphicsScene *SceneModel::scene() const
{
    return m_scene;
}

QAbstractItemModel *SceneModel::sourceModel() const
{
    return m_sourceModel;
}

void SceneModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (m_sourceModel == sourceModel)
        return;

    m_sourceModel = sourceModel;
    emit sourceModelChanged(sourceModel);
}

QQmlPropertyMap *SceneModel::roleBindings() const
{
    return m_roleBindings;
}

void SceneModel::clear()
{
    qDebug() << "clearing" << idx2instance.size() << "item instances";

    for (Instance g : idx2instance) {
        g.destroy();
    }
    idx2instance.clear();
}

SceneModel::Instance SceneModel::createInstance(const QByteArray &type, const QVariantMap &obj) const
{
    int tid = handleType(type);
    if (tid == QMetaType::UnknownType) {
        qWarning() << "Unknown item type" << type;
        return Instance();
    }
    Instance ret = createInstanceHelper(tid);
    if (ret.handle()->item() != nullptr)
        ret.update(obj);
    return ret;
}

SceneModel::Instance SceneModel::createInstanceHelper(int type) const
{
    Instance ret;
    ret.type = type;
    ret.ptr = QMetaType::create(type);
    if (!ret.ptr) {
        qWarning() << "Failed to create item" << type;
        return Instance();
    }
    ret.metaObject = QMetaType::metaObjectForType(type);
    if (!ret.metaObject) {
        qWarning() << QMetaType::typeName(type) << "is not Q_GADGET";
        return ret;
    }
    return ret;
}


bool SceneModel::writeValue(const QVariant &value, const QMetaProperty &property, void *Instance)
{
    int ptype = property.userType();
    if (value.canConvert(ptype))
        return property.writeOnGadget(Instance, value);
    qWarning() << "Can't convert value type" << value.typeName() << "into property" << property.name() << "type" << property.typeName();
    return false;
}

bool SceneModel::updateValue(const QModelIndex &idx) const
{
    if (!idx2instance.contains(idx)) {
        qWarning() << "Unknown index" << idx;
        return false;
    }
    QVariantMap val = idx.data(i_valueRole).toMap();
    return idx2instance[idx].update(val);
}

//void SceneModel::bind(QAbstractItemModel *model)
//{
//    qDebug() << "binding model" << model->roleNames().values();
//    connect(model, &QAbstractItemModel::dataChanged, this, &SceneModel::onSourceDataChanged, Qt::UniqueConnection);
//    //    connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this, &SceneModel::onSourceRowsAboutToBeInserted);
//    connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &SceneModel::onSourceRowsAboutToBeRemoved, Qt::UniqueConnection);
//    connect(model, &QAbstractItemModel::rowsAboutToBeMoved, this, &SceneModel::onSourceRowsAboutToBeMoved, Qt::UniqueConnection);
//    connect(model, &QAbstractItemModel::rowsInserted, this, &SceneModel::onSourceRowsInserted, Qt::UniqueConnection);
//    connect(model, &QAbstractItemModel::rowsRemoved, this, &SceneModel::onSourceRowsRemoved, Qt::UniqueConnection);
//    //    connect(model, &QAbstractItemModel::rowsMoved, this, &SceneModel::onSourceRowsMoved);
//    connect(model, &QAbstractItemModel::modelReset, this, &SceneModel::onSourceModelReset, Qt::UniqueConnection);
//    connect(model, &QAbstractItemModel::modelAboutToBeReset, this, &SceneModel::onSourceModelAboutToBeReset, Qt::UniqueConnection);

//}

//void SceneModel::unbind(QAbstractItemModel *model)
//{
//    i_typeRole = -1;
//    i_valueRole = -1;
//    model->disconnect(0, this, 0);
//    connect(model, &QAbstractItemModel::dataChanged, this, &SceneModel::tryBind);

//    //    connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this, &SceneModel::onSourceRowsAboutToBeInserted);
//    disconnect(model, &QAbstractItemModel::dataChanged, this, &SceneModel::onSourceDataChanged);
//    disconnect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &SceneModel::onSourceRowsAboutToBeRemoved);
//    disconnect(model, &QAbstractItemModel::rowsAboutToBeMoved, this, &SceneModel::onSourceRowsAboutToBeMoved);
//    disconnect(model, &QAbstractItemModel::rowsInserted, this, &SceneModel::onSourceRowsInserted);
//    disconnect(model, &QAbstractItemModel::rowsRemoved, this, &SceneModel::onSourceRowsRemoved);
//    disconnect(model, &QAbstractItemModel::modelReset, this, &SceneModel::onSourceModelReset);
//    disconnect(model, &QAbstractItemModel::modelAboutToBeReset, this, &SceneModel::onSourceModelAboutToBeReset);
//    //    connect(model, &QAbstractItemModel::rowsMoved, this, &SceneModel::onSourceRowsMoved);
//    //    model->disconnect(0, this, 0);
//}



int SceneModel::row(QGraphicsItem *item) const
{
    if( item->parentItem() == nullptr )
        return m_rootChildren.indexOf(item);
    else
        return item->parentItem()->childItems().indexOf(item);
}

QModelIndex SceneModel::index(int row, int column, const QModelIndex &parent) const {
    if(!hasIndex(row, column, parent)) return QModelIndex();

    QGraphicsItem *parentItem{nullptr};
    if(parent.isValid())
        parentItem = static_cast<QGraphicsItem*>(parent.internalPointer());

    QGraphicsItem *childItem{nullptr};
    if( parentItem != nullptr )
        childItem = parentItem->childItems().at(row);
    else
        childItem = m_rootChildren.at(row);

    if (childItem != nullptr)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex SceneModel::parent(const QModelIndex &child) const {
    if (!child.isValid()) return QModelIndex();

    QGraphicsItem *childItem = static_cast<QGraphicsItem*>(child.internalPointer());
    QGraphicsItem *parentItem = childItem->parentItem();

    if (parentItem == nullptr)
        return QModelIndex();

    int row{0};
    if( parentItem->parentItem() == nullptr )
        row = m_rootChildren.indexOf(parentItem);
    else
        row = parentItem->parentItem()->childItems().indexOf(parentItem);
    return createIndex(row, 0, parentItem); // is it right?
}

int SceneModel::rowCount(const QModelIndex &parent) const {
    QGraphicsItem *parentItem{nullptr};
    if (parent.isValid())
        parentItem = static_cast<QGraphicsItem*>(parent.internalPointer());

    if( parentItem == nullptr )
        return m_rootChildren.size();
    else
        return parentItem->childItems().size();
}

int SceneModel::columnCount(const QModelIndex &parent) const {
    return 1;
}

QVariant SceneModel::data(const QModelIndex &index, int role) const {
    //        if (!index.isValid())
    //            return QVariant();

    //        if (role != Qt::DisplayRole)
    //            return QVariant();

    //        Instance *item = static_cast<Instance*>(index.internalPointer());

    //        return item->data(index.column());
}


void SceneModel::regenerate()
{
    clear();
    build(index());
    //    for
}

void SceneModel::build(const QModelIndex &parent, int first, int last)
{
    if (last < 0)
        last = model()->rowCount(parent) - 1;
    //    if (last-first >= 0)
    //        qDebug() << "building" << last - first + 1 << "Instances in parent" << parent.data(mrlrole) << m_mode;
    if (!parent.isValid()) {
        qWarning() << "creating node in root index is not supported";
        return;
    }
    for (int i = first; i <= last; ++i) {
        QModelIndex cIdx = parent.child(i, 0);
        if (idx2instance.contains(cIdx))
            idx2instance[cIdx].destroy();
        QString type = cIdx.data(i_typeRole).toString();
        QVariantMap attr = cIdx.data(i_valueRole).toMap();
        if (!type.isEmpty()) {
            Instance g = createInstance(type.toLocal8Bit(), attr);
            if (g.ptr) {
                idx2instance.insert(cIdx, g);
                Instance pg = idx2instance.value(parent);
                emit objectAdded(g, pg, i);
                if (m_mode == Tree) {
                    if (model()->rowCount(cIdx) > 0) {
//                        qDebug() << "Building recursive in" << cIdx.data(mrlrole);
                        build(cIdx);
                    }
                }
            }
        }
    }
}

void SceneModel::remove(const QModelIndex &parent, int first, int last)
{
    if (last < 0)
        last = model()->rowCount(parent);
    for (int i = first; i <= last; ++i) {
        QModelIndex cIdx = model()->index(i, 0, index());
        if (idx2instance.contains(cIdx)) {
            Instance g = idx2instance.take(cIdx);
            Instance pg = idx2instance.value(parent);
            emit objectRemoved(g, pg, i);
            g.destroy();
        }
    }
}

bool SceneModel::Instance::update(const QVariantMap &attr) const
{
    if (attr.isEmpty())
        return true;
    bool result = true;
    for (int i = 0; i < metaObject->propertyCount(); ++i) {
        QMetaProperty property = metaObject->property(i);
        if (!attr.contains(property.name()))
            continue;
        QVariant v = attr[property.name()];
        // TODO: notify changed values
        result &= property.writeOnGadget(ptr, v);
        if (!result)
            qDebug() << "writing property" << property.typeName() << property.name() << "failed";
    }

//        if (!attr.contains(key))
//            return false;
//        QMetaProperty property = metaObject->property(metaObject->indexOfProperty(key.toLocal8Bit().constData()));
//        result = property.writeOnGadget(ptr, attr.value(key));
    return result;
}

void SceneModel::Instance::destroy()
{
    if(!ptr)
        return;
    QMetaType::destroy(type, ptr);
    ptr = nullptr;
    type = QMetaType::UnknownType;
    metaObject = nullptr;
}


void SceneModel::onModelAboutToBeReset()
{
    qDebug() << "source model about to reset";
}

void SceneModel::onModelReset()
{
    qDebug() << "source model was reset";
}

void SceneModel::onRowsInserted(ModeFlags mode, const QModelIndex &parent, int first, int last)
{
    qDebug() << "added" << last - first << "rows";
    build(parent, first, last);
}

void SceneModel::onRowsMoved(ModeFlags mode, const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    if (idx2instance.contains(parent) && idx2instance.contains(destination)) {
        qDebug() << "moved" << start- end << "rows" << "to" << row;
        emit objectsMoved(idx2instance.value(parent), start, end, idx2instance.value(destination), row);
    }
}

void SceneModel::onRowsRemoved(ModeFlags mode, const QModelIndex &parent, int first, int last)
{
    qDebug() << "removed" << last - first << "rows";
    remove(parent, first, last);
}

void SceneModel::onDataChanged(SignalFilter::ModeFlags mode, const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{

//    qDebug() << "Updated value" << topLeft.data(mrlrole) << roles << topLeft.data(i_valueRole);
//    for (int role: roles) {
//        qDebug() << model()->roleNames().value(role);
//    }
    if (roles.contains(i_valueRole)) {
        updateValue(topLeft);
        emit valueChanged(topLeft, idx2instance.value(topLeft));
    }
}

void SceneModel::addRoleBinding(const QString &key, const QVariant &value)
{
    qDebug() << "added binding" << key << value;
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
