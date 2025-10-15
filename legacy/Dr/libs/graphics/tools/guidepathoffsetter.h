<<<<<<< .mine
#ifndef GUIDEPATHOFFSETTER_H
#define GUIDEPATHOFFSETTER_H
#include "guidepath.h"

class GuidePathOffsetter
{
public:
    class Wavefront
    {
        friend class GuidePath;
        friend class GuidePathOffsetter;
    public:
        struct node
        {
            node() {}
            node(const VectorNode *vectorNode, const VectorNode *nextVectorNode);
            qreal maxOffset(const node &prevNode) const;
            QVector2D bsuv;  //bisect unit vector
            VectorNode vnode;

        };

    private:
        QLinkedList<node> nodes;
    };

    GuidePathOffsetter();
};

#endif // GUIDEPATHOFFSETTER_H
=======
#ifndef GUIDEPATHOFFSETTER_H
#define GUIDEPATHOFFSETTER_H
#include "guidepath.h"

class Wavefront
{
    friend class GuidePath;
    friend class GuidePathOffsetter;
public:
    struct WFNode
    {
        WFNode() {}
//        void setDecayRate(const WFNode &next)
//        {
//            leftDecay = QVector2D::dotProduct(bsuv, QVector2D(vnode.unitVector()));
//            rightDecay = QVector2D::dotProduct(next.bsuv, -QVector2D(vnode.unitVector()));
//        }

        inline float maxOffset() const { return vnode.length() / decayRate; }
        VectorNode vnode;
        float decayRate = 0;
    };

    Wavefront(const GuidePath &path);

private:
    QVector2D unitBisector(const VectorNode *vectorNode, const VectorNode *prevVectorNode) const;

    QLinkedList<WFNode>           nodes;
    QMultiMap<float, QLinkedListNode<WFNode>*> offsetMap;
//        QLinkedListNode<node>
    QPointF                     m_sourcePoint;
};



class GuidePathOffsetter
{
public:
    GuidePathOffsetter();
};

#endif // GUIDEPATHOFFSETTER_H
>>>>>>> .r2725
