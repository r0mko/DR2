#ifndef CUSTOMTRANSFORM_H
#define CUSTOMTRANSFORM_H

#include <QObject>
#include <QTransform>
#include <QQuickTransform>
class CustomTransform : public QQuickTransform
{
    Q_OBJECT
    QTransform t;
    Q_PROPERTY(qreal m11 READ m11 WRITE setM11 NOTIFY transformChanged)
    Q_PROPERTY(qreal m12 READ m12 WRITE setM12 NOTIFY transformChanged)
    Q_PROPERTY(qreal m13 READ m13 WRITE setM13 NOTIFY transformChanged)

    Q_PROPERTY(qreal m21 READ m21 WRITE setM21 NOTIFY transformChanged)
    Q_PROPERTY(qreal m22 READ m22 WRITE setM22 NOTIFY transformChanged)
    Q_PROPERTY(qreal m23 READ m23 WRITE setM23 NOTIFY transformChanged)

    Q_PROPERTY(qreal m31 READ m31 WRITE setM31 NOTIFY transformChanged)
    Q_PROPERTY(qreal m32 READ m32 WRITE setM32 NOTIFY transformChanged)
    Q_PROPERTY(qreal m33 READ m33 WRITE setM33 NOTIFY transformChanged)

public:

    explicit CustomTransform(QObject *parent = 0);
    qreal m11() const { return t.m11(); }
    qreal m12() const { return t.m12(); }
    qreal m13() const { return t.m13(); }
    qreal m21() const { return t.m21(); }
    qreal m22() const { return t.m22(); }
    qreal m23() const { return t.m23(); }
    qreal m31() const { return t.m31(); }
    qreal m32() const { return t.m32(); }
    qreal m33() const { return t.m33(); }

    Q_INVOKABLE qreal determinant() const { return t.determinant(); }
    Q_INVOKABLE qreal dx() const { return t.dx(); }
    Q_INVOKABLE qreal dy() const { return t.dy(); }
    Q_INVOKABLE bool isAffine() const { return t.isAffine(); }
    Q_INVOKABLE bool isIdentity() const { return t.isIdentity(); }
    Q_INVOKABLE bool isInvertible() const { return t.isInvertible(); }
    Q_INVOKABLE bool isRotating() const { return t.isRotating(); }
    Q_INVOKABLE bool isScaling() const { return t.isScaling(); }
    Q_INVOKABLE bool isTranslating() const { return t.isTranslating(); }
    void applyTo(QMatrix4x4 *matrix) const;

signals:
    void transformChanged();

public slots:
    void rotate(qreal angle, Qt::Axis axis = Qt::ZAxis);
    void rotateRadians(qreal angle, Qt::Axis axis = Qt::ZAxis);
    void scale(qreal sx, qreal sy);
    void shear(qreal sh, qreal sv);
    void reset();
    void setM11(qreal arg);
    void setM12(qreal arg);
    void setM13(qreal arg);
    void setM21(qreal arg);
    void setM22(qreal arg);
    void setM23(qreal arg);
    void setM31(qreal arg);
    void setM32(qreal arg);
    void setM33(qreal arg);
};



#endif // CUSTOMTRANSFORM_H
