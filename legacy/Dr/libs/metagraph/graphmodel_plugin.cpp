#include "graphmodel_plugin.h"
#include "graphmodel.h"
#include "graphmodel.h"
#include "nodechildrenmodel.h"
#include "qqml.h"
#include "node.h"
#include "edge.h"
#include "type.h"
#include "methodbuilder.h"
#include "propertybuilder.h"
#include "enumbuilder.h"
#include "collectionobject.h"
#include "dynamicobjectextension.h"
#include "dummy.h"
#include "type.h"
#include "blob.h"
#include "collection2.h"
#include "datanode.h"
#include <QQmlContext>
#include <QQmlEngine>
#include <graphindex.h>
#include <qqml.h>
#include "modelagent.h"
#include "subtreemodel.h"
#include "modelwatcher.h"
#include "typevalidator.h"
#include "sqlbridge.h"
#include "modelpictureimageprovider.h"
#include "collection.h"
#include "element.h"
#include "rootnode.h"
#include "graphmodelhelper.h"
#include "weaver.h"
#include "pointnode.h"
#include "edgemodel.h"
template<class... Args> QVector<int> qRegisterMetaTypes() { return { qRegisterMetaType<Args>()... }; }
const QVector<HandlerData> TypeFactory::handlers_imp = registerMetaHandlers<Dummy, ElementAgent>();

template <typename T, typename H = typename HandleType<T>::type, typename F = typename HandleType<T>::factoryType>
struct handle;

template <typename T, typename F>
struct handle<T, void, F>
{
    static QPair<int, int> registerNode()
    {
//        debugPrint << "Registering" << QMetaType::typeName(qMetaTypeId<T>()) << "factory type" << QMetaType::typeName(qMetaTypeId<F>());
        return qMakePair(qMetaTypeId<T>(), 0);
    }
};

template <typename T, typename H, typename F>
struct handle
{
    static QPair<int, int> registerNode()
    {
//        debugPrint << "Registering" << QMetaType::typeName(qMetaTypeId<T>()) << "with handler" << QMetaType::typeName(qMetaTypeId<H>()) << "factory type" << QMetaType::typeName(qMetaTypeId<F>());
        QMetaType::registerConverter<H, NodeHandle>();
//        QMetaType::registerConverter<H, QModelIndex>();
        return qMakePair(qMetaTypeId<T>(), qMetaTypeId<H>());
    }
};

template <typename T>
QPair<int, int> registerHandle() { return handle<T>::registerNode(); }

template<class... T> QVector<QPair<int, int>> registerNodes() { return { registerHandle<T>()...}; }

