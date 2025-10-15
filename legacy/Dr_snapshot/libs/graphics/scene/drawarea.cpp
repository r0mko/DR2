#include "drawarea.h"
#include <QDebug>
#include <QSGNode>
#include <QtMath>
const qreal DrawArea::sseries[] = { 1.0, 1.5 };

DrawArea::DrawArea(QQuickItem *parent):
    QQuickItem(parent)
{
    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()

     setFlag(ItemHasContents, true);
     setAcceptHoverEvents(true);
}

DrawArea::~DrawArea()
{
}

qreal DrawArea::zoom() const
{
    return m_zoom;
}

QRectF DrawArea::viewportRect() const
{
    return m_viewportRect;
}

QPointF DrawArea::viewportCenter() const
{
    return m_tr.inverted().map(boundingRect().center());
}

void DrawArea::setZoom(qreal zoom)
{
    if (m_zoom == zoom)
        return;
    m_zoom = zoom;
    emit zoomChanged();

    updateMatrix();
    update();
}

void DrawArea::setViewportCenter(QPointF viewportCenter)
{
    if (m_viewportRect.center() == viewportCenter)
        return;
    qDebug() << "Implement explicit panning";
}

QPointF DrawArea::hoverPos() const
{
    return m_hoverPos;
}

void DrawArea::hoverMoveEvent(QHoverEvent *event)
{
    setHoverPos(m_tr.inverted().map(event->posF()));
}



void DrawArea::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "key press" << event << event->key() << event->modifiers();
    event->ignore();
}

void DrawArea::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "key release" << event << event->key() << event->modifiers();
    event->ignore();
}

void DrawArea::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "mouse press" << event << event->buttons() << event->modifiers();
    event->ignore();
}

void DrawArea::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "mouse release" << event << event->buttons() << event->modifiers();
    event->ignore();
}

void DrawArea::wheelEvent(QWheelEvent *event)
{
    m_zoomStep += event->angleDelta().y() / 120;
    int ser_id = qAbs(m_zoomStep % 2);
    int pbase = m_zoomStep >> 1;
    qreal scale = qPow(2, pbase) * sseries[ser_id];
    m_lastEventPos = event->posF();
    setHoverPos(m_tr.inverted().map(m_lastEventPos));
    setZoom(scale);
}

void DrawArea::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    qDebug() << "geometry changed" << newGeometry << oldGeometry;
}

QSGNode *DrawArea::updatePaintNode(QSGNode *n, UpdatePaintNodeData *nd)
{
    nd->transformNode->setMatrix(m_tr);
    if (!n) {
        n = new QSGNode;
    }
    return n;
}

void DrawArea::setHoverPos(QPointF hoverPos)
{
    if (m_hoverPos == hoverPos)
        return;

    m_hoverPos = hoverPos;
    emit hoverPosChanged(hoverPos);
}

void DrawArea::updateMatrix()
{
    QVector3D tr1 = QVector3D(m_lastEventPos);
    QVector3D tr2 = QVector3D(m_hoverPos);
    m_tr.setToIdentity();
    m_tr.translate(tr1);
    m_tr.scale(m_zoom);
    m_tr.translate(-tr2);
    m_viewportRect = m_tr.inverted().mapRect(boundingRect());
    emit viewportRectChanged();
    emit viewportCenterChanged(m_viewportRect.center());
}
