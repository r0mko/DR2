#include "weaver.h"
#include <QDebug>


Weaver::Weaver(QObject *parent) : QObject(parent)
{

}

void Weaver::classBegin()
{
}

void Weaver::componentComplete()
{
    qDebug() << "weaver complete" << model() << index();
    tryBind();
}


QString Weaver::typeRole() const
{
    return m_typeRole;
}

QString Weaver::valueRole() const
{
    return m_valueRole;
}

void Weaver::setTypeRole(QString typeRole)
{
    if (m_typeRole == typeRole)
        return;
    m_typeRole = typeRole;
    if (model())
        i_typeRole = findRole(m_typeRole, model());
    emit typeRoleChanged(typeRole);
}

void Weaver::setValueRole(QString valueRole)
{
    if (m_valueRole == valueRole)
        return;

    m_valueRole = valueRole;
    if (model())
        i_valueRole = findRole(m_valueRole, model());
    emit valueRoleChanged(valueRole);
}

QModelIndex Weaver::index() const
{
    return SignalFilter::index();
}

void Weaver::setIndex(QModelIndex index)
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

Weaver::InstantiationMode Weaver::mode() const
{
    return m_mode;
}

void Weaver::setMode(Weaver::InstantiationMode mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged(mode);
}

int Weaver::count() const
{
    return m_gadgets.size() + m_objects.size();
}

QVariant Weaver::instance(const QModelIndex &index) const
{
    //    qDebug() << "query for instance" << index.data(mrlrole) << m_gadgets.contains(index);
    return m_gadgets.value(index);
}


bool Weaver::tryBind()
{
    if (model()) {
        i_typeRole = findRole(m_typeRole, model());
        if (i_typeRole < 0)
            qWarning() << "model has no type role" << m_typeRole << model()->roleNames().values();
        i_valueRole = findRole(m_valueRole, model());
        if (i_valueRole < 0)
            qWarning() << "model has no value role" << m_valueRole << model()->roleNames().values();
        mrlrole = findRole("mrl", model());
        if (i_typeRole > 0 && i_typeRole > 0) {
            disconnect(model(), &QAbstractItemModel::dataChanged, this, &Weaver::tryBind);
            regenerate();
            return true;
        }
    }
    return false;
}

//void Weaver::onSourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
//{

//}

void Weaver::clear()
{
    qDebug() << "clearing" << m_objects.size() << "objects and" << m_gadgets.size() << "gadgets";
    for (QPointer<QObject> p : m_objects) {
        if (!p.isNull()) {
            p->deleteLater();
        }
    }
    m_objects.clear();
    for (Gadget g : m_gadgets) {
        g.destroy();
    }
    m_gadgets.clear();
}

Weaver::Gadget Weaver::buildGadget(const QByteArray &type, const QJsonObject &obj) const
{
    int tid = QMetaType::type(type.constData());
    if (tid == QMetaType::UnknownType) {
        qWarning() << "Unknown type" << type;
        return Gadget();
    }
    Gadget ret;
    ret.type = tid;
    ret.ptr = QMetaType::create(tid);
    if (!ret.ptr) {
        qWarning() << "Failed to create object" << type;
        return Gadget();
    }
    ret.metaObject = QMetaType::metaObjectForType(tid);
    if (!ret.metaObject) {
        qWarning() << type << "is not a Q_GADGET";
        return ret;
    }
    ret.update(obj);
    return ret;
}


bool Weaver::writeValue(const QVariant &value, const QMetaProperty &property, void *gadget)
{
    int ptype = property.userType();
    if (value.canConvert(ptype))
        return property.writeOnGadget(gadget, value);
    qWarning() << "Can't convert value type" << value.typeName() << "into property" << property.name() << "type" << property.typeName();
    return false;
}

bool Weaver::updateValue(const QModelIndex &idx) const
{
    if (!m_gadgets.contains(idx)) {
        qWarning() << "Unknown index" << idx;
        return false;
    }
    QJsonObject val = idx.data(i_valueRole).toJsonObject();
    return m_gadgets[idx].update(val);
}

//void Weaver::bind(QAbstractItemModel *model)
//{
//    qDebug() << "binding model" << model->roleNames().values();
//    connect(model, &QAbstractItemModel::dataChanged, this, &Weaver::onSourceDataChanged, Qt::UniqueConnection);
//    //    connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this, &Weaver::onSourceRowsAboutToBeInserted);
//    connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &Weaver::onSourceRowsAboutToBeRemoved, Qt::UniqueConnection);
//    connect(model, &QAbstractItemModel::rowsAboutToBeMoved, this, &Weaver::onSourceRowsAboutToBeMoved, Qt::UniqueConnection);
//    connect(model, &QAbstractItemModel::rowsInserted, this, &Weaver::onSourceRowsInserted, Qt::UniqueConnection);
//    connect(model, &QAbstractItemModel::rowsRemoved, this, &Weaver::onSourceRowsRemoved, Qt::UniqueConnection);
//    //    connect(model, &QAbstractItemModel::rowsMoved, this, &Weaver::onSourceRowsMoved);
//    connect(model, &QAbstractItemModel::modelReset, this, &Weaver::onSourceModelReset, Qt::UniqueConnection);
//    connect(model, &QAbstractItemModel::modelAboutToBeReset, this, &Weaver::onSourceModelAboutToBeReset, Qt::UniqueConnection);

//}

