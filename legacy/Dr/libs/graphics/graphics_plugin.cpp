#include "graphics_plugin.h"

#include <QGraphicsScene>
#include <abstractguideitem.h>
#include <eventwrappers.h>
#include <guidepathsmoother.h>
#include <guidepathstroke.h>
#include <guidepathtext.h>
#include <pointitem.h>
#include <polygonsmoother.h>
#include <qqml.h>
#include "shape.h"
#include "aawindow.h"
#include "pen.h"
#include "brush.h"
#include "cursor.h"
#include "mousearea.h"
#include "scenerenderer.h"
#include "scene.h"
#include "stencil.h"
#include "distort.h"
#include "svgpath.h"
#include "line.h"
#include "polygon.h"
#include "customtransform.h"
#include "dummysceneitem.h"
#include "polygonsceneitem.h"
#include "rectsceneitem.h"
#include "shapesceneitem.h"
#include "sceneitemstroker.h"
#include "pathsceneitem.h"
#include "graphribsceneitem.h"
#include "graphribslidersceneitem.h"
//#include "offsetpolylinesceneitem.h"
#include "vectorpath.h"
#include "smoothline.h"
#include "groove.h"
#include "smoothwire.h"
#include "wiringelement.h"
#include "wiretransition.h"
//#include "graphroaditem.h"
#include "wireterminator.h"
#include "visualinstance.h"
#include "dummyitem.h"
#include "scenemodel.h"
#include "gpen.h"
#include "gbrush.h"
#include "shapeinstance.h"
#include "polygonitem.h"
#include "pathitem.h"
#include "painterpath.h"
#include "creatist.h"
#include "drawarea.h"
QHash<int, int> item2handle = QHash<int, int>();

//template <class T>
//int registerItem()
//{
//    qDebug() << "Registering item" << QMetaType::typeName(qMetaTypeId<T>()) << "Converter function:" << QMetaType::registerConverter<T, VisualInstanceHandle>();
//    return qRegisterMetaType<T>();
//}
//template<class... T> QVector<int> registerItems() {
//    return { registerItem<T>()...};
//}

template<class... Args> QVector<int> qRegisterMetaTypes() { return { qRegisterMetaType<Args>()... }; }

template <typename T, typename H = typename HandleType<T>::type>
struct ItemHandleImpl;

template <typename T, typename H>
struct ItemHandleImpl
{
    static int registerItem(const char *uri)
    {
//        debugPrint << "Registering" << QMetaType::typeName(qMetaTypeId<T>()) << "with handler" << QMetaType::typeName(qMetaTypeId<H>()) << "factory type" << QMetaType::typeName(qMetaTypeId<F>());
        QMetaType::registerConverter<H, VisualInstanceHandle>();

//        QMetaType::registerConverter<H, QModelIndex>();
        QMetaType::registerDebugStreamOperator<H>();
        item2handle.insert(qRegisterMetaType<T*>(), qRegisterMetaType<H>());
        QByteArray itemType(QMetaType::typeName(qMetaTypeId<T*>()));
        itemType.chop(1);

        // @uri Dr.Graphics
        return qmlRegisterUncreatableType<H>(uri, 1, 0, itemType.constData(), QString("%1 is not a QML type").arg(itemType.constData()));
    }
};
template <typename T>
struct ItemHandleImpl<T, void>
{
    static void registerItem()
    {
        Q_ASSERT_X(false, QString("register type %1").arg(QMetaType::typeName(qMetaTypeId<T*>())).toLocal8Bit().constData(), "Handle type not provided");
//        debugPrint << "Registering" << QMetaType::typeName(qMetaTypeId<T>()) << "factory type" << QMetaType::typeName(qMetaTypeId<F>());
    }
};


template <typename T>
int registerItem(const char *uri) { return ItemHandleImpl<T>::registerItem(uri); }

