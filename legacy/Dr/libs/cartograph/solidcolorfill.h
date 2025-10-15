#ifndef SOLIDCOLORFILL_H
#define SOLIDCOLORFILL_H

#include "stylenode.h"
#include <QColor>
#include <QVariant>
class SolidColorFill;
class QSGNode;
class SolidColorFillData : public StyleNodeData
{
public:
    QColor color;
};

class SolidColorFill : public StyleNode
{
    Q_GADGET
    DECLARE_DATA(SolidColorFill)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    SolidColorFill() : StyleNode(new SolidColorFillData) {}
    QColor color() const;
    void setColor(QColor color);
    QVariant get();

protected:
    QSGNode *updateStyleNode(QSGNode *node, const VertexBuffer &geometry);
};
Q_DECLARE_METATYPE(SolidColorFill)

#endif // SOLIDCOLORFILL_H
