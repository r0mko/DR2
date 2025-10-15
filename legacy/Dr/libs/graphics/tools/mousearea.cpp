#include "mousearea.h"
#include <QtMath>
#include <QPainterPathStroker>
#include "shape.h"

MouseArea::MouseArea(QQuickItem *parent) :
    QQuickMouseArea(parent),
    m_shape(nullptr)
{
}

Shape *MouseArea::shape() const
{
    return m_shape;
}

MouseArea::HitTestMode MouseArea::hitTestMode() const
{
    return m_hitTestMode;
}

qreal MouseArea::outlineTolerance() const
{
    return m_outlineTolerance;
}

void MouseArea::setOutlineTolerance(qreal arg)
{
    if (m_outlineTolerance == arg)
        return;

    m_outlineTolerance = arg;
    updateOutline();
    emit outlineToleranceChanged(arg);
}

void MouseArea::setHitTestMode(MouseArea::HitTestMode arg)
{
    if (m_hitTestMode == arg)
        return;

    m_hitTestMode = arg;
    updateOutline();
    emit hitTestModeChanged(arg);
}

bool MouseArea::contains(const QPointF &point) const
{
    if (QQuickMouseArea::contains(point)) {
        if (!m_shape)
            return true;
        switch (m_hitTestMode) {
        case BoundingBox:
            return true;
        case Fill:
            return m_shape->path().contains(point);
        case Outline:
            return outline.contains(point);
        case OutlineAndFill:
            return m_shape->path().contains(point) || outline.contains(point);

            break;
        default:
            break;
        }
        if (m_shape->path().contains(point))
            return true;
    }
    return false;
}

void MouseArea::updateOutline()
{
    if (!m_shape)
        return;
    if (m_hitTestMode != Outline && m_hitTestMode != OutlineAndFill)
        return;
    QPainterPathStroker s;
    s.setWidth(m_outlineTolerance);
    s.setJoinStyle(Qt::MiterJoin);
    s.setCapStyle(Qt::SquareCap);
    s.setDashPattern(Qt::SolidLine);
    outline = s.createStroke(m_shape->path());
}

void MouseArea::setShape(Shape *arg)
{
    if (m_shape == arg)
        return;
    if (m_shape) {
        disconnect(m_shape, 0, this, 0);
    }

    m_shape = arg;
    connect(m_shape, SIGNAL(pathChanged()), this, SLOT(updateOutline()));
    if (!m_shape->path().isEmpty())
        updateOutline();
    emit shapeChanged();
}
