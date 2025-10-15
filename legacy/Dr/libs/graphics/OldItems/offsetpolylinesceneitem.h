#ifndef OFFSETPOLYLINESCENEITEM_H
#define OFFSETPOLYLINESCENEITEM_H
#include "shapesceneitem.h"
#include <QtMath>
#include <QVariant>
#include "pathguide.h"

class OffsetPolylineSceneItem : public ShapeSceneItem
{
    Q_OBJECT
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QPolygonF polygon READ polygon WRITE setPolygon NOTIFY polygonChanged)
    Q_PROPERTY(bool simple READ simple WRITE setSimple NOTIFY simpleChanged)
    Q_PROPERTY(int precision READ precision WRITE setPrecision NOTIFY precisionChanged)

    qreal m_offset;
    qreal m_radius;
    bool m_simple;
    int m_precision;

public:
    explicit OffsetPolylineSceneItem(ShapeSceneItem *parent = 0);

    qreal offset() const;
    qreal radius() const;
    QPolygonF polygon() const;
    bool simple() const;
    int precision() const;

    void setOffset(qreal arg);
    void setRadius(qreal arg);
    void setPolygon(QPolygonF arg);
    void setSimple(bool arg);
    void setPrecision(int arg);

signals:
    void offsetChanged(qreal arg);
    void radiusChanged(qreal arg);
    void polygonChanged(QPolygonF arg);
    void simpleChanged(bool arg);
    void precisionChanged(int arg);

public slots:
    void initPolygon(QVariant pol);
    void initPointList(QVariant pol);

private:
    void updateWire();
    void updatePath();
};

#endif // OFFSETPOLYLINESCENEITEM_H
