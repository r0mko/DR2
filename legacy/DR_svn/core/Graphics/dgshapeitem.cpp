#include "dgshapeitem.h"
#include <QPainter>
#include <QtMath>
#include <QRegion>

DGShapePainter::DGShapePainter(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    m_shape(0),
    m_scaleMode(NoScale),
    m_shapeScale(1)
{
    setClip(true);
    setAntialiasing(true);
//    setSmooth(false);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setPerformanceHint(QQuickPaintedItem::FastFBOResizing);
}

DGShape *DGShapePainter::shape() const
{
    return m_shape;
}

void DGShapePainter::paint(QPainter *painter)
{
    if (!isVisible())
        return;
    painter->save();
    if (m_scaleMode == ScaleToFit)
        painter->scale(width() / contentsSize().width(), height() / contentsSize().height());
    painter->scale(m_shapeScale, m_shapeScale);
    painter->translate(-m_viewRect.topLeft());

    render(painter);
    painter->restore();
}

void DGShapePainter::render(QPainter *painter)
{
    if (m_shape->hasFill())
        painter->fillPath(m_shape->path(), m_shape->brush()->brush());
    if (m_shape->hasStroke())
        painter->strokePath(m_shape->path(), m_shape->pen()->pen());
}



DGShapePainter::ScaleMode DGShapePainter::scaleMode() const
{
    return m_scaleMode;
}

void DGShapePainter::componentComplete()
{
    QQuickPaintedItem::componentComplete();
    update();
}

QRectF DGShapePainter::contentsBoundingRect() const
{

    return isVisible() ? QRectF(0, 0, width(), height()) : QRectF();
}

void DGShapePainter::setShape(DGShape *arg)
{
    if (m_shape != arg) {
        if (m_shape)
            disconnect(m_shape, 0, this, 0);
        m_shape = arg;
        connect(m_shape, SIGNAL(boundingRectChanged(QRectF)), this, SLOT(updateContentsSize()));
        connect(m_shape, SIGNAL(pathChanged()), this, SLOT(update()));
        QRectF rect = arg->boundingRect();
        setContentsSize(rect.size().toSize());
        emit shapeChanged(arg);
        update();
    }
}

void DGShapePainter::setScaleMode(DGShapePainter::ScaleMode arg)
{
    if (m_scaleMode != arg) {
        m_scaleMode = arg;
        emit scaleModeChanged(arg);
        update();
    }
}

QRectF DGShapePainter::viewRect() const
{
    return m_viewRect;
}

qreal DGShapePainter::shapeScale() const
{
    return m_shapeScale;
}

void DGShapePainter::setViewRect(QRectF arg)
{
    if (m_viewRect != arg) {
        m_viewRect = arg;
        emit viewRectChanged(arg);
        if (!signalsBlocked())
            update();
    }
}

void DGShapePainter::setShapeScale(qreal arg)
{
    if (m_shapeScale != arg) {
        m_shapeScale = arg;
        emit shapeScaleChanged(arg);
        if (!signalsBlocked())
            update();
    }
}

void DGShapePainter::updateContentsSize()
{
    setContentsSize(m_shape->boundingRect().size().toSize());
    //    qDebug() << "The shape size has been updated" << m_shape->boundingRect().size().toSize() << width() << height();
    update();
}

DGShapeItem::DGShapeItem(QQuickItem *parent)
    : DGSceneItem(parent),
      m_lazyRepaint(false)
{
    shapePainter = new DGShapePainter(this);
    shapePainter->setTransformOrigin(QQuickItem::TopLeft);
    connect(this, &DGSceneItem::sceneChanged, this, &DGShapeItem::updateSceneConnections);
}

QRectF DGShapeItem::viewportRect() const
{
    return m_viewportRect;
}

void DGShapeItem::componentComplete()
{
    DGSceneItem::componentComplete();

    shapePainter->setWidth(m_viewportRect.width());
    shapePainter->setHeight(m_viewportRect.height());
    if (scene())
        connect (scene(), &DGScene::scaleChanged, this, &DGShapeItem::updatePainterScale);
}

bool DGShapeItem::lazyRepaint() const
{
    return m_lazyRepaint;
}

QSizeF DGShapeItem::cacheSize() const
{
    return m_cacheSize;
}

void DGShapeItem::setShape(DGShape *arg)
{
    if (shapePainter->shape() != arg) {
        shapePainter->setShape(arg);
        emit shapeChanged(arg);
    }
}

bool isStepScale(qreal scale)
{
    qreal step1 = log2(scale);
    qreal step2 = log2(scale / 1.5);
    //    qDebug() << (step1 == floor(step1)) << (step2 == floor(step2));
    return (qFuzzyCompare(step1, floor(step1))) || qFuzzyCompare(step2, floor(step2));
}


void DGShapeItem::setViewportRect(QRectF viewportRect)
{
    if (!scene())
        return;
    m_viewportRect = viewportRect;
    qreal s = scene()->scale();
    if (m_lazyRepaint && !isStepScale(s))
        return;
    QRectF itemRect = QRectF(x() * s, y() * s, width() * s, height() * s);
    PaintState newState = viewportRect.intersects(itemRect) ? (viewportRect.contains(itemRect) ? Visible : PartlyVisible ) : Invisible;
    if (newState == Visible) {
        if (newState != state) {
            state = Visible;
            shapePainter->setVisible(true);
            shapePainter->update();
            shapePainter->setPosition(QPointF(0, 0));
            shapePainter->setViewRect(QRectF(0, 0, width() * s, height() * s));
            qDebug() << "Item" <<  this << "became fully visible" << state << shapePainter->width() << shapePainter->height();
        }
        shapePainter->setSize(QSizeF(width() * s, height() * s));
    }
    if (newState == PartlyVisible) {
        if (newState != state) {
            state = PartlyVisible;
            shapePainter->setVisible(true);
            qDebug() << "Item became partly visible";
        }
        QRectF intersected = viewportRect.intersected(itemRect).translated(-itemRect.topLeft());
        qreal ix, iy, iw, ih;
        intersected.getRect(&ix, &iy, &iw, &ih);
        ix /= s;
        iy /= s;
        iw /= s;
        ih /= s;
        QRectF itemR = QRectF(ix, iy, iw, ih);
        shapePainter->setViewRect(itemR);
        shapePainter->setPosition(itemR.topLeft());
        shapePainter->setSize(intersected.size());
        shapePainter->update();
    }

    if (newState == Invisible && newState != state) {
        qDebug() << "Item became invisible";
        state = Invisible;
        shapePainter->setVisible(false);

    }
//    qDebug() << "vprect=" << viewportRect << newState;
//    switch (newState) {
//    case 0:
//        qDebug() << "Item became invisible";
//        state = Invisible;
//        shapePainter->setVisible(false);
//        break;
//    case 1:
//        if (!shapePainter->isVisible())
//            shapePainter->setVisible(true);
//        state = PartlyVisible;
//        qDebug() << "Item became partly visible" << state;
//        QRectF intersected = viewportRect.intersected(itemRect).translated(-itemRect.topLeft());
//        qreal ix, iy, iw, ih;
//        intersected.getRect(&ix, &iy, &iw, &ih);
//        ix /= s;
//        iy /= s;
//        iw /= s;
//        ih /= s;
//        QRectF itemR = QRectF(ix, iy, iw, ih);
//        shapePainter->setViewRect(itemR);
//        shapePainter->setPosition(itemR.topLeft());
//        shapePainter->setSize(intersected.size());
//        shapePainter->update();
//        break;
//    default:
//        break;
//    }
//    if (state != Visible && (viewportRect.contains(itemRect) || viewportRect.isNull())) {
//        state = Visible;

//    } else if (viewportRect.intersects(itemRect)) {
//        if (!shapePainter->isVisible())
//            shapePainter->setVisible(true);

//    } else if (state ) {

////        shapePainter->setSize(QSizeF(width(), height()));
//    }
    emit viewportRectChanged(viewportRect);
}

void DGShapeItem::setLazyRepaint(bool arg)
{
    if (m_lazyRepaint != arg) {
        m_lazyRepaint = arg;
        emit lazyRepaintChanged(arg);
    }
}

void DGShapeItem::setCacheSize(QSizeF arg)
{
    if (m_cacheSize != arg) {
        m_cacheSize = arg;
        emit cacheSizeChanged(arg);
    }
}


void DGShapeItem::updatePainterScale()
{
    qreal s = scene()->scale();
    if (m_lazyRepaint && !isStepScale(s))
        return;
    shapePainter->blockSignals(true);
    shapePainter->setScale(1 / s);
    shapePainter->setShapeScale(s);
//    shapePainter->setWidth(width() * s);
//    shapePainter->setHeight(height() * s);
    shapePainter->blockSignals(false);
    setViewportRect(m_viewportRect);
}

void DGShapeItem::updateSceneConnections(DGScene *s)
{
//    qDebug() << "scene changed to" << s;
    connect(s, &DGScene::scaleChanged, this, &DGShapeItem::updatePainterScale);
}
