#include "pen.h"


Pen::Pen(QObject *parent) : QObject(parent), m_pen(QPen()), m_visible(true)
{
    connect (this, &Pen::widthChanged, this, &Pen::penChanged);
    connect (this, &Pen::colorChanged, this, &Pen::penChanged);
    connect (this, &Pen::cosmeticChanged, this, &Pen::penChanged);
    connect (this, &Pen::strokeStyleChanged, this, &Pen::penChanged);
    connect (this, &Pen::joinStyleChanged, this, &Pen::penChanged);
    connect (this, &Pen::capStyleChanged, this, &Pen::penChanged);
    connect (this, &Pen::dashOffsetChanged, this, &Pen::penChanged);

}

qreal Pen::width() const
{
    return m_pen.widthF();
}

QColor Pen::color() const
{
    return m_pen.color();
}

bool Pen::cosmetic() const
{
    return m_pen.isCosmetic();
}

Qt::PenStyle Pen::strokeStyle() const
{
    return m_pen.style();
}

Qt::PenJoinStyle Pen::joinStyle() const
{
    return m_pen.joinStyle();
}

Qt::PenCapStyle Pen::capStyle() const
{
    return m_pen.capStyle();
}

QPen Pen::pen() const
{
    return m_pen;
}

void Pen::setWidth(qreal arg)
{
    if (m_pen.widthF() != arg) {
        m_pen.setWidthF(arg);
        emit widthChanged(arg);
    }
}

void Pen::setColor(QColor arg)
{
    if (m_pen.color()!= arg) {
        m_pen.setColor(arg);
        emit colorChanged(arg);
    }
}

void Pen::setCosmetic(bool arg)
{
    if (m_pen.isCosmetic() != arg) {
        m_pen.setCosmetic(arg);
        emit cosmeticChanged(arg);
    }
}

void Pen::setStrokeStyle(Qt::PenStyle arg)
{
    if (m_pen.style() != arg) {
        m_pen.setStyle(arg);
        emit strokeStyleChanged(arg);
        if (m_visible != (arg != Qt::NoPen)) {
            m_visible = (arg != Qt::NoPen);
            emit visibleChanged(m_visible);
        }
    }
}

void Pen::setJoinStyle(Qt::PenJoinStyle arg)
{
    if (m_pen.joinStyle() != arg) {
        m_pen.setJoinStyle(arg);
        emit joinStyleChanged(arg);
    }
}

void Pen::setCapStyle(Qt::PenCapStyle arg)
{
    if (m_pen.capStyle() != arg) {
        m_pen.setCapStyle(arg);
        emit capStyleChanged(arg);
    }
}

qreal Pen::dashOffset() const
{
    return m_pen.dashOffset();
}

void Pen::setDashOffset(qreal arg)
{
    if (m_pen.dashOffset() != arg) {
        m_pen.setDashOffset(arg);
        emit dashOffsetChanged(arg);
    }
}

bool Pen::visible() const
{
    return m_visible;
}

