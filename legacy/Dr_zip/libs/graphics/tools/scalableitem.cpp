#include "scalableitem.h"
#include <QPainter>
#include <QQuickWindow>
namespace DR{
ScalableItem::ScalableItem(QQuickItem *parent) :
    QQuickItem(parent),
    m_lazy(false)
{

}

void ScalableItem::checkSize()
{
    if (!m_lazy) {
        adjust();
        return;
    }
    if (lazyTimer.isActive())
        return;
    lazyTimer.start(167);
}

void ScalableItem::adjust()
{
    qreal s = scale();
    QQuickItem *p = parentItem();
    while (p) {
        s *= p->scale();
        p = p->parentItem();
    }
    QPointF ul = mapToScene(QPointF(0, 0));
    QPointF br = mapToScene(QPointF(width(), height()));
    QRectF rect(ul, br);
    QRectF scene(0, 0, window()->width(), window()->height());
    QRectF visRect = scene.intersected(rect);
    setEffectiveScale(s);
    setVisibleRect(QRectF(mapFromScene(visRect.topLeft()), mapFromScene(visRect.bottomRight())));
    setSceneRect(rect);
}

void ScalableItem::componentComplete()
{
    QQuickItem::componentComplete();
    connect(this->window(), SIGNAL(afterAnimating()), this, SLOT(checkSize()));
    lazyTimer.setSingleShot(true);
    connect(&lazyTimer, SIGNAL(timeout()), this, SLOT(adjust()));
}

qreal ScalableItem::effectiveScale() const
{
    return m_effectiveScale;
}

QRectF ScalableItem::sceneRect() const
{
    return m_sceneRect;
}

QRectF ScalableItem::visibleRect() const
{
    return m_visibleRect;
}

bool ScalableItem::lazy() const
{
    return m_lazy;
}

void ScalableItem::setLazy(bool arg)
{
    if (m_lazy == arg)
        return;

    m_lazy = arg;
    emit lazyChanged(arg);
}

void ScalableItem::setSceneRect(QRectF arg)
{
    if (m_sceneRect == arg)
        return;
    m_sceneRect = arg;
    emit sceneRectChanged();
}

void ScalableItem::setVisibleRect(QRectF arg)
{
    if (m_visibleRect == arg)
        return;
    m_visibleRect = arg;
    emit visibleRectChanged();
}

void ScalableItem::setEffectiveScale(qreal arg)
{
    if (m_effectiveScale == arg)
        return;
    m_effectiveScale = arg;
    emit effectiveScaleChanged();
}
}
