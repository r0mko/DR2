#ifndef POLYGONSMOOTHER_H
#define POLYGONSMOOTHER_H

#include <QObject>
#include "guidepath.h"
class PolygonSmoother : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPolygonF input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal precision READ precision WRITE setPrecision NOTIFY precisionChanged)



    Q_PROPERTY(QPolygonF axis READ axis NOTIFY axisChanged)
    Q_PROPERTY(QPolygonF output READ output NOTIFY outputChanged)

    GuidePath chain;
    QPolygonF m_input;
    qreal m_radius = 0;
    qreal m_offset = 0;
    qreal m_precision = 2;
    QPolygonF m_output;

    QPolygonF m_axis;


public:
    explicit PolygonSmoother(QObject *parent = 0);

    QPolygonF input() const;
    void setInput(QPolygonF input);

    qreal radius() const;
    void setRadius(qreal radius);

    qreal offset() const;
    void setOffset(qreal offset);

    qreal precision() const;
    void setPrecision(qreal precision);

    QPolygonF output() const;
    QPolygonF axis() const
    {
        return m_axis;
    }

signals:
    void inputChanged(QPolygonF input);
    void radiusChanged(qreal radius);
    void outputChanged(QPolygonF output);
    void offsetChanged(qreal offset);
    void axisChanged(QPolygonF axis);

    void precisionChanged(qreal precision);

public slots:
    void update();

};

#endif // POLYGONSMOOTHER_H
