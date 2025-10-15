#ifndef GBRUSH_H
#define GBRUSH_H

#include <QObject>
#include <QExplicitlySharedDataPointer>
#include <QBrush>

struct GBrushData: public QSharedData
{
    QBrush brush;
};


class GBrush
{
    Q_GADGET
    QExplicitlySharedDataPointer<GBrushData> d;
public:
    GBrush();
    void create();
    void create( QBrush brush );
    void create( GBrush brushHandle );
    void copy( GBrush brushHandle );
    const QExplicitlySharedDataPointer<GBrushData>& sharedDataPointer() const;
    QBrush brush() const;
    void setBrush( const QBrush& brush );
    operator QBrush() const { return d->brush; }
private:
    Q_PROPERTY(QColor color READ color WRITE setColor )
    Q_PROPERTY(Qt::BrushStyle style READ style WRITE setStyle)

public:

    QColor color() const
    {
        return d->brush.color();
    }
    Qt::BrushStyle style() const
    {
        return d->brush.style();
    }

    void setColor(QColor color)
    {
        if( d->brush.color() != color )
            d->brush.setColor( color );
    }
    void setStyle(Qt::BrushStyle style)
    {
        if( d->brush.style() != style )
            d->brush.setStyle( style );
    }
};
Q_DECLARE_METATYPE(GBrush)
#endif // GBRUSH_H
