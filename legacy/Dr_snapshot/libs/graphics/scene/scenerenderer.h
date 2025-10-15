#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include <QQuickPaintedItem>
#include <qpixmap.h>

class Scene;
class QGraphicsItem;
class SceneRenderer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Scene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(QRectF sceneRect READ sceneRect WRITE setSceneRect NOTIFY sceneRectChanged)
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing NOTIFY antialiasingChanged)
    Q_PROPERTY(bool cacheEnabled READ cacheEnabled WRITE setCacheEnabled NOTIFY cacheEnabledChanged)

    QRectF m_sceneRect;
    Scene *m_scene = nullptr;
    uint renderCount = 0;

//    QImage cache;
    QPixmap cache_working;
    QPixmap cache_render;
    QVector<QGraphicsItem*> items;
    QRectF targetRect;
    QRect lastDirtyRect;

    bool m_cacheEnabled = false;

public:
    QVector<QGraphicsItem *> itemVector ;
    SceneRenderer(QQuickItem *parent = 0);
    void paint(QPainter *painter);
    void setScene(Scene *scene);
    void setSceneRect(QRectF sceneRect);
    Scene *scene() const;
    QRectF sceneRect() const;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    bool cacheEnabled() const;
    void setCacheEnabled(bool cacheEnabled);

public slots:

private slots:
    void renderCache(const QRect &rect = QRect());
    void checkDirty(const QList<QRectF> & region);
signals:
    void sceneChanged(Scene* arg);
    void sceneRectChanged(QRectF arg);
    void antialiasingChanged();
    void cacheEnabledChanged(bool cacheEnabled);

protected slots:
    void updateItemList();
};

#endif // SCENERENDERER_H
