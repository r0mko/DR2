#include "pathsplitter.h"


bool PathSplitter::atLeftEnd() const
{
    return m_atLeftEnd;
}

bool PathSplitter::atRightEnd() const
{
    return m_atRightEnd;
}

VectorPath *PathSplitter::left()
{
    return m_left;
}

VectorPath *PathSplitter::right()
{
    return m_right;
}

void PathSplitter::setLeft(VectorPath *left)
{
    if (ownsLeft && m_left)
        delete m_left;
    ownsLeft = false;
    m_left = left;
    init();
}

void PathSplitter::setRight(VectorPath *right)
{
    if (ownsRight  && m_right)
        delete m_right;
    ownsRight = false;
    m_right = right;
    init();
}


QPointF PathSplitter::position() const
{
    if (m_left && m_right && rightNode())
        return rightNode()->point;
    return QPointF();
}

void PathSplitter::splitPath(const VectorPath &path, qreal splitPos, VectorPath *left, VectorPath *right)
{
    if (ownsLeft && m_left)
        delete m_left;
    if (!left) {
        m_left = new VectorPath;
        ownsLeft = true;
    } else {
        m_left = left;
        ownsLeft = false;
    }
    if (ownsRight && m_right)
        delete m_right;
    if (!right) {
        m_right = new VectorPath;
        ownsRight = true;
    } else {
        m_right = right;
        ownsRight = false;
    }
    VectorPath::Runner r = path.split(splitPos, m_left, m_right);
    cNode.point = r.point();
    cNode = *rightNode();
    cNode.prev = leftNode();
    cNode.next = rightNode();
    cNode.sideLength = rightNode()->sideLength;
    init();
}



void PathSplitter::init()
{
    if (!m_left || !m_right)
        return;
    if (m_left->isNull() || m_right->isNull())
        return;
    if (leftNode()->endPoint() != rightNode()->point) {
        qDebug() << "[ WARNING ] PathSplitter: paths are not connected: endpoints" << leftNode()->endPoint() << rightNode()->point;
    }
    cNode = *rightNode();
    rightNode()->resetPrev();
    leftNode()->resetNext();
    cNode.prev = leftNode();
    cNode.next = rightNode();
    m_left->checkLength();
    m_right->checkLength();

}

void PathSplitter::moveToNext()
{
    Q_ASSERT(rightNode()->sideLength == 0);
    if (m_right->nodes.count() < 2) {
        m_atRightEnd = true;
        return;
    }
    leftNode()->setNext(rightNode());
    rightNode()->setPrev(leftNode());
    m_left->appendNode(m_right->takeFirstNode());
    leftNode()->resetNext();
    rightNode()->resetPrev();
    leftNode()->unitVector = rightNode()->unitVector;
    //    leftNode()->point = rightNode()->point;
}

void PathSplitter::moveToPrev()
{
    Q_ASSERT(leftNode()->sideLength == 0);
    if (m_left->nodes.count() < 2) {
        m_atLeftEnd = true;
        return;
    }
    leftNode()->setNext(rightNode());
    rightNode()->setPrev(leftNode());
    m_right->prependNode(m_left->takeLastNode());
    leftNode()->resetNext();
    rightNode()->resetPrev();
    rightNode()->unitVector = leftNode()->unitVector;
}

PathSplitter::PathSplitter() :
    m_atLeftEnd(false),
    m_atRightEnd(false),
    m_left(nullptr),
    m_right(nullptr),
    ownsLeft(false),
    ownsRight(false)
{
}

PathSplitter::PathSplitter(const VectorPath &path, qreal splitPos) :
    m_atLeftEnd(false),
    m_atRightEnd(false),
    m_left(nullptr),
    m_right(nullptr),
    ownsLeft(true),
    ownsRight(true)
{
    splitPath(path, splitPos);
}

qreal PathSplitter::shiftLeft(qreal len)
{
    if (m_right->nodes.isEmpty() || m_left->nodes.isEmpty())
        return len;
    while (len > 0) {
        qreal dist = leftNode()->sideLength;
        if (len >= dist) {
            len -= dist;
            rightNode()->sideLength += dist;
            rightNode()->point = leftNode()->point;
            leftNode()->sideLength = 0;
            m_left->m_length -= dist;
            m_right->m_length += dist;
            moveToPrev();
            if (m_atLeftEnd)
                break;
        } else {
            rightNode()->sideLength += len;
            rightNode()->point -= (rightNode()->unitVector * len).toPointF();
            leftNode()->sideLength -= len;
            m_left->m_length -= len;
            m_right->m_length += len;
            len = 0;
            m_atLeftEnd = false;
        }
    }
    m_left->checkLength();
    m_right->checkLength();
    return len;

}

qreal PathSplitter::shiftRight(qreal len)
{
    if (m_right->nodes.isEmpty() || m_left->nodes.isEmpty())
        return len;
    while (len > 0) {
        qreal dist = rightNode()->sideLength;
        if (len >= dist) {
            len -= dist;
            rightNode()->point = rightNode()->endPoint();
            rightNode()->sideLength = 0;
            leftNode()->sideLength += dist;
            m_left->m_length += dist;
            m_right->m_length -= dist;
            moveToNext();
            if (m_atRightEnd)
                break;
        } else {
            leftNode()->sideLength += len;
            rightNode()->sideLength -= len;
            rightNode()->point += (rightNode()->unitVector * len).toPointF();
            m_left->m_length += len;
            m_right->m_length -= len;
            m_atLeftEnd = false;
            len = 0;
        }
    }
    m_left->checkLength();
    m_right->checkLength();
    return len;
}


PathSplitter::~PathSplitter()
{
//    qDebug() << "Splitter destroyed";
    if (ownsLeft)
        delete m_left;
    if (ownsRight)
        delete m_right;
}

