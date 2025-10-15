#ifndef DGSHAPEITEM_H
#define DGSHAPEITEM_H

#include <QQuickPaintedItem>
#include "dgshape.h"
#include "dgscene.h"

class DGShapePainter : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing)
    Q_PROPERTY(DGShape* shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(ScaleMode scaleMode READ scaleMode WRITE setScaleMode NOTIFY scaleModeChanged)
    Q_PROPERTY(QRectF viewRect READ viewRect WRITE setViewRect NOTIFY viewRectChanged)
    Q_PROPERTY(qreal shapeScale READ shapeScale WRITE setShapeScale NOTIFY shapeScaleChanged)
    Q_ENUMS(ScaleMode)
public:

    enum ScaleMode {
        NoScale,
        ScaleToFit
    };
    DGShapePainter(QQuickItem *parent = 0);
    void componentComplete();
    QRectF contentsBoundingRect() const;
    DGShape* shape() const;
    void paint(QPainter *painter);
    inline void render(QPainter *painter);
    ScaleMode scaleMode() const;
    void setShape(DGShape* arg);
    void setScaleMode(ScaleMode arg);
    QRectF viewRect() const;
    qreal shapeScale() const;

signals:
    void shapeChanged(DGShape* arg);
    void scaleModeChanged(ScaleMode arg);
    void viewRectChanged(QRectF arg);
    void shapeScaleChanged(qreal arg);

public slots:
    void setViewRect(QRectF arg);
    void setShapeScale(qreal arg);

private slots:
    void updateContentsSize();

private:
    DGShape* m_shape;
    ScaleMode m_scaleMode;
    QRectF m_viewRect;
    qreal m_shapeScale;
};

class DGShapeItem : public DGSceneItem
{
    Q_OBJECT
    Q_PROPERTY(DGShape* shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(bool lazyRepaint READ lazyRepaint WRITE setLazyRepaint NOTIFY lazyRepaintChanged)
    Q_PROPERTY(QSizeF cacheSize READ cacheSize WRITE setCacheSize NOTIFY cacheSizeChanged)
public:
    DGShapeItem(QQuickItem *parent = 0);
    DGShape* shape() const { return shapePainter->shape(); }
    QRectF viewportRect() const;

    void componentComplete();

    bool lazyRepaint() const;
    QSizeF cacheSize() const;

public slots:
    void setShape(DGShape* arg);
    void setViewportRect(QRectF arg);
    void setLazyRepaint(bool arg);

    void setCacheSize(QSizeF arg);

signals:
    void shapeChanged(DGShape* arg);
    void viewportRectChanged(QRectF arg);
    void lazyRepaintChanged(bool arg);
    void cacheSizeChanged(QSizeF arg);

private slots:
    void updatePainterScale();
    void updateSceneConnections(DGScene *s);

private:
    enum PaintState
    {
        Visible,
        PartlyVisible,
        Invisible
    };
    DGShapePainter *shapePainter;
    PaintState state;
    QRectF m_viewportRect;
    bool m_lazyRepaint;
    QSizeF m_cacheSize;
};

#endif // DGSHAPEITEM_H
