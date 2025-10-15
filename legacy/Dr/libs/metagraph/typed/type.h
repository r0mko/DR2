#ifndef TYPENODE_H
#define TYPENODE_H

#include "blob.h"

class Type : public Blob
{
    QString m_className;
//    QByteArray m_rawData;
    int m_metaTypeId = -1;

    METAGRAPH_NODE(Type)
public:
    using Blob::Blob;
    void setClassName(const QString &value);
//    void setRawData(const QByteArray &value);
    void setMetaTypeId(int value);
    QString className() const;
    QByteArray rawData() const;
    int metaTypeId() const;

    virtual int typeId() const;
    QVariantList getProperties() const;
private:
};

class TypeHandle : public BlobHandle
{
    Q_GADGET
    Q_PROPERTY(QString className READ className)
    Q_PROPERTY(int metaTypeId READ metaTypeId)
    Q_PROPERTY(bool isRegistered READ isRegistered)
    Q_PROPERTY(QVariantList properties READ properties)
    Q_PROPERTY(int rawDataSize READ rawDataSize)


public:
    static int id_metaTypeId;
    static int id_isRegistered;
    NODE_FN(Type)
    using BlobHandle::BlobHandle;
    inline QString className() const { idxCheck(QString()); return node()->className(); }
    inline int metaTypeId() const { idxCheck(QMetaType::UnknownType); return node()->metaTypeId(); }
    inline QVariantList properties() const { idxCheck(QVariantList()); return node()->getProperties(); }
    bool isRegistered() const;
    void notifyCollections( bool registered );
    int rawDataSize() const;
};

#include "private/qmetaobjectbuilder_p.h"
#include <QQmlListProperty>
#include "node.h"
#include "type.h"
#include <QVariantMap>

class PropertyBuilder;
class MethodBuilder;
class EnumBuilder;
class Property;
class Method;
class Enum;
typedef void (*MetaCallFn)(QObject *, QMetaObject::Call, int, void **);

struct HandlerData
{
    MetaCallFn metaCall = nullptr;
    QMetaType::Deleter del = nullptr;
    QMetaType::Creator create = nullptr;
    QMetaType::Destructor dtor = nullptr;
    QMetaType::Constructor ctor = nullptr;
    int size = 0;
    const QMetaObject *staticMetaObject = nullptr;
};
class Type;

class TypeFactory : public BlobFactory
{
    Q_OBJECT
    Q_PROPERTY(QStringList savedTypeNames READ savedTypeNames NOTIFY savedTypesChanged)
    Q_PROPERTY(QStringList baseTypeNames READ baseTypeNames CONSTANT)
    Q_PROPERTY(QStringList allTypeNames READ allTypeNames CONSTANT)
    Q_PROPERTY(QVariantList typesInfo READ typesInfo NOTIFY typesInfoChanged)

    Q_PROPERTY(QString className READ className WRITE setClassName NOTIFY classNameChanged)
    Q_PROPERTY(QVariantMap classInfo READ classInfo WRITE setClassInfo NOTIFY classInfoChanged)
    Q_PROPERTY(QQmlListProperty<Property> properties READ qmlProperties NOTIFY propertiesChanged)
    Q_PROPERTY(QQmlListProperty<Method> constructors READ qmlConstructors NOTIFY constructorsChanged)
    Q_PROPERTY(QQmlListProperty<Method> methods READ qmlMethods NOTIFY methodsChanged)
    Q_PROPERTY(QQmlListProperty<Enum> enums READ qmlEnums NOTIFY enumsChanged)

    Q_PROPERTY(int currentTypeId READ currentTypeId WRITE setCurrentTypeId NOTIFY currentTypeIdChanged)
    Q_PROPERTY(QStringList handlers READ handlers CONSTANT)
    Q_PROPERTY(int handlerId READ handlerId WRITE setHandlerId NOTIFY handlerIdChanged)
    Q_PROPERTY(QString baseClassName READ baseClassName WRITE setBaseClassName NOTIFY baseClassNameChanged)

    Q_FLAGS(AddMembers)

public:
    friend class Property;
    friend class Method;
    friend class EnumBuilder;
    enum AddMember
    {
        ClassName               = 0x00000001,
        SuperClass              = 0x00000002,
        Methods                 = 0x00000004,
        Signals                 = 0x00000008,
        Slots                   = 0x00000010,
        Constructors            = 0x00000020,
        Properties              = 0x00000040,
        Enumerators             = 0x00000080,
        ClassInfos              = 0x00000100,
        RelatedMetaObjects      = 0x00000200,
        StaticMetacall          = 0x00000400,
        PublicMethods           = 0x00000800,
        ProtectedMethods        = 0x00001000,
        PrivateMethods          = 0x00002000,
        AllMembers              = 0x7FFFFFFF,
        AllPrimaryMembers       = 0x7FFFFBFC
    };
    Q_DECLARE_FLAGS(AddMembers, AddMember)

    friend class PropertyBuilder;
    friend class MethodBuilder;
    friend class Type;
    friend class TypeValidator;

    TypeFactory(QObject *parent = 0);
    ~TypeFactory();

