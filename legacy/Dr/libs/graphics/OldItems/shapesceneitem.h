#ifndef SHAPESCENEITEM_H
#define SHAPESCENEITEM_H
#include "sceneitem.h"
#include "graphicsitem.h"

#define sup (outer<ShapeSceneItem>())

class ShapeSceneItem : public SceneItem
{
    Q_OBJECT
    Q_PROPERTY(Pen *pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(Brush* brush READ brush WRITE setBrush NOTIFY brushChanged)

    Q_PROPERTY(qreal minimumWidth READ minimumWidth WRITE setMinimumWidth NOTIFY minimumWidthChanged)

    Q_PROPERTY(bool drawOutline READ drawOutline WRITE setDrawOutline NOTIFY drawOutlineChanged)
    Q_PROPERTY(Pen* outlinePen READ outlinePen WRITE setOutlinePen NOTIFY outlinePenChanged)
    Q_PROPERTY(Brush* outlineBrush READ outlineBrush WRITE setOutlineBrush NOTIFY outlineBrushChanged)
    friend class GraphicsPathItem;

    class GraphicsPathItem : public QuickGraphicsShapeItem
    {

    public:
        GraphicsPathItem(SceneItem *item, QGraphicsItem *parent = 0);
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        bool drawOutline = false;
        Pen* outlinePen = nullptr;
        Brush* outlineBrush = nullptr;

    };

    PROTO_ITEM(GraphicsPathItem)

    public:
        explicit ShapeSceneItem(SceneItem *parent = 0);
    Pen *pen() const;
    Brush *brush() const;
    qreal minimumWidth() const;
    bool drawOutline() const;
    Pen *outlinePen() const;
    Brush* outlineBrush() const;

    void setPen(Pen * arg);
    void setBrush(Brush* arg);
    void setMinimumWidth(qreal arg);
    void setDrawOutline(bool arg);
    void setOutlinePen(Pen* arg);
    void setOutlineBrush(Brush* arg);


signals:
    void penChanged(Pen *pen);
    void brushChanged(Brush *brush);
    void minimumWidthChanged(qreal minimumWidth);
    void drawOutlineChanged(bool  drawOutline);
    void outlinePenChanged(Pen *outlinePen);
    void outlineBrushChanged(Brush *outlineBrush);


protected:
    virtual void beforeDraw(QPainter *painter);
    virtual void afterDraw(QPainter *painter);
    void setBoundingRectDirty();
    void setPath(const QPainterPath &path);
    QPainterPath &pathRef();
    const QPainterPath &pathRef() const;
};

#endif // SHAPESCENEITEM_H
