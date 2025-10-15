#ifndef MODELOBJECT_H
#define MODELOBJECT_H
#include "dynamicobjectextension.h"

#include <QAbstractItemModel>
#include <QJSValue>
#include <QVector>
#include <graphindex.h>

class GraphModel;

class ModelAgentBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(QVariantList children READ children NOTIFY childrenChanged)
    Q_PROPERTY(QModelIndex modelIndex READ modelIndex WRITE setModelIndex)

public:
    ModelAgentBase(QObject *parent = 0) : QObject(parent) { }
    virtual ~ModelAgentBase() {}
    void setModelIndex(const QModelIndex &index);
    inline bool isValid() const { return d.isValid();}
    QVariantList children() const;
    inline QPersistentModelIndex modelIndex() const { return d; }
public slots:
    QVariantList outEdges(QString tag  = QString()) const;
    QVariantList inEdges(QString tag = QString()) const;
    QVariantList outNodes(QString tag = QString()) const;
    QVariantList inNodes(QString tagFilter = QString()) const;

protected:
    GraphModel *model() const;
    virtual void dataChange(const QVector<int> &) {}
    virtual void invalidate() {}
    QPersistentModelIndex d;
    GraphModel *g_model = nullptr;

private slots:
    void onDataChanged          (const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
//    void onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onRowsInserted         (const QModelIndex &parent, int, int);
    void onRowsAboutToBeRemoved (const QModelIndex &parent, int first, int last);
    void onRowsRemoved          (const QModelIndex &parent, int, int);
//    void onRowsAboutToBeMoved   (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);
//    void onRowsMoved            (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);
//    void onModelReset           ();
    void onModelAboutToBeReset  ();

signals:
    void isValidChanged(bool isValid);
    void childrenChanged();
};

class ModelAgent : public DynamicObjectExtension<ModelAgentBase>
{
public:

    struct proto {
        mutable QMetaObject *metaObject = nullptr;
        mutable QHash<int, int> role2signal;
        mutable QHash<int, int> prop2role;
        inline int getRole(int propertyId) const { return prop2role.value(propertyId); }
        void addMapping(int role, int propertyIndex, int notifierIndex) const;
        static void qt_static_metacall(QObject *o, QMetaObject::Call c, int id, void **argv)
        {
            qDebug() << "handled static metacall" << o << c << id;
            if (o)
                o->qt_metacall(c, id, argv);
        }
        bool build(QHash<int, QByteArray> roles, QHash<int, QByteArray> roleTypes, const QAbstractItemModel *model, bool strictTypes = false) const;
    };

    ModelAgent(const QModelIndex &idx, const proto &prototype, QObject *parent = 0);
    void invalidate();
    ~ModelAgent() {}

protected:
    void dataChange(const QVector<int> &roles);
    void setProperty(int id, QVariant value);
    QVariant getProperty(int index) const;
    QVariant callMethod(int, QVariantList) { return QVariant(); }
    void connectNotify(const QMetaMethod &);
    void disconnectNotify(const QMetaMethod &);

private:
    int connects = 0;
    const proto *_proto = nullptr;
};

Q_DECLARE_METATYPE(ModelAgent*)
#endif // MODELOBJECT_H
