#ifndef WEAVER_H
#define WEAVER_H

#include <QAbstractItemModel>
#include <QObject>
#include <QQmlComponent>
#include <QPointer>
#include <QJsonObject>
#include "signalfilter.h"

class Weaver : public QObject, public SignalFilter, public QQmlParserStatus
{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
//    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QModelIndex index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(QString typeRole READ typeRole WRITE setTypeRole NOTIFY typeRoleChanged)
    Q_PROPERTY(QString valueRole READ valueRole WRITE setValueRole NOTIFY valueRoleChanged)
    Q_PROPERTY(InstantiationMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)


public:
    enum InstantiationMode {
        None =      0x1,
        Node =      0x2,
        Children =  0x4,
        Tree =      0x8
    };
    Q_ENUM(InstantiationMode)
    explicit Weaver(QObject *parent = 0);

    void classBegin();
    void componentComplete();

    QString typeRole() const;
    void setTypeRole(QString typeRole);

    QString valueRole() const;
    void setValueRole(QString valueRole);

    QModelIndex index() const;
    void setIndex(QModelIndex index);

    InstantiationMode mode() const;
    void setMode(InstantiationMode mode);

    int count() const;


    Q_INVOKABLE QVariant instance(const QModelIndex &index) const;
signals:
    void modelChanged();
    void typeRoleChanged(QString typeRole);
    void valueRoleChanged(QString valueRole);
    void objectAdded(QVariant object, QVariant parent, int index);
    void objectRemoved(QVariant object, QVariant parent, int index);
    void objectsMoved(QVariant source, int first, int last, QVariant destination, int index);
    void indexChanged(const QModelIndex &index);
    void modeChanged(InstantiationMode mode);
    void countChanged();
    void valueChanged(const QModelIndex &index, const QVariant &instance);

public slots:
    bool tryBind();


protected:
    void onModelAboutToBeReset();
    void onModelReset();
    void onRowsInserted(ModeFlags mode, const QModelIndex &parent, int first, int last);
    void onRowsMoved(ModeFlags mode, const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void onRowsAboutToBeRemoved(ModeFlags mode, const QModelIndex &parent, int first, int last);
    void onDataChanged(ModeFlags mode, const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    struct Gadget {
        int type = QMetaType::UnknownType;
        const QMetaObject *metaObject = nullptr;
        void *ptr = nullptr;
        bool update(const QJsonObject &attr, const QString &key = QString()) const;
        void destroy();
        inline operator bool() const { return ptr != nullptr; }
        inline operator QVariant() const { return QVariant(type, ptr); }
    };

    QHash<QPersistentModelIndex, QPointer<QObject>> m_objects;
    QHash<QPersistentModelIndex, Gadget> m_gadgets;
    QString           m_typeRole;
    QString           m_valueRole;
    int               i_typeRole = -1;
    int               i_valueRole = -1;
    int                 mrlrole = -1;
    InstantiationMode m_mode = Tree;

    void regenerate();
    void build(const QModelIndex &parent, int first = 0, int last = -1);
    void remove(const QModelIndex &parent, int first = 0, int last = -1);
    void clear();
    Gadget buildGadget(const QByteArray &type, const QJsonObject &obj) const;
    bool writeValue(const QVariant &value, const QMetaProperty &property, void *gadget);
    bool updateValue(const QModelIndex &idx) const;
//    void bind(QAbstractItemModel *model);
//    void unbind(QAbstractItemModel *model);
    int findRole(QString role, const QAbstractItemModel *model);


    // SignalFilter interface

    int m_count;
};





#endif // WEAVER_H
