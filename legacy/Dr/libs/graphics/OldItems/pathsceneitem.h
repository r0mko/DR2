#ifndef PATHSCENEITEM_H
#define PATHSCENEITEM_H
#include "sceneitem.h"
#include "graphicspath.h"
#include "graphicsitem.h"


class Pen;
class Brush;
class PathSceneItem : public SceneItem
{
    Q_OBJECT
    Q_PROPERTY(GraphicsPath* path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(Pen* pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(Brush* brush READ brush WRITE setBrush NOTIFY brushChanged)

    class PathGraphicsItem : public QuickGraphicsShapeItem
    {
        mutable QPainterPath m_opaqueArea;
        mutable bool opaqueAreaDirty;

    public:
        GraphicsPath *m_path;

        PathGraphicsItem(SceneItem *item, QGraphicsItem *parent = 0);
        QPainterPath shape() const;
        QPainterPath opaqueArea() const;

        bool isOpaqueAreaDirty() const;
        void setOpaqueAreaDirty(bool value = true);

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        QRectF calculateBoundingRect() const;

    private:
        void updateOutline() const;
    };

    PROTO_ITEM(PathGraphicsItem)

public:
    explicit PathSceneItem(SceneItem *parent = 0);

    GraphicsPath* path() const;
    Pen *pen() const;
    Brush *brush() const;

signals:
    void pathChanged(GraphicsPath *arg);
    void penChanged(Pen *arg);
    void brushChanged(Brush *arg);


protected:
    void componentComplete();
public slots:
    void setPath(GraphicsPath* arg);
    void setPen(Pen* arg);
    void setBrush(Brush* arg);
private slots:
    void updateOutline();
};

#endif // PATHSCENEITEM_H
