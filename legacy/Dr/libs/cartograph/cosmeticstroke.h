#ifndef COSMETICSTROKE_H
#define COSMETICSTROKE_H

#include "stylenode.h"

#include <QColor>
class CosmeticStrokeData : public StyleNodeData
{
public:
    qreal width;
    QColor color;
};

class CosmeticStroke : public StyleNode
{
    Q_GADGET
    DECLARE_DATA(CosmeticStroke)

    Q_PROPERTY(qreal width READ width WRITE setWidth)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    CosmeticStroke() : StyleNode(new CosmeticStrokeData) {}
    qreal width() const;
    QColor color() const;

    void setWidth(qreal width);
    void setColor(QColor color);

protected:
    QSGNode *updateStyleNode(QSGNode *node, const VertexBuffer &buffer);
};
Q_DECLARE_METATYPE(CosmeticStroke)

#endif // COSMETICSTROKE_H
