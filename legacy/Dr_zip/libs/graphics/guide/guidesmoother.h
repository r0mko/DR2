#ifndef GuideSmoother_H
#define GuideSmoother_H
#include "guide.h"

class SmoothRunner : public Runner
{
    Q_GADGET
    Q_PROPERTY(qreal smoothRadius READ smoothRadius WRITE setSmoothRadius)

    qreal       currentCurvature = 0;
    qreal       m_smoothRadius = 0;
    qreal       rstep = 0;
    QPointF     m_negPt;
    QPointF     m_posPt;
    Guide   m_posWay;
    Guide   m_negWay;
    Runner  ahead;

public:
    using Runner::Runner;
    void beginSmooth();
    QPointF smoothedPoint() const;
    void advance();
    qreal smoothRadius() const;
    void setSmoothRadius(qreal smoothRadius);
};

class GuideSmoother
{
    Q_GADGET
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)
    Q_PROPERTY(qreal precision READ precision WRITE setPrecision)
    Q_PROPERTY(Guide source READ source WRITE setSource)
    Q_PROPERTY(Guide result READ result)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)


    qreal                   m_radius = 0;
    qreal                   m_precision = 1;
    QVector<Guide::Link>    couplers;
    mutable int             count = 0;
    qreal                   m_scale = 1.0;
    Guide                   m_source;
    Guide                   m_result;
    friend class GuideNet;

public:
    GuideSmoother() { }

    qreal radius() const;
    void setRadius(qreal m_radius);

    qreal precision() const;
    void setPrecision(qreal precision);
    Runner addCoupler(Guide::Link &&coupler, const QPointF &point = QPointF());
    Guide smoothed(Runner *head, qreal scale, qreal threshold = 0.01);

    Guide source() const
    {
        return m_source;
    }

    Guide result() const
    {
        return m_result;
    }

    void setSource(Guide source)
    {
        m_source = source;
        update();
    }

    qreal scale() const
    {
        return m_scale;
    }


    void setScale(qreal scale)
    {
        m_scale = scale;
    }

protected:
    QPointF getSmoothedPoint(Runner *back, qreal range) const;
private:
    void update();
    QLineF getNextPoint(Runner *back, Runner *forward, qreal range, qreal threshold, QPointF *bNegPt = nullptr, QPointF *bPosPt = nullptr) const;

};

Q_DECLARE_METATYPE(GuideSmoother)

#endif // GuideSmoother_H
