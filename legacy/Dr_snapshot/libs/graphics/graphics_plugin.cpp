#include "graphics_plugin.h"

#include <QGraphicsScene>
#include <abstractguideitem.h>
#include <eventwrappers.h>
#include <guidenetitem.h>
#include <guideoffsetter.h>
#include <guidesmoother.h>
#include <guidestroke.h>
#include <guidetext.h>
#include <pathobject.h>
#include <pointitem.h>
#include <polygonsmoother.h>
#include <qqml.h>
#include <textitem.h>
#include "aawindow.h"
#include "cursor.h"
#include "mousearea.h"
#include "scenerenderer.h"
#include "scene.h"
#include "distort.h"
#include "customtransform.h"
#include "sceneitemstroker.h"
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
#include "guidenet.h"
#include "textlayer.h"
#include "guideview.h"
#include "boundaryobject.h"
//#include "shape.h"
//#include "pen.h"
//#include "brush.h"
//#include "stencil.h"
//#include "svgpath.h"
//#include "line.h"
//#include "polygon.h"
//#include "dummysceneitem.h"
//#include "polygonsceneitem.h"
//#include "rectsceneitem.h"
//#include "shapesceneitem.h"
//#include "pathsceneitem.h"
//#include "graphribsceneitem.h"
//#include "graphribslidersceneitem.h"
//#include "offsetpolylinesceneitem.h"
//#include "vectorpath.h"
//#include "smoothline.h"
//#include "groove.h"
//#include "smoothwire.h"
//#include "wiringelement.h"
//#include "wiretransition.h"
//#include "graphroaditem.h"
//#include "wireterminator.h"