template <typename T>
void registerGadget(const char *uri)
{
    qmlRegisterUncreatableType<T>(uri, 1, 0, QMetaType::typeName(qMetaTypeId<T>()), QString("%1 is not valid QML type").arg(QMetaType::typeName(qMetaTypeId<T>())));
}

template<class... T> void registerItems(const char *uri) { int _[] { 0, (registerItem<T>(uri), 0)... }; (void)_; }
template<class... T> void registerGadgets(const char *uri) { int _[] { 0, (registerGadget<T>(uri), 0)... }; (void)_; }


void DynagraphPlugin::registerTypes(const char *uri)
{
//    if (!qApp->inherits("QApplication")) {
//        qDebug() << uri <<  "requires QApplication. Please add QT += widgets to your .pro file.";
//        return;
//    }

    // @uri Dr.Graphics
    qmlRegisterType<AAWindow>(uri, 1, 0, "AAWindow");
    qmlRegisterRevision<QQuickWindow, 1>(uri, 1, 0);
    qmlRegisterRevision<QQuickWindow, 2>(uri, 1, 0);

    qmlRegisterUncreatableType<QGraphicsScene>(uri, 1, 0, "QGraphicsScene", QString("QGraphicsScene is an abstract class"));
//    qmlRegisterUncreatableType<QGraphicsScene>(uri, 1, 0, "QGraphicsScene", QString("QGraphicsScene is an abstract class"));

    qmlRegisterUncreatableType<GuidePath>(uri, 1, 0, "GuidePath", QString("GuidePath is not a QML type"));
    qmlRegisterUncreatableType<GuidePathSmoother>(uri, 1, 0, "GuidePathSmoother", QString("GuidePathSmoother is not a QML type"));
//    qmlRegisterUncreatableType<PathRunner>(uri, 1, 0, "GuidePathSmoother", QString("GuidePathSmoother is not a QML type"));
//    qmlRegisterUncreatableType<VisualInstanceHandle>(uri, 1, 0, "VisualInstance", QString("VisualInstance is not a QML type"));
//    qmlRegisterUncreatableType<GuidePathStrokeHandle>(uri, 1, 0, "GuidePathStroke", QString("GuidePathStroke is not a QML type"));
//    qmlRegisterUncreatableType<PointItemHandle>(uri, 1, 0, "PointItem", QString("PointItem is not a QML type"));

    qmlRegisterType<Scene>(uri, 1, 0, "Scene");
    qmlRegisterType<SceneModel>(uri, 1, 0, "SceneModel");
    qmlRegisterType<DrawArea>(uri, 1, 0, "PaintBoard");
    qmlRegisterType<SceneRenderer>(uri, 1, 0, "SceneRenderer");
    qmlRegisterType<SceneItemStroker>(uri, 1, 0, "SceneItemStroker");
    qmlRegisterType<PolygonFactory>(uri, 1, 0, "PolygonFactory");
    qmlRegisterType<PolygonSmoother>(uri, 1, 0, "PolygonSmoother");
    qmlRegisterType<Distort>(uri, 1, 0, "Distort");
    qmlRegisterType<CustomTransform>(uri, 1, 0, "CustomTransform");
    qmlRegisterType<Creatist>(uri, 1, 0, "Creatist");

    registerItems<
            VisualInstance,
            PolygonItem,
            DummyItem,
            ShapeInstance,
            PathItem,
            AbstractGuideItem,
            GuidePathStroke,
            PointItem,
            GuidePathText
            >(uri);

    qRegisterMetaTypes<
            QPolygonF,
            QPainterPath,
            Qt::Orientation,
            Qt::MouseButtons,
            Qt::MouseButton
            >();

    registerGadgets<
            GPen,
            GBrush,
            PainterPath,
            VectorNode,
            VertexNode,
            HeadNode,
            TailNode,
            GuidePath,
            PathRunner,
            GraphicsSceneMouseEvent,
            GraphicsSceneHoverEvent,
            GraphicsSceneWheelEvent,
            PointRef
            >(uri);

    qRegisterMetaTypeStreamOperators<GuidePath>();

}







