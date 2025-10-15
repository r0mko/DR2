#ifndef PATHSPLITTER_H
#define PATHSPLITTER_H
#include "vectorpath.h"

class PathSplitter
{
    VectorPath *m_left;
    VectorPath *m_right;
    VectorPath::VNode cNode;
    bool m_atLeftEnd;
    bool m_atRightEnd;
    bool ownsLeft:1;
    bool ownsRight:1;

public:
    PathSplitter();
    PathSplitter(const VectorPath &path, qreal splitPos);
    ~PathSplitter();

    qreal shiftLeft(qreal len);
    qreal shiftRight(qreal len);

    bool atLeftEnd() const;
    bool atRightEnd() const;

    VectorPath *left();
    VectorPath *right();

    void setLeft(VectorPath *left);
    void setRight(VectorPath *right);

    QPointF position() const;

    void splitPath(const VectorPath &path, qreal splitPos, VectorPath *left = nullptr, VectorPath *right = nullptr);

private:
    inline VectorPath::VNode *leftNode();
    inline VectorPath::VNode *rightNode();
    inline const VectorPath::VNode *leftNode() const;
    inline const VectorPath::VNode *rightNode() const;
    void init();
    void moveToNext();
    void moveToPrev();
};

VectorPath::VNode *PathSplitter::leftNode()
{
    if (!m_left->nodes.isEmpty())
        return &m_left->nodes.last();
    return nullptr;
}

VectorPath::VNode *PathSplitter::rightNode()
{
    if (!m_right->nodes.isEmpty())
        return &m_right->nodes.first();
    return nullptr;
}

const VectorPath::VNode *PathSplitter::leftNode() const
{
    if (!m_left->nodes.isEmpty())
        return &m_left->nodes.last();
    return nullptr;
}

const VectorPath::VNode *PathSplitter::rightNode() const
{
    if (!m_right->nodes.isEmpty())
        return &m_right->nodes.first();
    return nullptr;
}

#endif // PATHSPLITTER_H
