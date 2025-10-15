#ifndef GUIDEPATHSMOOTHER_H
#define GUIDEPATHSMOOTHER_H
#include "guidepath.h"

class GuidePathSmoother
{
    Q_GADGET
    Q_PROPERTY(GuidePath source READ source WRITE setSource)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)
    Q_PROPERTY(qreal precision READ precision WRITE setPrecision)

    Q_PROPERTY(qreal startPos READ startPos WRITE setStartPos)
    Q_PROPERTY(qreal endPos READ endPos WRITE setEndPos)

    Q_PROPERTY(GuidePath result READ result)

    GuidePath m_source;
    qreal m_radius = 0;
    qreal m_precision = 1;
    qreal m_startPos = 0;
    qreal m_endPos = 1;

    GuidePath m_result;


public:
    GuidePathSmoother() { }

    GuidePath source() const;
    void setSource(GuidePath source);

    qreal radius() const;
    void setRadius(qreal radius);

    qreal precision() const;
    void setPrecision(qreal precision);

    qreal startPos() const;
    void setStartPos(qreal startPos);

    qreal endPos() const;
    void setEndPos(qreal endPos);

    GuidePath result() const;

    Q_INVOKABLE void update();
};

Q_DECLARE_METATYPE(GuidePathSmoother)

#endif // GUIDEPATHSMOOTHER_H
