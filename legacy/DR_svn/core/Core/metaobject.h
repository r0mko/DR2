#ifndef METAOBJECT_H
#define METAOBJECT_H

#include <QObject>
#include "metaenum.h"
#include "metamethod.h"
#include "metaproperty.h"
#include <QtQml>

class MetaObject : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<MetaMethod> methods READ qmlListMethods NOTIFY methodsChanged)
    Q_PROPERTY(QQmlListProperty<MetaProperty> properties READ qmlListProperties NOTIFY propertiesChanged)
    Q_PROPERTY(QQmlListProperty<MetaEnum> enums READ qmlListEnums NOTIFY enumsChanged)

    Q_PROPERTY(QString className READ className NOTIFY classNameChanged STORED false)
    Q_PROPERTY(MetaObject* superClass READ superClass NOTIFY superClassChanged STORED false)
    Q_PROPERTY(int methodOffset READ methodOffset NOTIFY methodOffsetChanged STORED false)
    Q_PROPERTY(int enumeratorOffset READ enumeratorOffset NOTIFY enumeratorOffsetChanged STORED false)
    Q_PROPERTY(int propertyOffset READ propertyOffset NOTIFY propertyOffsetChanged STORED false)

    Q_PROPERTY(QObject* object READ object WRITE setObject NOTIFY objectChanged)

public:
    MetaObject(QObject *parent = 0);
    MetaObject(const QMetaObject *metaObject, QObject *parent = 0);
    QString className() const { return m_mobj->className(); }
    MetaObject* superClass();

    int methodOffset() const { return m_mobj->methodOffset(); }
    int enumeratorOffset() const { return m_mobj->enumeratorOffset(); }
    int propertyOffset() const { return m_mobj->propertyOffset(); }

    QQmlListProperty<MetaMethod> qmlListMethods();
    QQmlListProperty<MetaProperty> qmlListProperties();
    QQmlListProperty<MetaEnum> qmlListEnums();
    void classBegin() {}
    void componentComplete();
    QObject* object() const;

signals:
    void methodsChanged();
    void objectChanged();
    void classNameChanged();
    void superClassChanged();
    void methodOffsetChanged();
    void enumeratorOffsetChanged();
    void propertyOffsetChanged();
    void propertiesChanged();
    void enumsChanged();

public slots:
    void setObject(QObject* arg);
    void test();

private:
    void init(const QMetaObject *mo);

    static MetaMethod *qmlMetaMethodAt(QQmlListProperty<MetaMethod> *list, int index);
    static MetaProperty *qmlMetaPropertyAt(QQmlListProperty<MetaProperty> *list, int index);
    static MetaEnum *qmlMetaEnumAt(QQmlListProperty<MetaEnum> *list, int index);

    static int qmlMetaMethodCount(QQmlListProperty<MetaMethod> *list);
    static int qmlMetaPropertyCount(QQmlListProperty<MetaProperty> *list);
    static int qmlMetaEnumCount(QQmlListProperty<MetaEnum> *list);

    QList<MetaMethod*> m_methods;
    QList<MetaProperty*> m_properties;
    QList<MetaEnum*> m_enums;
    const QMetaObject* m_mobj;
    QObject* m_object;
    QString m_className;
    mutable MetaObject* m_superClass;
    int m_methodOffset;
    int m_enumeratorOffset;
    int m_propertyOffset;
};

#endif // METAOBJECT_H
