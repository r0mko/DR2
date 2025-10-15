#ifndef WIRINGELEMENT_H
#define WIRINGELEMENT_H

#include <QObject>
#include "sceneitem.h"
#include <QBrush>

class WiringElement : public SceneItem
{
    Q_OBJECT
    Q_PROPERTY(bool drawOutline READ drawOutline WRITE setDrawOutline NOTIFY drawOutlineChanged)
    Q_PROPERTY(Pen* outlinePen READ outlinePen WRITE setOutlinePen NOTIFY outlinePenChanged)
    Q_PROPERTY(Brush* outlineBrush READ outlineBrush WRITE setOutlineBrush NOTIFY outlineBrushChanged)
    Q_PROPERTY(QPainterPath shape READ shape NOTIFY shapeChanged)
    Q_PROPERTY(qreal outlineMargin READ outlineMargin WRITE setOutlineMargin NOTIFY outlineMarginChanged)
    Q_PROPERTY(bool fixedScale READ fixedScale WRITE setFixedScale NOTIFY fixedScaleChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(bool debugNodes READ debugNodes WRITE setDebugNodes NOTIFY debugNodesChanged)

    qreal m_zoomFactor;
    bool m_fixedScale;
    bool m_debugNodes;

    friend class WiringElementItem;

    class WiringElementItem : public QuickGraphicsItem
    {
    public:
        bool m_drawOutline;
        Pen* m_outlinePen;
        Brush* m_outlineBrush;
        qreal m_outlineMargin;

        WiringElementItem(SceneItem *item, QGraphicsItem *parent = 0);
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        QRectF calculateBoundingRect() const;
        QPainterPath shape() const;
    };

protected:
    PROTO_ITEM(WiringElementItem);


public:
    WiringElement(SceneItem *parent = 0);
    ~WiringElement();
    bool drawOutline() const;
    Pen* outlinePen() const;
    Brush* outlineBrush() const;
    qreal outlineMargin() const;
    bool fixedScale() const;
    qreal zoomFactor() const;
    bool debugNodes() const;

    void setDrawOutline(bool arg);
    void setOutlinePen(Pen* arg);
    void setOutlineBrush(Brush* arg);
    void setOutlineMargin(qreal arg);
    void setFixedScale(bool arg);
    void setZoomFactor(qreal arg);
    void setDebugNodes(bool arg);
    virtual QPainterPath shape() const;

public slots:
    void rebuild();

signals:
    void drawOutlineChanged(bool arg);
    void outlinePenChanged(Pen* arg);
    void outlineBrushChanged(Brush* arg);
    void shapeChanged(QPainterPath arg);
    void outlineMarginChanged(qreal arg);
    void zoomFactorChanged(qreal arg);
    void fixedScaleChanged(bool arg);
    void debugNodesChanged(bool arg);

protected:
    virtual void build() {}
    virtual void paint(QPainter *painter);
    virtual QRectF getBoundingRect() const;
    void initScene();
    void drawPolygonNodes(QPainter *painter, const QPolygonF &polygon, const QPen &pen) const;
    QPainterPath buildOutline(const QPainterPath &path, const QPen &pen, const QBrush &brush = QBrush()) const;
};





#endif // WIRINGELEMENT_H
