#include "distort.h"
#include <QDebug>
#include <QPolygonF>
#include <QMatrix4x4>
#include <QTransform>
Distort::Distort(QObject *parent) :
    QQuickTransform(parent)
{
}

void Distort::applyTo(QMatrix4x4 *matrix) const
{
    QQuickItem* p = qobject_cast<QQuickItem*>( parent() );
    QPolygonF src;
    src << QPointF( 0, 0 ) << QPointF( p->width(), 0 ) << QPointF( p->width(), p->height() ) << QPointF( 0, p->height() );
    QPolygonF dst;
    dst << m_p1 << m_p2 << m_p3 << m_p4;
    QTransform m_t;
    QTransform::quadToQuad( src, dst, m_t );
    QMatrix4x4 m( m_t );
    *matrix = m;
//    qDebug() << "matrix" << *matrix;
}

QPointF Distort::p1() const
{
    return m_p1;
}

QPointF Distort::p2() const
{
    return m_p2;
}

QPointF Distort::p3() const
{
    return m_p3;
}

QPointF Distort::p4() const
{
    return m_p4;
}

void Distort::setP1(QPointF arg)
{
    if (m_p1 == arg)
        return;

    m_p1 = arg;
    emit p1Changed(arg);
    update();
}

void Distort::setP2(QPointF arg)
{
    if (m_p2 == arg)
        return;

    m_p2 = arg;
    emit p2Changed(arg);
    update();
}

void Distort::setP3(QPointF arg)
{
    if (m_p3 == arg)
        return;

    m_p3 = arg;
    emit p3Changed(arg);
    update();
}

void Distort::setP4(QPointF arg)
{
    if (m_p4 == arg)
        return;

    m_p4 = arg;
    emit p4Changed(arg);
    update();
}

