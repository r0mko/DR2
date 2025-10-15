#ifndef GUIDEGRAPH_H
#define GUIDEGRAPH_H

#include "guide.h"

#include <QObject>
#include <qpen.h>

class GuideView;

class Segment
{
    Q_GADGET
    Q_ENUMS(GuideType)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset)
    Q_PROPERTY(Guide source READ source WRITE setSource)
    Q_PROPERTY(Guide offsetted READ offsetted)

    Q_PROPERTY(Guide posResult READ posResult)
    Q_PROPERTY(Guide negResult READ negResult)

    Q_PROPERTY(GuideView* view READ view )
    Q_PROPERTY(uint uid READ uid)

    class SegmentData : public QSharedData
    {
    public:
        qreal               offset = 0;
        Guide               source;
        Guide               offsetted;
        Guide               posResult;
        Guide               negResult;
        GuideView *         view = nullptr;
        uint                uid = -1;

        SegmentData*        prevSegment = nullptr;
        SegmentData*        nextSegment = nullptr;
        GuideNode::NodeType prevLinkType = GuideNode::InvalidNode;
        GuideNode::NodeType nextLinkType = GuideNode::InvalidNode;
    };

    QExplicitlySharedDataPointer<SegmentData> d;
    friend class GuideView;


public:
    enum GuideType {
        SourceGuide,
        OffsettedGuide,
        PosResultGuide,
        NegResultGuide
    };
    Q_ENUM(GuideType)

    Segment();

    qreal offset() const;
    void setOffset(qreal offset);

    Guide source() const;
    void setSource(Guide guide);

    Guide offsetted() const;

    GuideView* view() const;

    uint uid() const;

    Q_INVOKABLE void updateOffset(qreal scale);
    Q_INVOKABLE void updateOutput(qreal spacing, qreal scale, qreal precision, qreal radius);

    Q_INVOKABLE void setView(GuideView *view);
    Q_INVOKABLE void clearLinks();

    operator QVariant() const { return QVariant::fromValue(*this); }
    Guide posResult() const;
    Guide negResult() const;
protected:
    Segment(SegmentData *data) : d(data) {}
};

Q_DECLARE_METATYPE(Segment)

//class Vertex
//{
//    Q_GADGET
//    Q_PROPERTY(Segment previous READ previous WRITE setPrevious)
//    Q_PROPERTY(Segment next READ next WRITE setNext)
//    Q_PROPERTY(VertexType type READ type)
//    Q_PROPERTY(bool linked READ linked WRITE setLinked)

//    Q_ENUMS(VertexType)

//public:
//    enum VertexType {
//        InvalidType,
//        HeadTerminatorType,
//        TailTerminatorType,
//        BoundaryType,
//        JunctionType,
//        ConvergenceType
//    };

//    inline operator QVariant() const { return QVariant::fromValue(*this); }
//    bool operator==(const Vertex &other) const { return d == other.d; }
//    bool operator!=(const Vertex &other) const { return d != other.d; }

//protected:
//    class VertexData : public QSharedData
//    {
//    public:
//        virtual ~VertexData(){}
//        int         type = InvalidType;
//    };

//    QExplicitlySharedDataPointer<VertexData> d;
//};

#endif // GUIDEGRAPH_H
