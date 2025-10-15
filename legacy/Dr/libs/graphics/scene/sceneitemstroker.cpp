#include "sceneitemstroker.h"
#include "qdebug.h"
#include <QPainterPath>
#include <QPainter>
#include <QGraphicsScene>
#include <visualinstance.h>
#include "sceneitem.h"
#include "scene.h"

SceneItemStroker::SceneItemStroker(SceneItem *parent) :
    m_scene(nullptr)
{
    setTransformOrigin(QQuickItem::TopLeft);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setPerformanceHint(QQuickPaintedItem::FastFBOResizing);
}

void SceneItemStroker::paint(QPainter *painter)
{
//    painter->setWorldMatrixEnabled(true);
    painter->setWorldTransform(m_scene->viewTransform());
    QPen pen(QColor(255, 0, 255));
    pen.setCosmetic(true);
    pen.setWidthF(0.0);
//    qDebug() << "Painting selection" << m_scene->qGraphicsScene()->selectedItems();
    painter->strokePath(shape, pen);


//    QQmlListReference r(m_scene, "selectedItems");
//    for (int i = 0; i < r.count(); ++i) {
//        SceneItem *item = qobject_cast<SceneItem*>(r.at(i));
//        if (item && !(item->flags() & SceneItem::NoSelectionOutline)) {
//            QPainterPath outline = item->item()->shape();
//            QTransform tr;
//            tr.translate(item->position().x(), item->position().y());
//            tr.rotate(item->rotation());
//            tr.scale(item->scale(), item->scale());
//            painter->save();
//            painter->setTransform(tr, true);
//            painter->strokePath(outline, pen);
//            painter->restore();
//        }
//    }
//    for (SceneItem *item:m_items) {
//    for (SceneItem *item:m_items) {
//        QPainterPath outline = item->item()->shape();
//        QTransform tr;
//        tr.translate(item->position().x(), item->position().y());
//        tr.rotate(item->rotation());
//        tr.scale(item->scale(), item->scale());
//        painter->save();
//        painter->setTransform(tr, true);
//        painter->strokePath(outline, pen);
//        painter->restore();
//    }
}

Scene *SceneItemStroker::scene() const
{
    return m_scene;
}

void SceneItemStroker::setScene(Scene *arg)
{
    if (m_scene == arg)
        return;

    if (m_scene)
        m_scene->disconnect(0, this, 0);

    m_scene = arg;
    connect(m_scene, SIGNAL(viewTransformChanged(QTransform)), this, SLOT(update()));
    connect(m_scene, SIGNAL(changed()), this, SLOT(updateShape()));
    connect(m_scene, SIGNAL(selectionChanged()), this, SLOT(updateShape()));
    emit sceneChanged(arg);
}

void SceneItemStroker::updateShape()
{
    shape = QPainterPath();
    for (QGraphicsItem *item :  m_scene->qGraphicsScene()->selectedItems()) {
        if (item->data(VisualInstance::ExtraFlagsKey).toInt() & VisualInstanceHandle::NoSelectionOutline)
            continue;
        QPainterPath outline = item->shape();
        QTransform tr = item->transform();
        outline.translate(item->pos());
        tr.map(outline);
//        painter->save();
//        painter->translate(item->pos());
//        painter->setTransform(tr, true);
//        painter->strokePath(outline, pen);
//        painter->restore();
        shape.addPath(outline);
    }
    update();
}