//void Weaver::unbind(QAbstractItemModel *model)
//{
//    i_typeRole = -1;
//    i_valueRole = -1;
//    model->disconnect(0, this, 0);
//    connect(model, &QAbstractItemModel::dataChanged, this, &Weaver::tryBind);

//    //    connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this, &Weaver::onSourceRowsAboutToBeInserted);
//    disconnect(model, &QAbstractItemModel::dataChanged, this, &Weaver::onSourceDataChanged);
//    disconnect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &Weaver::onSourceRowsAboutToBeRemoved);
//    disconnect(model, &QAbstractItemModel::rowsAboutToBeMoved, this, &Weaver::onSourceRowsAboutToBeMoved);
//    disconnect(model, &QAbstractItemModel::rowsInserted, this, &Weaver::onSourceRowsInserted);
//    disconnect(model, &QAbstractItemModel::rowsRemoved, this, &Weaver::onSourceRowsRemoved);
//    disconnect(model, &QAbstractItemModel::modelReset, this, &Weaver::onSourceModelReset);
//    disconnect(model, &QAbstractItemModel::modelAboutToBeReset, this, &Weaver::onSourceModelAboutToBeReset);
//    //    connect(model, &QAbstractItemModel::rowsMoved, this, &Weaver::onSourceRowsMoved);
//    //    model->disconnect(0, this, 0);
//}

int Weaver::findRole(QString role, const QAbstractItemModel *model)
{
    QHash<int, QByteArray> roles = model->roleNames();
    return roles.key(role.toLocal8Bit(), -1);
}


void Weaver::regenerate()
{
    clear();
    build(index());
    //    for
}

void Weaver::build(const QModelIndex &parent, int first, int last)
{
    if (last < 0)
        last = model()->rowCount(parent) - 1;
    if (last-first >= 0)
//        qDebug() << "building" << last - first + 1 << "gadgets in parent" << parent.data(mrlrole) << m_mode;
    if (!parent.isValid()) {
        qWarning() << "creating node in root index not supported";
        return;
    }
    for (int i = first; i <= last; ++i) {
        QModelIndex cIdx = parent.child(i, 0);
        if (m_gadgets.contains(cIdx))
            m_gadgets[cIdx].destroy();
        QString type = cIdx.data(i_typeRole).toString();
        QJsonObject attr = cIdx.data(i_valueRole).toJsonObject();
        if (!type.isEmpty()) {
            Gadget g = buildGadget(type.toLocal8Bit(), attr);
            if (g.ptr) {
                m_gadgets.insert(cIdx, g);
                Gadget pg = m_gadgets.value(parent);
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

void Weaver::remove(const QModelIndex &parent, int first, int last)
{
    if (last < 0)
        last = model()->rowCount(parent);
    for (int i = first; i <= last; ++i) {
        QModelIndex cIdx = model()->index(i, 0, parent);
        if (m_gadgets.contains(cIdx)) {
            Gadget g = m_gadgets.take(cIdx);
            Gadget pg = m_gadgets.value(parent);
            emit objectRemoved(g, pg, i);
            g.destroy();
        }
    }
}

bool Weaver::Gadget::update(const QJsonObject &attr, const QString &key) const
{
//    qDebug() << "Updating property" << attr.toVariantMap() << operator QVariant();
    bool result = true;
    if (key.isEmpty()) {
        for (int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
            QMetaProperty property = metaObject->property(i);
            if (!attr.contains(property.name()))
                continue;
            QVariant v = attr[property.name()].toVariant();
            // TODO: notify changed values
            result &= property.writeOnGadget(ptr, v);
        }
    } else {
        if (!attr.contains(key))
            return false;
        QMetaProperty property = metaObject->property(metaObject->indexOfProperty(key.toLocal8Bit().constData()));
        result = property.writeOnGadget(ptr, attr.value(key).toVariant());
    }
    return result;
}

void Weaver::Gadget::destroy()
{
    if(!ptr)
        return;
    QMetaType::destroy(type, ptr);
    ptr = nullptr;
    type = QMetaType::UnknownType;
    metaObject = nullptr;
}


void Weaver::onModelAboutToBeReset()
{
    qDebug() << "source model about to reset";
}

void Weaver::onModelReset()
{
    qDebug() << "source model was reset";
}

void Weaver::onRowsInserted(ModeFlags mode, const QModelIndex &parent, int first, int last)
{
    qDebug() << "added" << last - first << "rows";
    build(parent, first, last);
}

void Weaver::onRowsMoved(ModeFlags mode, const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    if (m_gadgets.contains(parent) && m_gadgets.contains(destination)) {
        qDebug() << "moved" << start- end << "rows" << "to" << row;
        emit objectsMoved(m_gadgets.value(parent), start, end, m_gadgets.value(destination), row);
    }
}

void Weaver::onRowsAboutToBeRemoved(ModeFlags mode, const QModelIndex &parent, int first, int last)
{
    qDebug() << "removed" << last - first + 1 << "rows from" << parent << first << last;
    remove(parent, first, last);
}

void Weaver::onDataChanged(SignalFilter::ModeFlags mode, const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{

//    qDebug() << "Updated value" << topLeft.data(mrlrole) << roles << topLeft.data(i_valueRole);
//    for (int role: roles) {
//        qDebug() << model()->roleNames().value(role);
//    }
    if (roles.contains(i_valueRole)) {
        updateValue(topLeft);
        emit valueChanged(topLeft, m_gadgets.value(topLeft));
    }
}
