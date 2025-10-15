#ifndef SMARTOBJECT_H
#define SMARTOBJECT_H

#include <QObject>
#include <QQmlListProperty>
#include <QQmlParserStatus>
#include <qset.h>
#define AUTO_LIST(Class)                                                              \
Q_PROPERTY(QQmlListProperty<QObject> data READ qmlData)                               \
Q_CLASSINFO("DefaultProperty", "data")                                                \
QQmlListProperty<QObject> qmlData() {                                                 \
    return QQmlListProperty<QObject>(this, 0,                                         \
                                     &Class::qmlDataAppend,                           \
                                     &Class::qmlDataCount,                            \
                                     &Class::qmlDataAt,                               \
                                     QQmlListProperty<QObject>::ClearFunction());     \
}                                                                                     \
static void qmlDataAppend(QQmlListProperty<QObject> *list, QObject *object)           \
{                                                                                     \
    Class *o = qobject_cast<Class*>(list->object);                                    \
    o->appendChild(object);                                                           \
}                                                                                     \
static int qmlDataCount(QQmlListProperty<QObject> *list)                              \
{                                                                                     \
    Class *o = qobject_cast<Class*>(list->object);                                    \
    return o->childCount();                                                           \
}                                                                                     \
static QObject *qmlDataAt(QQmlListProperty<QObject> *list, int index)                 \
{                                                                                     \
    Class *o = qobject_cast<Class*>(list->object);                                    \
    return o->childAt(index);                                                         \
}


class QQmlInstantiator;
class InstantiatorCollection;
class AutoList
{
public:
    AutoList();
    ~AutoList();
    bool complete() const;
    void setComplete(bool complete);
    void appendChild(QObject *child);
    bool removeChild(QObject *child);
    int childCount() const { return m_data.size(); }
    QObject *childAt(int index) const { return m_data.at(index); }

protected:
    void scan(QObject *object, bool defaultPropertyHandlerAccessible = false);
    void handleAdd(QObject *child);
    void handleRemove(QObject *child);
    void insertObject(int index, QObject* object);
    void removeObject(int index, QObject* object);
    void bind(QVector<InstantiatorCollection*> list);


private:
    QList<QObject *> m_data;
    QHash<const QMetaObject*, QQmlListReference> m_lists;
    QSet<const QMetaObject*> handled;
    QQmlListReference m_defaultList;
    QHash<QObject*, QPair<QMetaObject::Connection, QMetaObject::Connection>> m_instantiators;
    bool m_complete : 1;
    bool m_defaultPropertyHandlerAccessible : 1;
    QObject *instance = nullptr;
};

class SmartObject : public QObject, public QQmlParserStatus, public AutoList
{
    Q_OBJECT

    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QQmlListProperty<QObject> data READ qmlData NOTIFY dataChanged)

    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit SmartObject(QObject *parent = 0);
    ~SmartObject();
    void classBegin();
    void componentComplete();


signals:
    void dataChanged();

public slots:


private:

    QQmlListProperty<QObject> qmlData();
    static void qmlDataAppend(QQmlListProperty<QObject> *list, QObject *object);
    static int qmlDataCount(QQmlListProperty<QObject> *list);
    static QObject *qmlDataAt(QQmlListProperty<QObject> *list, int index);

};

#endif // SMARTOBJECT_H
