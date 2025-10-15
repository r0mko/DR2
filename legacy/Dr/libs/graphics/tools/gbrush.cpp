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

