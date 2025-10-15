#ifndef LINESTROKE_H
#define LINESTROKE_H

#include "stylenode.h"
#include "stroketriangulator.h"
#include <qcolor.h>
class LineStroke;
class LineStrokeData : public StyleNodeData
{
public:
    QColor color;
    StrokeTriangulator triangulator;
};

class LineStroke : public StyleNode
{
    Q_GADGET
    DECLARE_DATA(LineStroke)

    Q_PROPERTY(qreal width READ width WRITE setWidth)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset)
    Q_PROPERTY(QColor color READ color WRITE setColor)

    VertexBuffer m_buffer;

public:
    LineStroke() : StyleNode(new LineStrokeData) {}

    qreal width() const;
    qreal offset() const;
    QColor color() const;

    void setWidth(qreal width);
    void setOffset(qreal offset);
    void setColor(QColor color);

protected:
    void initBuffer(const VertexBuffer &vb);
    QSGNode *updateStyleNode(QSGNode *node, const VertexBuffer &);

};
Q_DECLARE_METATYPE(LineStroke)


#endif // LINESTROKE_H
