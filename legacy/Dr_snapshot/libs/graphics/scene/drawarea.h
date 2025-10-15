#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QMatrix4x4>
#include <qquickitem.h>



class DrawArea : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(DrawArea)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(QPointF viewportCenter READ viewportCenter WRITE setViewportCenter NOTIFY viewportCenterChanged)
    Q_PROPERTY(QPointF hoverPos READ hoverPos WRITE setHoverPos NOTIFY hoverPosChanged)

    static const qreal sseries[2];
    qreal m_zoom = 1;
    int m_zoomStep = 0;

    QRectF m_viewportRect;
    QMatrix4x4 m_tr;

    QPointF m_hoverPos;
    QPointF m_lastEventPos;

public:
    DrawArea(QQuickItem *parent = 0);
    ~DrawArea();
    qreal zoom() const;
    QRectF viewportRect() const;
    QPointF viewportCenter() const;
    void setZoom(qreal zoom);
    void setViewportCenter(QPointF viewportCenter);
    QPointF hoverPos() const;

public slots:

signals:
    void zoomChanged();
    void viewportRectChanged();
    void viewportCenterChanged(QPointF viewportCenter);

    // QQuickItem interface
    void hoverPosChanged(QPointF hoverPos);

protected:
    void hoverMoveEvent(QHoverEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

private:
    void setHoverPos(QPointF hoverPos);
    void updateMatrix();

};

#endif // DRAWAREA_H
