#ifndef DGSCENE_H
#define DGSCENE_H

#include <QQuickItem>
#include <QQmlListProperty>
#include "dgcursor.h"
#include "dgitem.h"

class DGScene;

class DGSceneItem : public DGItem
{
    Q_OBJECT

    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(DGScene* scene READ scene WRITE setScene NOTIFY sceneChanged)

public:
    friend class DGScene;
    explicit DGSceneItem(QQuickItem *parent = 0);
    bool selected() const;
    DGScene *scene() const;
    QRectF contentsBoundingRect() const;

signals:
    void selectedChanged(bool arg);
    void sceneChanged(DGScene* arg);

public slots:
    void addToScene(DGScene *scene);
    void setSelected(bool arg);
    void setScene(DGScene* arg);

protected:
    void componentComplete();

private:
    qreal m_canvasX;
    bool m_scalable;
    DGScene* m_scene;
    bool m_selected;
};


class DGScene : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal sceneWidth READ sceneWidth WRITE setSceneWidth NOTIFY sceneWidthChanged)
    Q_PROPERTY(qreal sceneHeight READ sceneHeight WRITE setSceneHeight NOTIFY sceneHeightChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(qreal viewportX READ viewportX WRITE setViewportX NOTIFY viewportXChanged)
    Q_PROPERTY(qreal viewportY READ viewportY WRITE setViewportY NOTIFY viewportYChanged)
    Q_PROPERTY(qreal viewportWidth READ viewportWidth WRITE setViewportWidth NOTIFY viewportWidthChanged)
    Q_PROPERTY(qreal viewportHeight READ viewportHeight WRITE setViewportHeight NOTIFY viewportHeightChanged)

    Q_PROPERTY(QQmlListProperty<DGSceneItem> items READ qmlItems NOTIFY itemsChanged)

public:
    explicit DGScene(QQuickItem *parent = 0);

    // property getters
    qreal zoom() const { return m_zoom; }
    qreal sceneWidth() const { return m_sceneWidth; }
    qreal sceneHeight() const { return m_sceneHeight; }
    qreal viewportX() const;
    qreal viewportY() const;
    qreal viewportWidth() const;
    qreal viewportHeight() const;

    // property setters
    void setZoom(qreal arg);
    void setSceneWidth(qreal arg);
    void setSceneHeight(qreal arg);
    void setViewportX(qreal arg);
    void setViewportY(qreal arg);
    void setViewportWidth(qreal arg);
    void setViewportHeight(qreal arg);
    void childEvent(QChildEvent *e);

    QRectF contentsBoundingRect() const;

signals:
    void zoomChanged(qreal arg);
    void sceneWidthChanged(qreal arg);
    void sceneHeightChanged(qreal arg);
    void viewportXChanged(qreal arg);
    void viewportYChanged(qreal arg);
    void viewportWidthChanged(qreal arg);
    void viewportHeightChanged(qreal arg);

    void itemsChanged();

public slots:
    void addItem(DGSceneItem *item);

private:
    QList<DGSceneItem*> m_items;

    qreal m_zoom;
    qreal m_sceneWidth;
    qreal m_sceneHeight;
    qreal m_viewportX;
    qreal m_viewportY;
    qreal m_viewportWidth;
    qreal m_viewportHeight;

    QQmlListProperty<DGSceneItem> qmlItems();
    static void qmlItemsAppend(QQmlListProperty<DGSceneItem> *list, DGSceneItem *object);
    static int qmlItemsCount(QQmlListProperty<DGSceneItem> *list);
    static DGSceneItem *qmlItemsAt(QQmlListProperty<DGSceneItem> *list, int index);
};

#endif // DGSCENE_H
