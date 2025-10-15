#ifndef STYLE_H
#define STYLE_H

#include "stylenode.h"
#include <QVariant>

class StyleData : public StyleNodeData
{
public:
    qreal opacity = 0.8;
};

class Style : public StyleNode
{
    Q_GADGET
    DECLARE_DATA(Style)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    Style(): StyleNode(new StyleData) {}
    qreal opacity() const;
    void setOpacity(qreal opacity);

protected:
    QSGNode *updateStyleNode(QSGNode *node, const VertexBuffer &geometry);
};

Q_DECLARE_METATYPE(Style)

#endif // STYLE_H
