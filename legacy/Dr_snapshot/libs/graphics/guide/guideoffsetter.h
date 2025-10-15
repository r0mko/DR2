#ifndef GUIDEPATHOFFSETTER_H
#define GUIDEPATHOFFSETTER_H
#include "guide.h"

class Wavefront
{
    Q_GADGET

    friend class Guide;
    friend class GuideOffsetter;

public:
    struct WFNode
    {
        WFNode() {}

        inline float maxOffset() const { return /*currentOffset +*/ vnode.length() / decayRate(); }
        inline float decayRate() const { return leftDr + rightDr; }

        void shrink(float offset);
        void expand(float offset);

        VectorNode vnode;
        float leftDr = 0; // prev-this decay ratio
        float rightDr = 0; // this-next decay ratio
        float initialMaxOffset;
    };
    Wavefront(Guide path = Guide());

    Q_INVOKABLE void setSource(Guide path);

    QPointF sourceOrigin() const;
    QPointF resultOrigin() const;

    Q_INVOKABLE Guide toGuide() const;
    Q_INVOKABLE QPolygonF toPolygon() const;

    Q_INVOKABLE void update(bool trimCusps = true);
//    Q_INVOKABLE void updatePath();

    Q_INVOKABLE qreal offset() const;
    Q_INVOKABLE void setOffset(qreal offset);

private:
    void shrinkAll(float offset);


    float                   m_offset;
    QPointF                 m_sourceOrigin;
    QPointF                 m_resultOrigin;
    QLinkedList<WFNode>           m_source;
    QLinkedList<WFNode>           m_result;

};

//class GuideOffsetter
//{
//    Q_GADGET
//    Q_PROPERTY(Guide source READ source WRITE setSource)
//    Q_PROPERTY(Guide result READ result)
//    Q_PROPERTY(qreal offset READ offset WRITE setOffset)

//    Wavefront f;

//public:
//    GuideOffsetter();

//    Guide source() const;
//    void setSource(Guide source);

//    qreal offset() const;
//    void setOffset(qreal offset);

//    Guide result() const;
//};

//Q_DECLARE_METATYPE(GuideOffsetter)

#endif // GUIDEPATHOFFSETTER_H
