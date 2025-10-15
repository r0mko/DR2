#ifndef ABSTRACTEDGE_H
#define ABSTRACTEDGE_H

class Node;
class AbstractEdge
{
    Node *m_startNode = nullptr;
    Node *m_endNode = nullptr;
public:
    AbstractEdge();
    virtual ~AbstractEdge();
    virtual int typeId() const = 0;
    inline Node *startNode() const { return m_startNode; }
    inline Node *endNode() const { return m_endNode; }
    inline void setStartNode(Node *startNode) { m_startNode = startNode; }
    inline void setEndNode(Node *endNode) { m_endNode = endNode; }
};

#endif // ABSTRACTEDGE_H
