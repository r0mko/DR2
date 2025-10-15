#ifndef GPEN_H
#define GPEN_H

#include <QObject>
#include <QExplicitlySharedDataPointer>
#include <QPen>

struct GPenData: public QSharedData
{
    QPen pen;
};

class GPen
{
    Q_GADGET
    Q_PROPERTY(qreal width READ width WRITE setWidth STORED false)
    Q_PROPERTY(QColor color READ color WRITE setColor STORED false)
    Q_PROPERTY(bool cosmetic READ cosmetic WRITE setCosmetic STORED false)
    Q_PROPERTY(Qt::PenStyle strokeStyle READ strokeStyle WRITE setStrokeStyle STORED false)
    Q_PROPERTY(Qt::PenJoinStyle joinStyle READ joinStyle WRITE setJoinStyle STORED false)
    Q_PROPERTY(Qt::PenCapStyle capStyle READ capStyle WRITE setCapStyle STORED false)
    Q_PROPERTY(qreal dashOffset READ dashOffset WRITE setDashOffset STORED false)
    QExplicitlySharedDataPointer<GPenData> d;
    friend inline bool operator==(const GPen& left, const GPen& right);

public:
    GPen();
    void create();
    void create( QPen pen );
    void create( GPen penHandle );
    void copy( GPen penHandle);
    const QExplicitlySharedDataPointer<GPenData>& sharedDataPointer() const;
    QPen pen() const;
    void setPen( const QPen& pen );


    qreal width() const;
    QColor color() const;
    bool cosmetic() const;
    Qt::PenStyle strokeStyle() const;
    Qt::PenJoinStyle joinStyle() const;
    Qt::PenCapStyle capStyle() const;
    qreal dashOffset() const;
    void setWidth(qreal width);
    void setColor(QColor color);
    void setCosmetic(bool cosmetic);
    void setStrokeStyle(Qt::PenStyle strokeStyle);
    void setJoinStyle(Qt::PenJoinStyle joinStyle);
    void setCapStyle(Qt::PenCapStyle capStyle);
    void setDashOffset(qreal dashOffset);
    operator QPen() const { return d->pen; }
    Q_INVOKABLE inline void clone() { d.detach(); }
};
inline bool operator==(const GPen& left, const GPen& right)
{
    return left.d == right.d;
}

Q_DECLARE_METATYPE(GPen)
#endif // GPEN_H
