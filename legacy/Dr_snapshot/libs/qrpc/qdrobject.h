#ifndef QDROBJECT_H
#define QDROBJECT_H

#include <QObject>
#include <QQmlListProperty>

class DRObject : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "children")
    Q_PROPERTY(QQmlListProperty<QObject> children READ qmlChildren NOTIFY childrenChanged)

public:
    explicit DRObject(QObject *parent = 0);
    QQmlListProperty<QObject> qmlChildren();

signals:
    void childrenChanged();

public slots:
    void appendChild(QObject *object);
    int removeChild(QObject *object);

private:
    static void qmlAppendChild(QQmlListProperty<QObject> *list, QObject *child);
    static int qmlChildCount(QQmlListProperty<QObject> *list);
    static QObject *qmlChildAt(QQmlListProperty<QObject> *list, int index);
    QList<QObject*> m_children;
};

#endif // QDROBJECT_H
