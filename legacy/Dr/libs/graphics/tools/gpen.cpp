#include "gpen.h"

GPen::GPen()
{
    create();
}

void GPen::create() // new pen
{
    d = new GPenData;
}

void GPen::create(QPen pen) // new handle with copied pen
{
    d = new GPenData;
    d->pen = pen;
}

void GPen::create(GPen penHandle) // new handle with pen ref
{
    d = penHandle.sharedDataPointer();
}

void GPen::copy(GPen penHandle) // new handle
{
    d = new GPenData;
    d->pen = penHandle.pen();
}

const QExplicitlySharedDataPointer<GPenData> &GPen::sharedDataPointer() const
{
    return d;
}

QPen GPen::pen() const
{
    return d == nullptr ? QPen() : d->pen;
}

void GPen::setPen(const QPen &pen) {
    d->pen = pen;
}

qreal GPen::width() const
{
    return d->pen.widthF();
}

QColor GPen::color() const
{
    return d->pen.color();
}

bool GPen::cosmetic() const
{
    return d->pen.isCosmetic();
}

Qt::PenStyle GPen::strokeStyle() const
{
    return d->pen.style();
}

Qt::PenJoinStyle GPen::joinStyle() const
{
    return d->pen.joinStyle();
}

Qt::PenCapStyle GPen::capStyle() const
{
    return d->pen.capStyle();
}

qreal GPen::dashOffset() const
{
    return d->pen.dashOffset();
}

void GPen::setWidth(qreal width)
{
    if (d->pen.widthF() != width)
        d->pen.setWidthF( width );
}

void GPen::setColor(QColor color)
{
    if (d->pen.color() != color)
        d->pen.setColor( color );
}

void GPen::setCosmetic(bool cosmetic)
{
    if (d->pen.isCosmetic() != cosmetic)
        d->pen.setCosmetic( cosmetic );
}

void GPen::setStrokeStyle(Qt::PenStyle strokeStyle)
{
    if (d->pen.style() != strokeStyle)
        d->pen.setStyle( strokeStyle );
}

void GPen::setJoinStyle(Qt::PenJoinStyle joinStyle)
{
    if (d->pen.joinStyle() != joinStyle)
        d->pen.setJoinStyle( joinStyle );
}

void GPen::setCapStyle(Qt::PenCapStyle capStyle)
{
    if (d->pen.capStyle() != capStyle)
        d->pen.setCapStyle( capStyle );
}

void GPen::setDashOffset(qreal dashOffset)
{
    if (d->pen.dashOffset() == dashOffset)
        d->pen.setDashOffset( dashOffset );
}

