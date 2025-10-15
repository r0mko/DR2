#include "brush.h"

Brush::Brush(QObject *parent) : QObject(parent), m_brush(QBrush(Qt::SolidPattern))
{
    connect (this, &Brush::colorChanged, this, &Brush::brushChanged);
    connect (this, &Brush::styleChanged, this, &Brush::brushChanged);
}

QBrush Brush::brush() const
{
    return m_brush;
}

QColor Brush::color() const
{
    return m_brush.color();
}

Qt::BrushStyle Brush::style() const
{
    return m_brush.style();
}

void Brush::setColor(QColor arg)
{
    if (m_brush.color() != arg) {
        m_brush.setColor(arg);
        emit colorChanged(arg);
    }
}

void Brush::setStyle(Qt::BrushStyle arg)
{
    if (m_brush.style() != arg) {
        m_brush.setStyle(arg);
        emit styleChanged(arg);
        if (m_visible != (arg != Qt::NoBrush)) {
            m_visible = arg != Qt::NoBrush;
            emit visibleChanged(m_visible);
        }
    }
}

bool Brush::visible() const
{
    return m_visible;
}
