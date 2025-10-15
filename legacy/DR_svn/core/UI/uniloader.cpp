#include "uniloader.h"
#include "ui.h"
#include <QDebug>
#include <QMetaType>
#include <QMetaObject>
#include <QQmlContext>
#include <QQmlExpression>


QQmlListProperty<QObject> UniLoader::qmlListValue() {
    return QQmlListProperty<QObject>(this, 0,
                                     &UniLoader::qmlListDataAppend,
                                     &UniLoader::qmlListDataCount,
                                     &UniLoader::qmlListDataAt,
                                     QQmlListProperty<QObject>::ClearFunction());
}

void UniLoader::qmlListDataAppend(QQmlListProperty<QObject> *list, QObject *object)
{
    UniLoader *o = qobject_cast<UniLoader*>(list->object);
    if (o->listRef.canAppend())
        o->listRef.append(object);
}
int UniLoader::qmlListDataCount(QQmlListProperty<QObject> *list)
{
    UniLoader *o = qobject_cast<UniLoader*>(list->object);
    //    qDebug() << "Query list length" << o << o->listRef.isValid();
    //        if (o->listRef.canCount())
    return o->listRef.count();
    //        return 0;
}

QObject *UniLoader::qmlListDataAt(QQmlListProperty<QObject> *list, int index)
{
    UniLoader *o = qobject_cast<UniLoader*>(list->object);
    //    if (o->listRef.canAt())
    //    qDebug() << "Query list index" << index << o->listRef.at(index);

    return o->listRef.at(index);
    //    return 0;
}

UniLoader::UniLoader(QQuickItem *parent) :
    QQuickItem(parent),
    m_target(nullptr),
    m_delegateItem(nullptr),
    m_allowSuperClassDelegates(true)
{

    //    qDebug() << "Core" << QMetaType::type("Core*");
}

UniLoader::~UniLoader()
{
    //    qDebug() << "Destroyed" << this;
}

void UniLoader::classBegin()
{

    //    QQuickItem::classBegin();
}

void UniLoader::componentComplete()
{
    //    updateItem();
}



QVariant UniLoader::value() const
{
    return m_value;
}

TargetDef *UniLoader::target() const
{
    return m_target;
}

QString UniLoader::variant() const
{
    return m_variant;
}

bool UniLoader::allowSuperClassDelegates() const
{
    return m_allowSuperClassDelegates;
}

UniDelegate *UniLoader::delegateItem() const
{
    return m_delegateItem;
}

QString UniLoader::targetName() const
{
    return m_targetName;
}

bool UniLoader::isList() const
{
    return m_isList;
}

QString UniLoader::listItemType() const
{
    return m_listItemType;
}

QString UniLoader::valueSource() const
{
    return m_valueSourceExpression;
}


void UniLoader::setValue(QVariant arg)
{
    //    qDebug() << "Set value" << m_value << arg;
    if (m_value == arg)
        return;
    bool createItem = false;
    if (m_value.userType() != arg.userType()) {
        //        qDebug() << "Value type changed from" << m_value << "to" << arg;
        createItem = true;
    }
    m_value = arg;
    emit valueChanged();
    if (createItem)
        updateItem();
}

void UniLoader::setTarget(TargetDef* arg)
{
    if (m_target == arg)
        return;
    m_target = arg;
    if (m_target->isComplete()) {
        if (m_target->isComplete())
            setTargetName(m_target->targetName());
        emit targetChanged();
        updateItem();
    } else {
        connect(m_target, &TargetDef::buildComplete, this, &UniLoader::completeTarget);
    }

}

void UniLoader::setVariant(QString arg)
{
    if (m_variant == arg)
        return;
    m_variant = arg;
    emit variantChanged();
    updateItem();
}

void UniLoader::setAllowSuperClassDelegates(bool arg)
{
    if (m_allowSuperClassDelegates == arg)
        return;

    m_allowSuperClassDelegates = arg;
    emit allowSuperClassDelegatesChanged();
}

void UniLoader::setTargetName(QString arg)
{
    if (m_targetName == arg)
        return;
    if (UIHelper::targetStorage.contains(arg)) {
        m_target = UIHelper::targetStorage[arg].source;
        m_targetName = arg;
        emit targetChanged();
        emit targetNameChanged();
    } else {
        qWarning() << "Target" << arg << "not defined";
    }
}

void UniLoader::setValueSource(QString arg)
{
    if (m_valueSourceExpression == arg)
        return;

    QStringList argParts = arg.split(".");
    if (argParts.size() > 1) {
        QString propName = argParts.takeLast();
        QQmlExpression objExpr(qmlContext(this), this, argParts.join("."));
        sourceObject = objExpr.evaluate().value<QObject*>();
        if (!sourceObject) {
            qWarning() << "WARNING: no such object" << objExpr.expression();
            return;
        }
        m_valueSource = QQmlProperty(sourceObject, propName);
        if (!m_valueSource.isValid()) {
            qWarning() << "WARNING: no property" << propName << "in object" << sourceObject;
            return;
        }
    }
    updateValue();
    m_valueSource.connectNotifySignal(this, SLOT(updateValue()));
    connect(this, &UniLoader::valueChanged, this, &UniLoader::writeValueSource);
    m_valueSourceExpression = arg;
    emit valueSourceChanged(m_valueSourceExpression);
}

void UniLoader::updateImplicitWidth()
{
    //    qDebug() << "ImplicitWidth" << m_delegateItem->width() << m_delegateItem->implicitWidth();
    setImplicitWidth(m_delegateItem->width());
}

