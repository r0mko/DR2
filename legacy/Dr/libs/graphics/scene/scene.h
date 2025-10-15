#ifndef SCENE_H
#define SCENE_H

#include <QQuickItem>
#include <qqml.h>
#include <QTransform>


class QGraphicsScene;
class SceneItem;
class GraphicsPath;
class SceneRenderer;

class SceneData : public QObjectUserData
{
//    Q_GADGET
public:
    SceneData() {}
    QTransform viewTransform;
};

class Scene : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<SceneItem> sceneItems READ qmlSceneItems NOTIFY sceneItemsChanged)
    Q_PROPERTY(QQmlListProperty<SceneItem> selectedItems READ qmlSelectedItems NOTIFY selectedItemsChanged)
    Q_PROPERTY(QRectF sceneRect READ sceneRect WRITE setSceneRect NOTIFY sceneRectChanged)
    Q_PROPERTY(QQuickItem *viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing NOTIFY antialiasingChanged)
    Q_PROPERTY(SceneRenderer* renderer READ renderer CONSTANT)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QTransform viewTransform READ viewTransform NOTIFY viewTransformChanged)
    Q_PROPERTY(qreal viewportScale READ viewportScale NOTIFY viewportScaleChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(bool enableBsp READ enableBsp WRITE setEnableBsp NOTIFY enableBspChanged)
    Q_PROPERTY(int bspTreeDepth READ bspTreeDepth WRITE setBspTreeDepth NOTIFY bspTreeDepthChanged)
    Q_PROPERTY(QGraphicsScene* qGraphicsScene READ qGraphicsScene CONSTANT)
    Q_PROPERTY(bool notifyViewTransformUpdates READ notifyViewTransformUpdates WRITE setNotifyViewTransformUpdates NOTIFY notifyViewTransformUpdatesChanged)


public:
    explicit Scene(QQuickItem *parent = 0);
    ~Scene();

    Q_INVOKABLE SceneItem *itemAt(const QPointF &pos) const;
    Q_INVOKABLE SceneItem *itemAt(qreal x, qreal y) const;

    void classBegin() {}
    void componentComplete();

    QList<SceneItem*> sceneItems() const;

    QRectF sceneRect() const;
    QQuickItem *viewport() const;
    QTransform viewTransform() const;
    bool antialiasing() const;
//    bool enabled() const;

    QGraphicsScene *qGraphicsScene() const;
    void setQGraphicsScene(QGraphicsScene *scene);

    SceneRenderer* renderer() const;
    bool autoUpdate() const;
    QRectF viewportRect() const;
    bool enableBsp() const;
    int bspTreeDepth() const;
    qreal viewportScale() const;

    bool contains(const QPointF &point) const;
    SceneData *sceneInfo() const;

    bool notifyViewTransformUpdates() const;
    void setNotifyViewTransformUpdates(bool notifyViewTransformUpdates);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverEnterEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
    void childEvent(QChildEvent *e);

public slots:
    void addItem(SceneItem *item);
    void removeItem(SceneItem *item);
    void setSceneRect(QRectF arg);
    void clearSelection();

    void setSelectionArea(const QRectF &rect, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);
    void setSelectionArea(GraphicsPath *path, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape);

signals:
    void itemAdded(SceneItem *item);
    void itemRemoved(SceneItem *item);
    void sceneRectChanged(const QRectF &rect);
    void sceneItemsChanged();
    void selectedItemsChanged();
    void selectionChanged();
    void changed();
    void viewTransformChanged(const QTransform &viewTransform);
    void viewportChanged();
    void antialiasingChanged(bool arg);
    void rendererChanged(SceneRenderer* arg);
    void autoUpdateChanged(bool arg);
    void viewportRectChanged(QRectF arg);
    void enableBspChanged(bool arg);
    void bspTreeDepthChanged(int arg);
    void viewportScaleChanged(qreal viewportScale);
    void rendererScaleChanged(qreal);

    void notifyViewTransformUpdatesChanged(bool notifyViewTransformUpdates);

public slots:
    void onRendererScaleShanged();
    void setViewport(QQuickItem *arg);
    void setAntialiasing(bool arg);
    void setAutoUpdate(bool arg);
    void setEnableBsp(bool arg);
    void setBspTreeDepth(int arg);



private slots:
    void adjustView();
    void adjustSceneRect();

private:
    void initSceneImpl(QGraphicsScene *scene);
    friend class SceneItem;
    friend class SceneView;
    friend class SceneRenderer;

    void mouseMoveEventHandler(QMouseEvent *event);

    QGraphicsScene *    m_scene = nullptr;
    QQuickItem *        m_viewport = nullptr;
    SceneRenderer *     m_renderer = nullptr;

    QTransform          m_viewTransform;
    QPointF             lastMouseMoveScenePoint;
    QPoint              lastMouseMoveScreenPoint;
    QPointF             lastRubberbandScenePoint;
    QPoint              mousePressScreenPoint;
    QPointF             mousePressScenePoint;
    QPoint              mousePressViewPoint;
    quint32             sceneInteractionAllowed : 1;
    quint32             useLastMouseEvent : 1;
    quint32             sceneRectExplicitlySet : 1;
    quint32             m_notifyViewTransformUpdates : 1;
    quint32             renderCount : 29;
    QMouseEvent         lastMouseEvent;
    Qt::MouseButton     mousePressButton;
    bool                m_autoUpdate = true;

    QQmlListProperty<SceneItem> qmlSelectedItems();
    QQmlListProperty<SceneItem> qmlSceneItems();

    void replayLastMouseEvent();
    void storeMouseEvent(QMouseEvent *event);

    static void qmlItemsAppend(QQmlListProperty<SceneItem> *list, SceneItem *object);
    static SceneItem *qmlItemsAt(QQmlListProperty<SceneItem> *list, int index);
    static int qmlItemsCount(QQmlListProperty<SceneItem> *list);

    static int qmlSelectedItemsCount(QQmlListProperty<SceneItem> *list);
    static SceneItem *qmlSelectedItemsAt(QQmlListProperty<SceneItem> *list, int index);
};

#endif // SCENE_H
