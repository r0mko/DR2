#include "targetdef.h"
#include <QDebug>
#include "ui.h"

TargetDef::TargetDef(QObject *parent) :
    QObject(parent),
    m_isComplete(false)
{
}


QQmlListProperty<TypeDelegate> TargetDef::qmlDefs()
{
    return QQmlListProperty<TypeDelegate>(this, 0, &TargetDef::qmlAppendDelegate,
                                          &TargetDef::qmlDelegatesCount,
                                          &TargetDef::qmlDelegateAt,
                                          QQmlListProperty<TypeDelegate>::ClearFunction());
}

QString TargetDef::targetName() const
{
    return m_targetName;
}

void TargetDef::componentComplete()
{
    if (m_targetName.isEmpty()) {
        qWarning() << "Target name can not be empty";
        return;
    }
    for (TypeDelegate *d:m_delegates) {
        qDebug() << "Registering type" << d->typeName() << "for" << m_targetName << d->variantName() << "component" << d->component();
        if (d->component()) {
            if (d->variantNames().isEmpty())
                registerDelegateComponent(this, m_targetName, d->typeName(), d->component(), d->variantName(), d->extraData());
            else {
                for (QString vname:d->variantNames()) {
                    qDebug() << "Registering variant" << vname;
                    registerDelegateComponent(this, m_targetName, d->typeName(), d->component(), vname, d->extraData());
                }
            }
        }
    }
    m_isComplete = true;
    emit buildComplete();
}

void TargetDef::setTargetName(QString arg)
{
    if (m_targetName == arg)
        return;

    m_targetName = arg;
    emit targetNameChanged();
}

void TargetDef::qmlAppendDelegate(QQmlListProperty<TypeDelegate> *list, TypeDelegate *delegate)
{
    TargetDef *obj = qobject_cast<TargetDef *>(list->object);
    obj->m_delegates.append(delegate);
    obj->delegatesChanged();
}

int TargetDef::qmlDelegatesCount(QQmlListProperty<TypeDelegate> *list)
{
    TargetDef *obj = qobject_cast<TargetDef *>(list->object);
    return obj->m_delegates.size();
}

TypeDelegate *TargetDef::qmlDelegateAt(QQmlListProperty<TypeDelegate> *list, int index)
{
    TargetDef *obj = qobject_cast<TargetDef *>(list->object);
    return obj->m_delegates.value(index, nullptr);
}
bool TargetDef::isComplete() const
{
    return m_isComplete;
}

TypeDelegate::TypeDelegate() :
    m_component(nullptr)
{}

QString TypeDelegate::typeName() const
{
    return m_typeName;
}

QQmlComponent *TypeDelegate::component() const
{
    return m_component;
}

QString TypeDelegate::variantName() const
{
    return m_variantName;
}

QVariant TypeDelegate::extraData() const
{
    return m_extraData;
}

void TypeDelegate::setTypeName(QString arg)
{
    if (m_typeName == arg)
        return;

    m_typeName = arg;
    emit typeNameChanged();
}

void TypeDelegate::setComponent(QQmlComponent *arg)
{
    if (m_component == arg)
        return;
    m_component = arg;
    emit componentChanged();
}

void TypeDelegate::setVariantName(QString arg)
{
    if (m_variantName == arg)
        return;
    m_variantName = arg;
    emit variantNameChanged();
}

void TypeDelegate::setExtraData(QVariant arg)
{
    if (m_extraData == arg)
        return;

    m_extraData = arg;
    emit extraDataChanged();
}

QStringList TypeDelegate::variantNames() const
{
    return m_variantNames;
}

void TypeDelegate::setVariantNames(QStringList arg)
{
    if (m_variantNames == arg)
        return;
    m_variantNames = arg;
    emit variantNamesChanged();
}
