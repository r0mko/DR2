#ifndef GRAPHRIBSLIDERSCENEITEM_H
#define GRAPHRIBSLIDERSCENEITEM_H
#include "graphribsceneitem.h"
#include "rectsceneitem.h"
#include "shape.h"
#include "private/qquickrectangle_p.h"
#ifdef sup
#undef sup
#endif
#define sup (outer<GraphRibSlider>())
class GraphRibSlider : public SceneItem
{
    Q_OBJECT
    Q_PROPERTY(GraphRibSceneItem* rib READ rib WRITE setRib NOTIFY ribChanged)
    Q_PROPERTY(qreal pathPosition READ pathPosition WRITE setPathPosition NOTIFY pathPositionChanged)
    Q_PROPERTY(bool bindToPath READ bindToPath WRITE setBindToPath NOTIFY bindToPathChanged)
    Q_PROPERTY(bool frozen READ isFrozen WRITE setFrozen NOTIFY frozenChanged)

    Q_PROPERTY(bool atBeginning READ atBeginning NOTIFY atBeginningChanged)
    Q_PROPERTY(bool atEnd READ atEnd NOTIFY atEndChanged)

    Q_PROPERTY(Pen* pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(Brush* brush READ brush WRITE setBrush NOTIFY brushChanged)
    Q_PROPERTY(Shape* handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(QPointF desiredPos READ desiredPos NOTIFY desiredPosChanged)
    Q_PROPERTY(GraphRibSlider* nextSlider READ nextSlider WRITE setNextSlider NOTIFY nextSliderChanged)
    Q_PROPERTY(int originNodeId READ originNodeId WRITE setOriginNodeId NOTIFY originNodeIdChanged)
    Q_PROPERTY(qreal inPosition READ inPosition WRITE setInPosition NOTIFY inPositionChanged)

    Q_PROPERTY(qreal outPosition READ outPosition WRITE setOutPosition NOTIFY outPositionChanged)


    class GraphRibSliderItem : public QuickGraphicsShapeItem
    {
        GraphRibSceneItem* m_rib;
        qreal m_pathPos;
        Pen* m_pen;
        Brush* m_brush;
        Shape* m_handle;
        qreal m_vscale;
        bool m_atBeginning;
        bool m_atEnd;
        QPointF m_desiredPos;
        bool m_bindToPath;
        bool m_frozen;
        GraphRibSlider* m_nextSlider;
        int m_originNodeId;
        qreal m_inPosition;
        qreal m_outPosition;
    public:
        GraphRibSliderItem(SceneItem *item, QGraphicsItem *parent = 0);
        QRectF calculateBoundingRect() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

        GraphRibSceneItem *rib() const;
        qreal pathPosition() const;
        Pen *pen() const;
        Brush *brush() const;
        Shape *handle() const;
        qreal vscale() const;
        bool atBeginning() const;
        bool atEnd() const;
        QPointF desiredPos() const;
        bool bindToPath() const;
        bool frozen() const;
        GraphRibSlider *nextSlider() const;
        int originNodeId() const;
        qreal inPosition() const;
        qreal outPosition() const;

        void setRib(GraphRibSceneItem *rib);
        void setPathPosition(const qreal &pathPosition);
        void setPen(Pen *pen);
        void setBrush(Brush *brush);
        void setHandle(Shape *handle);
        void setVscale(const qreal &vscale);
        void setAtBeginning(bool arg);
        void setAtEnd(bool arg);
        void setBindToPath(bool bindToPath);
        void setFrozen(bool frozen);
        void setNextSlider(GraphRibSlider *nextSlider);
        void setOriginNodeId(int originNodeId);
        void setInPosition(qreal arg);
        void setOutPosition(const qreal &outPosition);

    protected:
        QVariant itemChange(GraphicsItemChange change, const QVariant &value);


    };

    PROTO_ITEM(GraphRibSliderItem);




public:
    explicit GraphRibSlider(SceneItem *parent = 0);

    GraphRibSceneItem* rib() const;
    qreal pathPosition() const;
    Pen *pen() const;
    Brush* brush() const;
    Shape* handle() const;
    QPointF desiredPos() const;
    bool bindToPath() const;
    bool atBeginning() const;
    bool atEnd() const;
    bool isFrozen() const;
    GraphRibSlider* nextSlider() const;
    int originNodeId() const;

    qreal inPosition() const;
    qreal outPosition() const;

    void setRib(GraphRibSceneItem* arg);
    void setPathPosition(qreal arg);
    void setPen(Pen* arg);
    void setBrush(Brush* arg);
    void setHandle(Shape* arg);
    void setBindToPath(bool arg);
    void setFrozen(bool arg);
    void setNextSlider(GraphRibSlider* arg);
    void setOriginNodeId(int arg);
    void setInPosition(qreal arg);
    void setOutPosition(qreal arg);

    Q_INVOKABLE qreal distanceTo(qreal x, qreal y) const;
    Q_INVOKABLE qreal distanceTo(const QPointF &pos) const;
    Q_INVOKABLE qreal distanceTo(GraphRibSlider *other) const;
    Q_INVOKABLE QVariant buildPolygon() const;
    Q_INVOKABLE QVariant buildPointList() const;


public slots:

signals:
    void ribChanged(GraphRibSceneItem* arg);
    void pathPositionChanged(qreal arg);
    void penChanged(Pen* arg);
    void brushChanged(Brush* arg);
    void handleChanged(Shape* arg);
    void atBeginningChanged(bool arg);
    void atEndChanged(bool arg);
    void desiredPosChanged(QPointF arg);
    void bindToPathChanged(bool arg);
    void frozenChanged(bool arg);
    void nextSliderChanged(GraphRibSlider* arg);
    void originNodeIdChanged(int arg);
    void outPositionChanged(qreal arg);
    void inPositionChanged(qreal arg);

private slots:
    void updateSceneConnections();
    void updateBounds(qreal scale);
private:
    void polyLineMerge(QPolygonF &dst, const QPolygonF &src) const;

};

#endif // GRAPHRIBSLIDERSCENEITEM_H
