#ifndef MYMOUSEAREA_H
#define MYMOUSEAREA_H
#include "private/qquickmousearea_p.h"
#include <QPainterPath>
#include "painterpath.h"
class Shape;
class MouseArea : public QQuickMouseArea
{
    Q_OBJECT
    Q_ENUMS(HitTestMode)
    Q_PROPERTY(PainterPath shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(qreal outlineTolerance READ outlineTolerance WRITE setOutlineTolerance NOTIFY outlineToleranceChanged)
    Q_PROPERTY(HitTestMode hitTestMode READ hitTestMode WRITE setHitTestMode NOTIFY hitTestModeChanged)

public:
    enum HitTestMode {
        BoundingBox,
        Outline,
        Fill,
        OutlineAndFill
    };

    explicit MouseArea(QQuickItem *parent = 0);
    void setShape(PainterPath arg);
    void setHitTestMode(HitTestMode arg);
    Shape* shape() const;
    HitTestMode hitTestMode() const;
    qreal outlineTolerance() const;

public slots:
    void setOutlineTolerance(qreal arg);

protected:
    bool contains(const QPointF &point) const;

signals:
    void shapeChanged();
    void hitTestModeChanged(HitTestMode arg);
    void outlineToleranceChanged(qreal arg);

private slots:
    void updateOutline();

private:
    PainterPath m_shape;
    QPainterPath outline;
    HitTestMode m_hitTestMode;
    qreal m_outlineTolerance;
};
#endif // MYMOUSEAREA_H
