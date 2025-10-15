#ifndef GRAPHROADITEM_H
#define GRAPHROADITEM_H

#include <QQuickItem>
#include "sceneitem.h"
#include "pathguide.h"

class GraphRoute;
class GraphRoadItem : public SceneItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<RibReference> ribs READ qmlRibs NOTIFY ribsChanged)
    Q_PROPERTY(GraphRoute* route READ route CONSTANT)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(bool chamfer READ chamfer WRITE setChamfer NOTIFY chamferChanged)

    friend class GraphRoadGraphicsItem;
    class GraphRoadGraphicsItem : public QuickGraphicsShapeItem
    {
    public:
        GraphRoute* m_route;
        QVector<QLineF> lines;
        qreal m_zoomFactor;
        bool m_chamfer;
        GraphRoadGraphicsItem(SceneItem *item, QGraphicsItem *parent = 0);
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        QRectF calculateBoundingRect() const;
    };

    PROTO_ITEM(GraphRoadGraphicsItem)

public:
    explicit GraphRoadItem( SceneItem* parent = nullptr );
    ~GraphRoadItem();

    GraphRoute *route() const;
    qreal zoomFactor() const;
    bool chamfer() const;

signals:
    void prevChanged();
    void nextChanged();
    void ribsChanged();
    void zoomFactorChanged(qreal arg);
    void chamferChanged(bool arg);

public slots:
    void clearRibs();
    void appendRib(PathGuide *guide, qreal inPosition, qreal outPosition, qreal offset);
    void updatePath();
    void setZoomFactor(qreal arg);
    void setChamfer(bool arg);

private:
    void polyLineMerge(QPolygonF &dst, const QPolygonF &src) const;
    QQmlListProperty<RibReference> qmlRibs();
    static int qmlRouteCount(QQmlListProperty<RibReference> *list);
    static RibReference *qmlRouteAt(QQmlListProperty<RibReference> *list, int index);


};



#endif // GRAPHROADITEM_H
