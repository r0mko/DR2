#ifndef SCENEMODEL_H
#define SCENEMODEL_H

#include "visualinstance.h"
#include "scene.h"
#include "signalfilter.h"

#include <QJsonObject>
#include <QObject>
#include <QQmlParserStatus>
#include <QQmlEngine>
#include <QQmlPropertyMap>
//#include "path.h"

inline int findRole(QString role, const QAbstractItemModel *model)
{
    QHash<int, QByteArray> roles = model->roleNames();
    return roles.key(role.toLocal8Bit(), -1);
}

extern QHash<int, int> item2handle;
inline int getHandleType(int instanceType) { return item2handle.value(instanceType); }
inline int handleType(const QByteArray &type) { return getHandleType(QMetaType::type(type + QByteArray("*"))); }


class SceneModel : public QAbstractItemModel, public SignalFilter, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QAbstractItemModel* sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
    Q_PROPERTY(QModelIndex index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(QString typeRole READ typeRole WRITE setTypeRole NOTIFY typeRoleChanged)
    Q_PROPERTY(QString valueRole READ valueRole WRITE setValueRole NOTIFY valueRoleChanged)
    Q_PROPERTY(InstantiationMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QGraphicsScene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(QQmlPropertyMap* roleBindings READ roleBindings NOTIFY roleBindingsChanged)

public:
    explicit SceneModel(QObject *parent = 0);
    enum InstantiationMode {
        None =      0x1,
        Node =      0x2,
        Children =  0x4,
        Tree =      0x8
    };
    Q_ENUM(InstantiationMode)
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
    void setScene(QGraphicsScene* scene);
    QGraphicsScene* scene() const;

    QAbstractItemModel* sourceModel() const;

    void setSourceModel(QAbstractItemModel* sourceModel);
    QQmlPropertyMap* roleBindings() const;
    // QAbstractItemModel interface
    int row(QGraphicsItem* item) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    /*****/

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

    void sceneChanged();
    void sourceModelChanged(QAbstractItemModel* sourceModel);
    void roleBindingsChanged();


public slots:
    bool tryBind();
    void clearRoleBindings();
    //    QVariant addItem(const QString &type, QJsonObject params = QJsonObject());
    QVariant addItem(const QString &type, const QVariantMap &params = QVariantMap(), const QVariant &parent = QVariant());

protected:
    void onModelAboutToBeReset();
    void onModelReset();
    void onRowsInserted(ModeFlags mode, const QModelIndex &parent, int first, int last);
    void onRowsMoved(ModeFlags mode, const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void onRowsRemoved(ModeFlags mode, const QModelIndex &parent, int first, int last);
    void onDataChanged(ModeFlags mode, const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private slots:
    void addRoleBinding(const QString &key, const QVariant &value);

private:
    friend class SceneView;
    struct Instance {
        int                 type = QMetaType::UnknownType;
        const QMetaObject * metaObject = nullptr;
        void *              ptr = nullptr;
        Instance *          m_parentItem;
        QList<Instance*>    m_childItems;
        bool update(const QVariantMap &attr) const;
        void destroy();
        inline operator bool() const { return ptr != nullptr; }
        inline operator QVariant() const { return QVariant(type, ptr); }
        inline VisualInstanceHandle* handle() const { return static_cast<VisualInstanceHandle*>(ptr); }

        /** Item for model **/
        Instance( Instance *parent = nullptr ) : m_parentItem(parent) {}
        ~Instance(){ qDeleteAll(m_childItems); }

        void appendChild(Instance *child) { m_childItems.append(child); }
        Instance *child(int row) { return m_childItems.value(row); }
        int childCount() const { return m_childItems.size(); }
        int columnCount() const { return 1; }
        int row() const {
            if (m_parentItem != nullptr)
                return m_parentItem->m_childItems.indexOf(const_cast<Instance*>(this));
            return 0;
        }
        Instance *parentItem() { return m_parentItem; }
    };

    QHash<VisualInstance*, Instance> item2instance;
    QHash<QPersistentModelIndex, Instance> idx2instance;
    QString             m_typeRole;
    QString             m_valueRole;
    int                 i_typeRole = -1;
    int                 i_valueRole = -1;
    QHash<int, QByteArray> i_boundRoles;
    InstantiationMode   m_mode = Tree;
    QGraphicsScene*     m_scene = nullptr;
    QAbstractItemModel* m_sourceModel;
    QQmlPropertyMap*    m_roleBindings { new QQmlPropertyMap(this) };

    void regenerate();
    void build(const QModelIndex &parent, int first = 0, int last = -1);
    void remove(const QModelIndex &parent, int first = 0, int last = -1);
    void clear();
    Instance createInstance(const QByteArray &type, const QVariantMap &obj) const;
    Instance createInstanceHelper(int type) const;
    bool writeValue(const QVariant &value, const QMetaProperty &property, void *gadget);
    bool updateValue(const QModelIndex &idx) const;
    //    void bind(QAbstractItemModel *model);
    //    void unbind(QAbstractItemModel *model);



    /** model interface **/
    QList<QGraphicsItem *> m_rootChildren;

};

bool isParentOf(const QModelIndex &child, const QModelIndex &parent);

Q_DECLARE_METATYPE(QQmlEngine*)
#endif // SCENEMODEL_H
