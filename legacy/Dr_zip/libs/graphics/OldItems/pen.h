#ifndef PEN_H
#define PEN_H

#include <QObject>
#include <QPen>
class Pen : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged STORED false)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged STORED false)
    Q_PROPERTY(bool cosmetic READ cosmetic WRITE setCosmetic NOTIFY cosmeticChanged STORED false)
    Q_PROPERTY(Qt::PenStyle strokeStyle READ strokeStyle WRITE setStrokeStyle NOTIFY strokeStyleChanged STORED false)
    Q_PROPERTY(Qt::PenJoinStyle joinStyle READ joinStyle WRITE setJoinStyle NOTIFY joinStyleChanged STORED false)
    Q_PROPERTY(Qt::PenCapStyle capStyle READ capStyle WRITE setCapStyle NOTIFY capStyleChanged STORED false)
    Q_PROPERTY(qreal dashOffset READ dashOffset WRITE setDashOffset NOTIFY dashOffsetChanged STORED false)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)

public:
    explicit Pen(QObject *parent = 0);
    virtual ~Pen() {}

    qreal width() const;
    QColor color() const;
    bool cosmetic() const;
    Qt::PenStyle strokeStyle() const;
    Qt::PenJoinStyle joinStyle() const;
    Qt::PenCapStyle capStyle() const;
    QPen pen() const;
    qreal dashOffset() const;

    void setWidth(qreal arg);
    void setColor(QColor arg);
    void setCosmetic(bool arg);
    void setStrokeStyle(Qt::PenStyle arg);
    void setJoinStyle(Qt::PenJoinStyle arg);
    void setCapStyle(Qt::PenCapStyle arg);
    void setDashOffset(qreal arg);
    bool visible() const;



public slots:

signals:
    void widthChanged(qreal arg);
    void colorChanged(QColor arg);
    void cosmeticChanged(bool arg);
    void strokeStyleChanged(Qt::PenStyle arg);
    void joinStyleChanged(Qt::PenJoinStyle arg);
    void capStyleChanged(Qt::PenCapStyle arg);
    void dashOffsetChanged(qreal arg);

    void visibleChanged(bool arg);
    void penChanged();

private:
    QPen m_pen;
    bool m_visible;
};

#endif // PEN_H
