#include "style.h"


qreal Style::opacity() const
{
    return _df()->opacity;
}

void Style::setOpacity(qreal opacity)
{
    _df()->opacity = opacity;
}


QSGNode *Style::updateStyleNode(QSGNode *node, const VertexBuffer &geometry)
{
    QSGOpacityNode *n = getOpacityNode(node);
    if( _df()->opacity != n->opacity() ) {
        qDebug() << "Update style opacity" << _df()->opacity;
        n->setOpacity(_df()->opacity);
    }
    return n;
}
