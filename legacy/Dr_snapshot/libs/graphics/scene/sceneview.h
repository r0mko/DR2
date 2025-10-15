#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include "scenemodel.h"

#include <QQuickPaintedItem>
class SceneRenderer;
class SceneView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(SceneModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect NOTIFY viewportRectChanged)

    SceneModel* m_model;

public:
    SceneView(QQuickItem *parent = 0);
    void paint(QPainter *painter);
    void setModel(SceneModel *model);
    void setSceneRect(QRectF sceneRect);
    SceneModel *model() const;

    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverEnterEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
    void mousePressEvent(QMouseEvent *event);

    inline const QTransform &viewTransform() const { return m_viewTransform; }

    QRectF viewportRect() const;

public slots:
    void setViewTransform(const QTransform &viewTransform);

signals:
    void modelChanged();
    void viewTransformChanged(QTransform viewTransform);

    void viewportRectChanged(QRectF viewportRect);

private:


    QGraphicsScene *m_scene;
    QQuickItem *m_viewport;
    SceneRenderer *m_renderer;

    QTransform m_viewTransform;
    QPointF lastMouseMoveScenePoint;
    QPoint lastMouseMoveScreenPoint;
    QPointF lastRubberbandScenePoint;
    QPoint mousePressScreenPoint;
    QPointF mousePressScenePoint;
    QPoint mousePressViewPoint;
    quint32 sceneInteractionAllowed : 1;
    quint32 useLastMouseEvent : 1;
    quint32 sceneRectExplicitlySet : 1;
    quint32 renderCount : 29;
    QMouseEvent lastMouseEvent;
    Qt::MouseButton mousePressButton;
    bool m_autoUpdate = true;
    QRectF m_viewportRect;

    void replayLastMouseEvent();
    void storeMouseEvent(QMouseEvent *event);
    void mouseMoveEventHandler(QMouseEvent *event);

};

#endif // SCENEVIEW_H