void GraphModelPlugin::registerTypes(const char *uri)
{
    // @uri Dr.MetaGraph

    TypeFactory::initReferences();
    QMetaType::registerConverter<MethodBuilder::MethodType, int>();

    auto reg = registerNodes<
            Node,
            MetaNode,
            Blob,
            Type,
            DataNode,
            Collection,
            Collection2,
            Instance,
            Element,
            SqlBridgeNode,
            RootNode,
            PointNode
            >();
    for (QPair<int, int> p : reg) {
        if (p.second) {
            GraphModel::node2handle.insert(p.first, p.second);
        } else {
            GraphModel::node2handle.insert(p.first, qMetaTypeId<NodeHandle>());
        }
    }
    qRegisterMetaTypes<
            GraphIndex,
            EdgeHandle,
            QPainterPath,
            QList<int>
            >();
    qRegisterMetaTypeStreamOperators<QPainterPath>();
//    QMetaType::registerStreamOperators("QPainterPath", );

    qRegisterMetaType<MethodBuilder::MethodType>("MethodType");
    qRegisterMetaType<MethodBuilder::Access>("Access");

    qmlRegisterType<GraphModel>(uri, 1, 0, "GraphModel");
    qmlRegisterType<TypeValidator>(uri, 1, 0, "TypeValidator");
    qmlRegisterType<TypeFactory>(uri, 1, 0, "TypeBuilder");
    qmlRegisterType<Method>(uri, 1, 0, "Method");
    qmlRegisterType<MethodBuilder>(uri, 1, 0, "MethodBuilder");
    qmlRegisterType<Property>(uri, 1, 0, "Property");
    qmlRegisterType<PropertyBuilder>(uri, 1, 0, "PropertyBuilder");
    qmlRegisterType<Enum>(uri, 1, 0, "EnumBuilder");
    qmlRegisterType<EnumBuilder>(uri, 1, 0, "EnumBuilder");
    qmlRegisterType<Weaver>(uri, 1, 0, "Weaver");
    qmlRegisterType<EdgeModel>(uri, 1, 0, "EdgeModel");

    qmlRegisterType<ModelWatcher>(uri, 1, 0, "ModelWatcher");
    qmlRegisterType<SubtreeModel>(uri, 1, 0, "SubtreeModel");
    qmlRegisterType<NodeChildrenModel>(uri, 1, 0, "NodeChildrenModel");

    qmlRegisterType<BlobFactory>(uri, 1, 0, "BlobFactory");
    qmlRegisterType<NodeFactory>(uri, 1, 0, "NodeFactory");
    qmlRegisterType<MetaNodeFactory>(uri, 1, 0, "MetaNodeFactory");
    qmlRegisterType<DataNodeFactory>(uri, 1, 0, "DataNodeFactory");
    qmlRegisterType<CollectionFactory>(uri, 1, 0, "CollectionFactory");
    qmlRegisterType<Collection2Factory>(uri, 1, 0, "Collection2Factory");
    qmlRegisterType<InstanceFactory>(uri, 1, 0, "InstanceFactory");
    qmlRegisterType<ElementFactory>(uri, 1, 0, "ElementFactory");
    qmlRegisterType<SqlBridgeNodeFactory>(uri, 1, 0, "SqlBridgeNodeFactory");
    qmlRegisterType<RootNodeFactory>(uri, 1, 0, "RootNodeFactory");
    qmlRegisterType<PointNodeFactory>(uri, 1, 0, "PointNodeFactory");

    qmlRegisterUncreatableType<ModelAgent>(uri, 1, 0, "ModelObject", "ModelObject is not creatable");
    qmlRegisterUncreatableType<ElementAgent>(uri, 1, 0, "ElementAgent", "ModelObject is not creatable");
    qmlRegisterUncreatableType<NodeHandle>(uri, 1, 0, "NodeHandle", "NodeHandle is not creatable");
    qmlRegisterUncreatableType<DataNodeHandle>(uri, 1, 0, "DataNodeHandle", "DataNodeHandle is not creatable");
    qmlRegisterUncreatableType<MetaNodeHandle>(uri, 1, 0, "MetaNodeHandle", "DataNodeHandle is not creatable");
    qmlRegisterUncreatableType<CollectionHandle>(uri, 1, 0, "CollectionHandle", "CollectionHandle is not creatable");
    qmlRegisterUncreatableType<Collection2Handle>(uri, 1, 0, "Collection2Handle", "Collection2Handle is not creatable");
    qmlRegisterUncreatableType<ElementHandle>(uri, 1, 0, "ElementHandle", "ElementHandle is not creatable");
    qmlRegisterUncreatableType<InstanceHandle>(uri, 1, 0, "InstanceHandle", "InstanceHandle is not creatable");
    qmlRegisterUncreatableType<RootNodeHandle>(uri, 1, 0, "RootNodeHandle", "RootNodeHandle is not creatable");
    qmlRegisterUncreatableType<PointNodeHandle>(uri, 1, 0, "PointNodeHandle", "PointNodeHandle is not creatable");

//    qmlRegisterUncreatableType<ElementAgent>(uri, 1, 0, "CollectionObject", "CollectionObject is not creatable");
    qmlRegisterUncreatableType<SqlBridgeNodeHandle>(uri, 1, 0, "SqlBridgeNodeHandle", "SqlBridgeNodeHandle is not creatable");
}

void GraphModelPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    engine->addImageProvider("picture", new ModelPictureImageProvider);
    engine->rootContext()->setContextProperty("Graph", new GraphModelHelper(engine));
}