void UniLoader::updateImplicitHeight()
{
    //    qDebug() << "ImplicitHeight" << m_delegateItem->height() << m_delegateItem->implicitHeight();
    setImplicitHeight(m_delegateItem->height());
}

void UniLoader::updateItem()
{
    if (m_delegateItem) {
        m_delegateItem->deleteLater();
        m_delegateItem = nullptr;
    }
    if (!m_target) {
        //        qDebug() << "No target defined!";
        return;
    }
    if (!m_target->isComplete()) {
        qDebug() << "Target incomplete, waiting.";
        return;
    }
    QQmlComponent *comp = nullptr;
    QObject *obj = 0;
    QByteArray typeName;
    QStringList inheritChain;
    QVariant extra;
    if (m_value.isValid()) {
        obj = m_value.value<QObject*>();
        if (obj) {
            const QMetaObject *mo = obj->metaObject();
            do {
                typeName = mo->className();
                comp = getComponentForType(m_target, typeName, m_variant);
                if (!comp)
                    comp = getComponentForType(m_target, typeName + "*", m_variant, &extra);
                if (m_allowSuperClassDelegates && !comp && mo->superClass()) {
                    mo = mo->superClass();
                    inheritChain.prepend(typeName);
                    //                qDebug() << "looking for superclass" << typeName << "<<" << mo->className();
                } else {
                    break;
                }
            } while(comp == nullptr);
        } else {
            typeName = m_value.typeName();
            comp = getComponentForType(m_target, typeName, m_variant, &extra);
        }
    } else {
        typeName = UNKNOWN_TYPE;
        comp = getComponentForType(m_target, typeName, m_variant, &extra);
    }
    if (!comp) {
        QDebug dbg = qDebug().nospace();
        if(obj && m_allowSuperClassDelegates)
            dbg << "WARNING: No delegate found for types " << inheritChain.join(" > ");
        else
            dbg << "WARNING: No delegate found for type " << typeName;
        dbg << " in target " << m_target->targetName();
        m_delegateItem = new UniDelegate(this);
        return;
    }
    m_isList = typeName.startsWith("QQmlListProperty");
    if (m_isList) {
        QRegularExpression rx("QQmlListProperty<(\\w+)>");
        m_listItemType = rx.match(QString(typeName)).captured(1);
        QQmlListProperty<QObject>* list = reinterpret_cast<QQmlListProperty<QObject>*>(m_value.data());
        const QMetaObject *mo = list->object->metaObject();
        for (int i = 0; i < mo->propertyCount(); ++i) {
            QMetaProperty p = mo->property(i);
            if (p.typeName() == typeName) {
                if (p.read(list->object) == m_value) {
                    listRef = QQmlListReference(list->object, p.name());
                    //                    if (listRef.isValid())
                    //                        qDebug() << "List type" << typeName << list->object << "item type" << m_listItemType << listRef.isValid() << listRef.count();
                    break;
                }
            }
        }
        emit isListChanged();
        emit listValueChanged();
        emit listItemTypeChanged();
    }
    //    QObject *parentObj = ctx->parentContext()->contextObject();
    //    if (parentObj && !m_isList) {
    //        if (QByteArray(parentObj->metaObject()->className()).startsWith("QQuickLoader")) {
    //            parentObj->setProperty("sourceComponent", QVariant::fromValue(comp)); // load found component to loader instead of UniDelegate itself
    //            deleteLater();
    //            return;
    //        }
    //    }
    //    qDebug() << "Loader not found, constructing child object";
    QQmlContext *ctx = qmlContext(this);
    m_delegateItem = qobject_cast<UniDelegate*>(comp->beginCreate(ctx));

    if (!m_delegateItem) {
        qWarning() << "WARNING: Delegate must derive from UniDelegate";
        return;
    }

    if (!m_variant.isEmpty() && extra.isValid()) {
        //        qDebug() << "Extra data for variant" << m_variant << extra;
        m_delegateItem->setExtraData(extra);
    }
    m_delegateItem->setParentItem(this);
    m_delegateItem->setParent(this);
    comp->completeCreate();

    connect(m_delegateItem, &QQuickItem::widthChanged, this, &UniLoader::updateImplicitWidth);
    connect(m_delegateItem, &QQuickItem::heightChanged, this, &UniLoader::updateImplicitHeight);
    connect(parentItem(), &QQuickItem::widthChanged, [&](){ setWidth(parentItem()->width()); });
    connect(parentItem(), &QQuickItem::heightChanged, [&](){ setHeight(parentItem()->height()); });

    updateImplicitHeight();
    updateImplicitWidth();
    if (m_delegateItem)
        emit delegateItemChanged();
    else  {
        qDebug() << "No item created for value" << m_value;

        deleteLater();
        return;
    }
    //        qDebug() << "Updated UniLoader" << m_value << m_variant << typeName << width() << height();
}

void UniLoader::completeTarget()
{
    //    qDebug() << "Target" << m_target->targetName() << "is now complete";
    setTargetName(m_target->targetName());
    updateItem();
}

void UniLoader::updateValue()
{
    QVariant newval = m_valueSource.read();
    //    qDebug() << "read value" << sourceObject->property("name").toString() << newval;
    setValue(newval);
}

void UniLoader::writeValueSource()
{
    if (m_valueSource.read() == m_value)
        return;
    qDebug() << "write value" << sourceObject->property("name").toString() << m_value;
    m_valueSource.write(m_value);
}


