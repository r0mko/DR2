#include "gbrush.h"

GBrush::GBrush()
{
    create();
}

void GBrush::create()
{
    d = new GBrushData;
}

void GBrush::create(QBrush brush)
{
    d = new GBrushData;
    d->brush = brush;
}

void GBrush::create(GBrush brushHandle)
{
    d = brushHandle.sharedDataPointer();
}

void GBrush::copy(GBrush brushHandle)
{
    d = new GBrushData;
    d->brush = brushHandle.brush();
}

const QExplicitlySharedDataPointer<GBrushData> &GBrush::sharedDataPointer() const
{
    return d;
}

QBrush GBrush::brush() const
{
    return d == nullptr ? QBrush() : d->brush;
}

void GBrush::setBrush(const QBrush &brush)
{
    d->brush = brush;
}

GBrush::operator QBrush() const { return d->brush; }

QColor GBrush::color() const
{
    return d->brush.color();
}

Qt::BrushStyle GBrush::style() const
{
    return d->brush.style();
}

void GBrush::setColor(QColor color)
{
    if( d->brush.color() != color )
        d->brush.setColor( color );
}

void GBrush::setStyle(Qt::BrushStyle style)
{
    if( d->brush.style() != style )
        d->brush.setStyle( style );
}

