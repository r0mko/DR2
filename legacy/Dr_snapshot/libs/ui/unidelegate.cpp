#include "unidelegate.h"
#include "uniloader.h"
#include <QQmlContext>

#include <QDebug>
UniDelegate::UniDelegate(QQuickItem *parent) :
    QQuickItem(parent),
    loader(nullptr)
{

}

void UniDelegate::classBegin()
{
//    qDebug() << "ParentItem" << parentItem();
}

void UniDelegate::componentComplete()
{
//    qDebug() << "UniDelegate" << this << parent() << parentItem();
//    return;
//    const auto mo = metaObject();
//    qDebug() << mo->className();
//    for (int i = 0; i < mo->propertyCount(); ++i) {
//        qDebug() << "Property" << i << mo->property(i).name();
//    }
    QQuickItem *parent = parentItem();
    while (parent) {
        loader = qobject_cast<UniLoader*>(parent);
        if (!loader)
            parent = parent->parentItem();
        else
            break;
    }
    if (!loader) {
        qDebug() << "UniDelegate must be constructed as a child of UniLoader!";
        return;
    }
    connect (loader, &UniLoader::valueChanged, this, &UniDelegate::valueChanged);
    connect (loader, &UniLoader::listValueChanged, this, &UniDelegate::listValueChanged);
    connect (loader, &UniLoader::listItemTypeChanged, this, &UniDelegate::listItemTypeChanged);
    connect (loader, &UniLoader::isListChanged, this, &UniDelegate::isListChanged);
    emit valueChanged();
    emit isCompletedChanged(true);
}

QVariant UniDelegate::extraData() const
{
    return m_extraData;
}

QVariant UniDelegate::value() const
{
    if (loader)
        return loader->value();
    else
        return QVariant();
}

bool UniDelegate::isList() const
{
    if (loader)
        return loader->isList();
    else
        return false;
}

QQmlListProperty<QObject> UniDelegate::qmlListValue() const
{
    return loader->qmlListValue();
}

QString UniDelegate::listItemType() const
{
    if (loader)
        return loader->listItemType();
    else return QString();
}

void UniDelegate::setExtraData(QVariant arg)
{
    if (m_extraData == arg)
        return;
    m_extraData = arg;
    emit extraDataChanged();
}

bool UniDelegate::isCompleted() const
{
    return isComponentComplete();
}

void UniDelegate::setValue(QVariant arg)
{
//    qDebug() << "Updading value" << arg;
    if (loader)
        loader->setValue(arg);
}


