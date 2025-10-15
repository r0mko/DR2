#include "dynagraph_plugin.h"

#include <qqml.h>
#include "dgcursor.h"
#include "dgitem.h"
#include "dgscene.h"
#include "dgshapeitem.h"

void DynagraphPlugin::registerTypes(const char *uri)
{
    // @uri DataReactor.Graphics
    qmlRegisterType<DGShape>(uri, 1, 0, "Shape");
    qmlRegisterType<DGPen>(uri, 1, 0, "Pen");
    qmlRegisterType<DGBrush>(uri, 1, 0, "Brush");
    qmlRegisterType<DGScene>(uri, 1, 0, "Scene");
    qmlRegisterType<DGSceneItem>(uri, 1, 0, "SceneItem");
    qmlRegisterType<DGShapePainter>(uri, 1, 0, "ShapePainter");
    qmlRegisterType<DGShapeItem>(uri, 1, 0, "ShapeItem");
    qmlRegisterType<DGItem>(uri, 1, 0, "Item");
    qmlRegisterType<DGCursor>(uri, 1, 0, "CursorShape");
}


