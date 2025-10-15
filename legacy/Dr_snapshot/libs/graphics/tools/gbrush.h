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
    Q_PROPERTY(QColor color READ color WRITE setColor )
    Q_PROPERTY(Qt::BrushStyle style READ style WRITE setStyle)

    QExplicitlySharedDataPointer<GBrushData> d;
public:
    GBrush();
    void create();
    void create( QBrush brush );
    void create( GBrush brushHandle );
    void copy( GBrush brushHandle );
    const QExplicitlySharedDataPointer<GBrushData>& sharedDataPointer() const;

    QColor color() const;
    void setColor(QColor color);

    Qt::BrushStyle style() const;
    void setStyle(Qt::BrushStyle style);


    QBrush brush() const;
    void setBrush( const QBrush& brush );
    operator QBrush() const;

    inline bool operator ==(const GBrush &other) { return d == other.d; }
};
Q_DECLARE_METATYPE(GBrush)
#endif // GBRUSH_H
