#ifndef SMOOTHWIRE_H
#define SMOOTHWIRE_H
#include "wiringelement.h"
#include "groove.h"

class SmoothWire : public WiringElement
{
    Q_OBJECT
    Q_PROPERTY(Pen* pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(qreal precision READ precision WRITE setPrecision NOTIFY precisionChanged)
    Q_PROPERTY(Groove* guide READ guide WRITE setGuide NOTIFY guideChanged)
    Q_PROPERTY(Groove* result READ result NOTIFY resultChanged)
    Q_PROPERTY(qreal beginMargin READ beginMargin WRITE setBeginMargin NOTIFY beginMarginChanged)
    Q_PROPERTY(qreal endMargin READ endMargin WRITE setEndMargin NOTIFY endMarginChanged)

    Pen* m_pen;
    qreal m_offset;
    qreal m_radius;
    qreal m_precision;
    Groove* m_guide;
    qreal m_beginMargin;
    qreal m_endMargin;
    QPolygonF polygon;
    Groove* m_result;
    qreal effectiveRadius;
    friend class WireTransition;
public:
    SmoothWire(SceneItem *parent = 0);
    ~SmoothWire();

    // WiringElement interface
    Pen* pen() const;
    qreal offset() const;
    qreal radius() const;
    qreal precision() const;
    qreal beginMargin() const;

    qreal endMargin() const;
    Groove* guide() const;

    void setPen(Pen* arg);
    void setOffset(qreal arg);
    void setRadius(qreal arg);
    void setPrecision(qreal arg);
    void setBeginMargin(qreal arg);
    void setEndMargin(qreal arg);
    void setGuide(Groove* arg);
    void setFixedScale(bool arg);

    Groove* result() const;

    const VectorPath &offsetPath() const;
    const VectorPath &roundedPath() const;

signals:
    void penChanged();
    void offsetChanged();
    void radiusChanged();
    void precisionChanged();
    void beginMarginChanged(qreal arg);
    void endMarginChanged(qreal arg);
    void guideChanged();
    void fixedScaleChanged(bool arg);
    void resultChanged();

public slots:

    void appendRib(Groove *guide, qreal inPosition, qreal outPosition, QVariant data = QVariant());
    SmoothWire *split(qreal pos);

protected:
    VectorPath m_roundedPath;
    VectorPath m_offsetPath;

    virtual void build();
    QPainterPath shape() const;
    void paint(QPainter *painter);
    QRectF getBoundingRect() const;


private:



};

#endif // SMOOTHWIRE_H