QHash<int, int> item2handle = QHash<int, int>();

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
    qmlRegisterUncreatableType<GPen>(uri, 1, 0, "GPen", QString("GPen is not a QML element"));
    qmlRegisterUncreatableType<GBrush>(uri, 1, 0, "GBrush", QString("GBrush is not a QML element"));
    qmlRegisterUncreatableType<PainterPath>(uri, 1, 0, "PainterPath", QString("PainterPath is not a QML element"));
    qmlRegisterUncreatableType<GuideNode>(uri, 1, 0, "GuideNode", QString("GuideNode is not a QML element"));
    qmlRegisterUncreatableType<VectorNode>(uri, 1, 0, "VectorNode", QString("VectorNode is not a QML element"));
    qmlRegisterUncreatableType<VertexNode>(uri, 1, 0, "VertexNode", QString("VertexNode is not a QML element"));
    qmlRegisterUncreatableType<HeadNode>(uri, 1, 0, "HeadNode", QString("HeadNode is not a QML element"));
    qmlRegisterUncreatableType<TailNode>(uri, 1, 0, "TailNode", QString("TailNode is not a QML element"));
    qmlRegisterUncreatableType<Guide>(uri, 1, 0, "Guide", QString("Guide is not a QML element"));
    qmlRegisterUncreatableType<Wavefront>(uri, 1, 0, "Wavefront", QString("Wavefront is not a QML element"));
    qmlRegisterUncreatableType<Runner>(uri, 1, 0, "PathRunner", QString("PathRunner is not a QML element"));
    qmlRegisterUncreatableType<GuideSmoother>(uri, 1, 0, "GuideSmoother", QString("GuideSmoother is not a QML element"));
    qmlRegisterUncreatableType<GraphicsSceneMouseEvent>(uri, 1, 0, "GraphicsSceneMouseEvent", QString("GraphicsSceneMouseEvent is not a QML element"));
    qmlRegisterUncreatableType<GraphicsSceneHoverEvent>(uri, 1, 0, "GraphicsSceneHoverEvent", QString("GraphicsSceneHoverEvent is not a QML element"));
    qmlRegisterUncreatableType<GraphicsSceneWheelEvent>(uri, 1, 0, "GraphicsSceneWheelEvent", QString("GraphicsSceneWheelEvent is not a QML element"));
    qmlRegisterUncreatableType<NetSegment>(uri, 1, 0, "NetSegment", QString("NetSegment is not a QML element"));
    qmlRegisterUncreatableType<NetVertex>(uri, 1, 0, "NetVertex", QString("NetVertex is not a QML element"));
    qmlRegisterUncreatableType<BoundaryVertex>(uri, 1, 0, "BoundaryVertex", QString("BoundaryVertex is not a QML element"));
    qmlRegisterUncreatableType<JointVertex>(uri, 1, 0, "JointVertex", QString("JointVertex is not a QML element"));
    qmlRegisterUncreatableType<JunctionVertex>(uri, 1, 0, "JunctionVertex", QString("JunctionVertex is not a QML element"));
    qmlRegisterUncreatableType<ConvergenceVertex>(uri, 1, 0, "ConvergenceVertex", QString("ConvergenceVertex is not a QML element"));
    qmlRegisterUncreatableType<StartVertex>(uri, 1, 0, "StartVertex", QString("StartVertex is not a QML element"));
    qmlRegisterUncreatableType<EndVertex>(uri, 1, 0, "EndVertex", QString("EndVertex is not a QML element"));
    qmlRegisterUncreatableType<PointRef>(uri, 1, 0, "PointRef", QString("PointRef is not a QML element"));
    qmlRegisterUncreatableType<FindPointReply>(uri, 1, 0, "FindPointReply", QString("FindPointReply is not a QML element"));
    qmlRegisterUncreatableType<Segment>(uri, 1, 0, "Segment", QString("Segment is not a QML element"));

    qmlRegisterUncreatableType<VisualInstanceHandle>(uri, 1, 0, "VisualInstance", QString("VisualInstance is an abstract class"));
    qmlRegisterUncreatableType<ShapeInstanceHandle>(uri, 1, 0, "ShapeInstance", QString("ShapeInstance is not a QML element"));
    qmlRegisterUncreatableType<PathItemHandle>(uri, 1, 0, "PathItem", QString("PathItem is not a QML element"));
    qmlRegisterUncreatableType<GuideStrokeHandle>(uri, 1, 0, "GuideStroke", QString("GuideStroke is not a QML element"));
    qmlRegisterUncreatableType<GuideTextHandle>(uri, 1, 0, "GuideText", QString("GuideText is not a QML element"));
    qmlRegisterUncreatableType<PolygonItemHandle>(uri, 1, 0, "PolygonItem", QString("PolygonItem is not a QML element"));
    qmlRegisterUncreatableType<TextItemHandle>(uri, 1, 0, "TextItem", QString("TextItem is not a QML element"));

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
    qmlRegisterType<GuideView>(uri, 1, 0, "GuideView");
    qmlRegisterType<GuideNet>(uri, 1, 0, "GuideNet");
    qmlRegisterType<PathObject>(uri, 1, 0, "PathObject");

    qmlRegisterType<BoundaryObject>(uri, 1, 0, "BoundaryObject");


    qmlRegisterType<TextLayer>(uri, 1, 0, "TextLayer");

    registerItems<
            VisualInstance,
            PolygonItem,
            DummyItem,
            ShapeInstance,
            PathItem,
            AbstractGuideItem,
            GuideStroke,
            PointItem,
            GuideText,
            TextItem
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
//            GuideNode,
            HeadNode,
            TailNode,
            Guide,
            Wavefront,
            Runner,
            GuideSmoother,
            GraphicsSceneMouseEvent,
            GraphicsSceneHoverEvent,
            GraphicsSceneWheelEvent,
            NetSegment,
            NetVertex,
            FindPointReply,
            Segment,
            BoundaryVertex,
            JointVertex,
            JunctionVertex,
            ConvergenceVertex,
            StartVertex,
            EndVertex,
            PointRef
            >(uri);

    qRegisterMetaTypeStreamOperators<Guide>();

}







