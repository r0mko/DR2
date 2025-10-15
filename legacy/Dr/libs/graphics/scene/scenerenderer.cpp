#include "scenerenderer.h"
#include "scene.h"
#include "qgraphicsscene.h"
//#include <QDateTime>
//#include "../cartograph/defs.h"

QRectF SceneRenderer::sceneRect() const
{
    return m_sceneRect;
}

void SceneRenderer::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
    if (m_scene)
        update();
}

SceneRenderer::SceneRenderer(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    setTransformOrigin(QQuickItem::TopLeft);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
//    setRenderTarget(QQuickPaintedItem::Image);
    setPerformanceHint(QQuickPaintedItem::FastFBOResizing);
}

void SceneRenderer::paint(QPainter *painter)
{
    if(!isVisible())
        return;
    if (!m_scene->autoUpdate())
        return;
    QRectF target = QRectF(0, 0, width(), height());
//    renderCount++;
//    qint64 st = now();
    m_scene->m_scene->render(painter, target, m_sceneRect);
//    qDebug() << "scene rendered in" << now() - st << "ms";
}


void SceneRenderer::setScene(Scene *scene)
{
    if(m_scene == scene)
        return;
    if (m_scene)
        m_scene->disconnect(0, this, 0);
    m_scene = scene;
//    connect(m_scene, SIGNAL(changed()), this, SLOT(update()));
    connect(m_scene, SIGNAL(changed()), this, SLOT(update()), Qt::QueuedConnection);
    emit sceneChanged(m_scene);
    update();
}

void SceneRenderer::setSceneRect(QRectF sceneRect)
{
    if (m_sceneRect == sceneRect)
        return;
    m_sceneRect = sceneRect;
    emit sceneRectChanged(sceneRect);
    update();
}

Scene *SceneRenderer::scene() const
{
    return m_scene;
}
