#include "polygonsmoother.h"
#include <QDebug>

PolygonSmoother::PolygonSmoother(QObject *parent) : QObject(parent)
{

}

QPolygonF PolygonSmoother::input() const
{
    return m_input;
}

qreal PolygonSmoother::radius() const
{
    return m_radius;
}

QPolygonF PolygonSmoother::output() const
{
    return m_output;
}

void PolygonSmoother::update()
{
    if (chain.isEmpty())
        return;
    GuidePath tmp = chain;
    if (m_offset == 0 && m_radius == 0)
        m_output = m_input;
    else {
        if (m_radius > 0) {
            tmp = tmp.smoothed(m_radius, m_precision);
            m_axis = tmp.toPolygon();
        } else {
            m_axis = chain.toPolygon();
        }
        if (m_offset > 0)
            tmp = tmp.offseted(m_offset);
        m_output = tmp.toPolygon();
    }
    emit axisChanged(m_axis);
    emit outputChanged(m_output);
}

void PolygonSmoother::setInput(QPolygonF input)
{
    if (m_input == input)
        return;
    chain.setPolygon(input);
    m_input = input;
    emit inputChanged(input);
    update();

}

void PolygonSmoother::setRadius(qreal radius)
{
    if (m_radius == radius)
        return;
    m_radius = radius;
    emit radiusChanged(radius);
    update();

}

qreal PolygonSmoother::offset() const
{
    return m_offset;
}

void PolygonSmoother::setOffset(qreal offset)
{
    if (m_offset == offset)
        return;

    m_offset = offset;
    emit offsetChanged(offset);
    update();
}

qreal PolygonSmoother::precision() const
{
    return m_precision;
}

void PolygonSmoother::setPrecision(qreal precision)
{
    if (m_precision == precision)
        return;

    m_precision = precision;
    emit precisionChanged(precision);
    update();
}

