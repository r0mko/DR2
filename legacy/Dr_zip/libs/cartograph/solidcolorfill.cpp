#include "solidcolorfill.h"

#include <qsgflatcolormaterial.h>

QSGNode *SolidColorFill::updateStyleNode(QSGNode *node, const VertexBuffer &buffer)
{
    QSGGeometryNode *n = getGeometryNode(node);
    QSGFlatColorMaterial *m = getMaterial<QSGFlatColorMaterial>(n);
    if (m->color() != _df()->color) {
        m->setColor(_df()->color);
//        qDebug() << "Update fill color" << _df()->color;
    }
    if (!geometry())
        setGeometry(buffer.createFillGeometry());
    else
        buffer.updateFillGeometry(geometry());
    if (n->geometry() != geometry())
        n->setGeometry(geometry());
    return n;
}

QColor SolidColorFill::color() const {
    return _df()->color;
}

void SolidColorFill::setColor(QColor color) {
    _df()->color = color;
}

QVariant SolidColorFill::get()
{
    return QVariant::fromValue(*this);
}