    int handlerId() const;
    QStringList savedTypeNames() const;
    QStringList baseTypeNames() const;
    QStringList allTypeNames() const;
    QVariantList typesInfo(QMetaType::TypeFlags filter = 0) const;
    QStringList handlers() const;
    QString className() const;
    QVariantMap classInfo() const;
    QString baseClassName() const;
    int currentTypeId() const;

    void setCurrentTypeId(int arg);
    void setHandlerId(int arg);
    void setClassName(QString arg);
    void setClassInfo(QVariantMap arg);
    void setBaseClassName(QString arg);
    static void importMetaObject(const QByteArray className, const QMetaObject *metaObject);
    static void initReferences();

    void registerCurrentType();
    static QHash<QString, const QMetaObject*> dynamicTypes;
    static QMap<QByteArray, const QMetaObject *> references;

public slots:
    TypeHandle create(const QString &nodeId = QString());
    void setPrototype(QObject *obj, TypeFactory::AddMembers addMembers);
    void loadMetaObject(const QMetaObject *mo);

    QStringList qObjectTypes() const;

    void loadTypeDef(TypeHandle handle);
    MethodBuilder *addSlot(QString signature);
    MethodBuilder *addSlot(QString name, QStringList argtypes);
    MethodBuilder *addSignal(QString signature);
    MethodBuilder *addSignal(QString name, QStringList argtypes);
    MethodBuilder *addConstructor(QString signature);
    MethodBuilder *addConstructor(QStringList argtypes);
    MethodBuilder *addMethod(QString returnType, QString signature);
    MethodBuilder *addMethod(QString returnType, QString name, QStringList argtypes);
    PropertyBuilder *addProperty(QString name, QString type, bool notifyable = true);
    EnumBuilder *addEnum(QString name);

    bool isValidType(QString typeName) const;

    void removeSlot(Method *m);
    void removeSignal(Method *m);
    void removeConstructor(Method *m);
    void removeMethod(Method *m);
    void removeProperty(Property *m);
    void addClassInfo(QString name, QString value);
    void unregisterType(TypeHandle handle);
    void registerType(TypeHandle handle);
    void loadType(QString className);
    void clear();

    void saveToModel(const QModelIndex &parent);
    void save( TypeHandle handle );
    void loadTypesFromModel(const QModelIndex &parent);
    // NodeFactory interface
public:
    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;

signals:
    void classNameChanged();
    void constructorsChanged();
    void propertiesChanged();
    void methodsChanged();
    void enumsChanged();
    void currentTypeIdChanged();
    void classInfoChanged();
    void savedTypesChanged();
    void typesInfoChanged();
    void handlerIdChanged();
    void baseClassNameChanged(QString arg);

private:
    static const QVector<HandlerData> handlers_imp;


    QList<Method*> m_methods;
    QList<Method*> m_constructors;
    QList<Property*> m_properties;
    QList<Enum*> m_enums;
    QVariantMap m_classInfos;
    QString m_className;
    int m_currentTypeId = QMetaType::UnknownType;
    mutable QScopedPointer<QMetaObjectBuilder> d;
    int m_handlerId = 0;
//    GraphModel* m_model = nullptr;
    QString m_baseClassName;

    QMetaObjectBuilder *d_chk() const;
    void compileType(); // loads to builder from model
    void initModel();

    QQmlListProperty<Enum> qmlEnums();
    QQmlListProperty<Method> qmlMethods();
    QQmlListProperty<Property> qmlProperties();
    QQmlListProperty<Method> qmlConstructors();

    static Enum *qmlEnumsAt(QQmlListProperty<Enum> *list, int index);
    static Method *qmlMethodsAt(QQmlListProperty<Method> *list, int index);
    static Property *qmlPropertiesAt(QQmlListProperty<Property> *list, int index);
    static Method *qmlConstructorsAt(QQmlListProperty<Method> *list, int index);

    static int qmlEnumsCount(QQmlListProperty<Enum> *list);
    static int qmlMethodsCount(QQmlListProperty<Method> *list);
    static int qmlPropertiesCount(QQmlListProperty<Property> *list);
    static int qmlConstructorsCount(QQmlListProperty<Method> *list);


};

Q_DECLARE_OPERATORS_FOR_FLAGS(TypeFactory::AddMembers)
template<class T>
HandlerData registerMetaHandler()
{
    HandlerData ret;
    ret.size = sizeof(T);
    ret.staticMetaObject = &T::staticMetaObject;
    //    ret.del = QtMetaTypePrivate::QMetaTypeFunctionHelper<T*>::Delete;
    //    ret.create = QtMetaTypePrivate::QMetaTypeFunctionHelper<T*>::Create;
    ret.dtor = QtMetaTypePrivate::QMetaTypeFunctionHelper<T*>::Destruct;
    ret.ctor = QtMetaTypePrivate::QMetaTypeFunctionHelper<T*>::Construct;
    return ret;
}

template<class... T>
QVector<HandlerData> registerMetaHandlers() { return { registerMetaHandler<T>()... }; }


DECLARE_NODE(Type)


#endif // TYPENODE_H
