#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include <QQuickPaintedItem>

class Scene;
class SceneRenderer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Scene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(QRectF sceneRect READ sceneRect WRITE setSceneRect NOTIFY sceneRectChanged)
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing NOTIFY antialiasingChanged)

    QRectF m_sceneRect;
    Scene *m_scene = nullptr;
    uint renderCount = 0;

public:
    SceneRenderer(QQuickItem *parent = 0);
    void paint(QPainter *painter);
    void setScene(Scene *scene);
    void setSceneRect(QRectF sceneRect);
    Scene *scene() const;
    QRectF sceneRect() const;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
signals:
    void sceneChanged(Scene* arg);
    void sceneRectChanged(QRectF arg);
    void antialiasingChanged();
};






#endif // SCENERENDERER_H
