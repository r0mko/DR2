#ifndef QUICKGRAPHICSSHAPEITEM_H
#define QUICKGRAPHICSSHAPEITEM_H
#include <QGraphicsItem>
class Pen;
class Brush;
class SceneItem;

class QuickGraphicsItem : public QGraphicsItem
{
    SceneItem *m_item;
    mutable bool m_brectDirty;
    mutable QRectF m_brect;
    bool m_isDestroyed;

public:
    QuickGraphicsItem(SceneItem *qmlItem, QGraphicsItem *parent = 0) :
        QGraphicsItem(parent),
        m_item(qmlItem),
        m_brectDirty(true),
        m_isDestroyed(false)
    {}
    virtual ~QuickGraphicsItem();
    QRectF boundingRect() const;
    void destroy();

    SceneItem *super() const { return m_item; }

    template <class T>
    T *outer() const { return m_isDestroyed ? nullptr : reinterpret_cast<T*>(m_item); }

    bool boundingRectDirty() const;
    void setBoundingRectDirty();


protected:
    bool isDestroyed() const;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    virtual QRectF calculateBoundingRect() const = 0;
    void updateBoundingRect(const QRectF& rect);
};

class QuickGraphicsShapeItem : public QuickGraphicsItem
{
    Pen *m_pen;
    Brush *m_brush;
    qreal m_minimumWidth;
    mutable QPainterPath m_outlinePath;
    mutable bool shapeDirty;
    mutable qreal lastScale;
    QPainterPath m_path;

public:
    QuickGraphicsShapeItem(SceneItem *item, QGraphicsItem *parent = 0);

    Brush *brush() const;
    Pen *pen() const;
    qreal minimumWidth() const;
    QPainterPath path() const;
    QPainterPath &pathRef();


    void setBrush(Brush *brush);
    void setPen(Pen *pen);
    void setMinimumWidth(const qreal &minimumWidth);
    void setPath(const QPainterPath &path);
    void setShapeDirty();

    virtual QPainterPath shape() const;
    virtual QRectF calculateBoundingRect() const;

    QPainterPath outline() const;
};

#endif // QUICKGRAPHICSSHAPEITEM_H
