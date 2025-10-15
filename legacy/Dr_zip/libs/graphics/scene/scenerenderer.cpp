#include "scenerenderer.h"
#include "scene.h"
#include "qgraphicsscene.h"

#include <QPainter>
//#include <QDateTime>
//#include "../cartograph/defs.h"

QRectF SceneRenderer::sceneRect() const
{
    return m_sceneRect;
}

void SceneRenderer::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
    if (m_scene) {
//        QRectF target = QRectF(0, 0, width(), height());
//        cache = QPixmap(target.size().toSize());
//        cache = QImage(target.size().toSize(), QImage::Format_ARGB32_Premultiplied);
//        renderCache();
        targetRect = newGeometry;
//        updateItemList();
//        if (m_cacheEnabled) {
//            cache_working = QPixmap(targetRect.size().toSize());
////            cache_working = QImage(targetRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
//            renderCache();

//        }
//        qDebug() << "geometry changed" << newGeometry << oldGeometry;
        update();
    }
}

void SceneRenderer::renderCache(const QRect &rect)
{
    if (rect.isEmpty())
        cache_working.fill(Qt::transparent);

//    cache_working.fill();
    QPainter painter;
    painter.begin(&cache_working);
    if (!rect.isEmpty()) {
        painter.eraseRect(rect);
    }
    painter.setRenderHint(QPainter::Antialiasing);
    //    qDebug() << "target size" << target.size() << cache;
//    qDebug() << __func__ << "target rect" << targetRect;
    m_scene->m_scene->renderItems(&painter, m_sceneRect, targetRect, itemVector);
    painter.end();
    qSwap(cache_render, cache_working);
    update();
}


bool SceneRenderer::cacheEnabled() const
{
    return m_cacheEnabled;
}

void SceneRenderer::setCacheEnabled(bool cacheEnabled)
{
    if (m_cacheEnabled == cacheEnabled)
        return;

    m_cacheEnabled = cacheEnabled;
    emit cacheEnabledChanged(cacheEnabled);
}


void SceneRenderer::checkDirty(const QList<QRectF> &region)
{
    updateItemList();
    if (!m_cacheEnabled) {
        update();
        return;
    }
    QRectF dirtyRect;
    for (const QRectF & r : region) {
        if (m_sceneRect.intersects(r)) {
            dirtyRect = dirtyRect.united(r);
        }
    }
    QRect dr2 = mapRectFromItem(m_scene, dirtyRect).adjusted(-1, -1, 1, 1).toRect();;

    if (!dr2.isEmpty())
        update(dr2);
    else {
        update();
    }
    lastDirtyRect = dr2;


}

SceneRenderer::SceneRenderer(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    setTransformOrigin(QQuickItem::TopLeft);
//    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setRenderTarget(QQuickPaintedItem::Image);
    setPerformanceHint(QQuickPaintedItem::FastFBOResizing);

}


void SceneRenderer::paint(QPainter *painter)
{
    if(!isVisible())
        return;
    if (!m_scene->autoUpdate())
        return;
    if (m_cacheEnabled)
        painter->drawPixmap(QPointF(), cache_render);
    else
        m_scene->m_scene->renderItems(painter, m_sceneRect, targetRect, itemVector);
    //    m_scene->m_scene->render(painter, targetRect, m_sceneRect, );
}


void SceneRenderer::setScene(Scene *scene)
{
    if(m_scene == scene)
        return;
    if (m_scene)
        m_scene->disconnect(0, this, 0);
    m_scene = scene;
        connect(m_scene->m_scene, &MyGraphicsScene::changed, this, &SceneRenderer::checkDirty);
//    connect(m_scene, SIGNAL(changed()), this, SLOT(update()));
//    connect(m_scene, SIGNAL(changed()), this, SLOT(renderCache()));
//    connect(m_scene, SIGNAL(changed()), this, SLOT(update()), Qt::QueuedConnection);

//    connect(m)
    emit sceneChanged(m_scene);
    update();
}

void SceneRenderer::updateItemList()
{
    QList<QGraphicsItem *> itemList = m_scene->m_scene->items(m_sceneRect, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder);
    itemVector = itemList.toVector();
}

void SceneRenderer::setSceneRect(QRectF sceneRect)
{
    if (m_sceneRect == sceneRect)
        return;
    m_sceneRect = sceneRect;
    updateItemList();
    emit sceneRectChanged(sceneRect);
//    update();
}

Scene *SceneRenderer::scene() const
{
    return m_scene;
}
