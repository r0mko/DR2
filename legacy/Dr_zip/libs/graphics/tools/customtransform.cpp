#include "customtransform.h"
#include <QMatrix4x4>
CustomTransform::CustomTransform(QObject *parent) :
    QQuickTransform(parent)
{
    connect(this, SIGNAL(transformChanged()), this, SLOT(update()));
}

void CustomTransform::applyTo(QMatrix4x4 *matrix) const
{
    QMatrix4x4 m(t);
    *matrix = m;
//    qDebug() << "matrix" << *matrix;
}
void CustomTransform::rotate(qreal angle, Qt::Axis axis)
{
    t.rotate(angle, axis);
    emit transformChanged();
}

void CustomTransform::rotateRadians(qreal angle, Qt::Axis axis)
{
    t.rotateRadians(angle, axis);
    emit transformChanged();
}

void CustomTransform::scale(qreal sx, qreal sy)
{
    t.scale(sx, sy);
    emit transformChanged();
}

void CustomTransform::shear(qreal sh, qreal sv)
{
    t.shear(sh, sv);
    emit transformChanged();
}

void CustomTransform::reset()
{
    t.reset();
    emit transformChanged();

}

void CustomTransform::setM11(qreal arg)
{
    if (t.m11() == arg)
        return;
    t.setMatrix(arg,    t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), t.m31(), t.m32(), t.m33());
    emit transformChanged();
}

void CustomTransform::setM12(qreal arg)
{
    if (t.m12() == arg)
        return;
    t.setMatrix(t.m11(), arg,   t.m13(), t.m21(), t.m22(), t.m23(), t.m31(), t.m32(), t.m33());
    emit transformChanged();
}

void CustomTransform::setM13(qreal arg)
{
    if (t.m13() == arg)
        return;
    t.setMatrix(t.m11(), t.m12(), arg, t.m21(), t.m22(), t.m23(), t.m31(), t.m32(), t.m33());

    emit transformChanged();
}

void CustomTransform::setM21(qreal arg)
{
    if (t.m21() == arg)
        return;
    t.setMatrix(t.m11(), t.m12(), t.m13(), arg,     t.m22(), t.m23(), t.m31(), t.m32(), t.m33());

    emit transformChanged();
}

void CustomTransform::setM22(qreal arg)
{
    if (t.m21() == arg)
        return;
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), arg,     t.m23(), t.m31(), t.m32(), t.m33());

    emit transformChanged();
}

void CustomTransform::setM23(qreal arg)
{
    if (t.m23() == arg)
        return;
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), arg,     t.m31(), t.m32(), t.m33());

    emit transformChanged();
}

void CustomTransform::setM31(qreal arg)
{
    if (t.m31() == arg)
        return;
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), arg,     t.m32(), t.m33());

    emit transformChanged();
}

void CustomTransform::setM32(qreal arg)
{
    if (t.m32() == arg)
        return;
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), t.m31(), arg,     t.m33());

    emit transformChanged();
}

void CustomTransform::setM33(qreal arg)
{
    if (t.m33() == arg)
        return;
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), t.m31(), t.m32(), arg);

    emit transformChanged();
}
