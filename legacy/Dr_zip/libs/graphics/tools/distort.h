#ifndef DISTORT_H
#define DISTORT_H
#include <QDebug>
#include <QQuickTransform>
class Distort : public QQuickTransform
{
    Q_OBJECT
    Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)
    Q_PROPERTY(QPointF p2 READ p2 WRITE setP2 NOTIFY p2Changed)
    Q_PROPERTY(QPointF p3 READ p3 WRITE setP3 NOTIFY p3Changed)
    Q_PROPERTY(QPointF p4 READ p4 WRITE setP4 NOTIFY p4Changed)

    QPointF m_p1;
    QPointF m_p2;
    QPointF m_p3;
    QPointF m_p4;

public:
    explicit Distort(QObject *parent = 0);
    virtual ~Distort() { qDebug() << "Delete" << this; }
    void applyTo(QMatrix4x4 *matrix) const;

    QPointF p1() const;
    QPointF p2() const;
    QPointF p3() const;
    QPointF p4() const;

signals:
    void p1Changed(QPointF arg);
    void p2Changed(QPointF arg);
    void p3Changed(QPointF arg);
    void p4Changed(QPointF arg);

public slots:
    void setP1(QPointF arg);
    void setP2(QPointF arg);
    void setP3(QPointF arg);
    void setP4(QPointF arg);
};
#endif // DISTORT_H
