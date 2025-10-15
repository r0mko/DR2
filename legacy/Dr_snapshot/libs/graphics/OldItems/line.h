#ifndef LINE_H
#define LINE_H

#include <QObject>
#include <QLineF>
#include <QVector2D>
#include <QVariantMap>
class Line : public QObject
{
    Q_OBJECT
    Q_ENUMS(IntersectionType)
    Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed STORED false)
    Q_PROPERTY(QPointF p2 READ p2 WRITE setP2 NOTIFY p2Changed STORED false)
    Q_PROPERTY(qreal x1 READ x1 WRITE setX1 NOTIFY x1Changed STORED false)
    Q_PROPERTY(qreal y1 READ y1 WRITE setY1 NOTIFY y1Changed STORED false)
    Q_PROPERTY(qreal x2 READ x2 WRITE setX2 NOTIFY x2Changed STORED false)
    Q_PROPERTY(qreal y2 READ y2 WRITE setY2 NOTIFY y2Changed STORED false)
    Q_PROPERTY(qreal length READ length WRITE setLength NOTIFY lengthChanged STORED false)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged STORED false)

    QLineF line;
    QVector2D m_vector;

public:
    enum IntersectionType { NoIntersection, BoundedIntersection, UnboundedIntersection };
    explicit Line(QObject *parent = 0);

    Q_INVOKABLE bool isNull() const { return line.isNull(); }

    QPointF p1() const { return line.p1(); }
    QPointF p2() const { return line.p2(); }
    qreal x1() const { return line.x1(); }
    qreal y1() const { return line.y1(); }
    qreal x2() const { return line.x2(); }
    qreal y2() const { return line.y2(); }
    qreal length() const { return line.length(); }
    qreal angle() const { return line.angle(); }

    Q_INVOKABLE QVector2D vector() const { return QVector2D(line.dx(), line.dy()); }
    Q_INVOKABLE QVector2D normalVector() const { QLineF nv = line.normalVector(); return QVector2D(nv.dx(), nv.dy()); }
    Q_INVOKABLE QVector2D unitVector() const { QLineF uv = line.unitVector(); return QVector2D(uv.dx(), uv.dy()); }

    Q_INVOKABLE QPointF pointAt(qreal t) const { return line.pointAt(t); }
    Q_INVOKABLE qreal angleTo(Line* other) const { return line.angleTo(other->line); }

    Q_INVOKABLE QVariantMap intersection(Line *other) const;

signals:
    void p1Changed();
    void p2Changed();
    void x1Changed();
    void y1Changed();
    void x2Changed();
    void y2Changed();
    void lengthChanged();
    void angleChanged();

public slots:
    void setP1(QPointF arg);
    void setP2(QPointF arg);
    void setX1(qreal arg);
    void setY1(qreal arg);
    void setX2(qreal arg);
    void setY2(qreal arg);
    void setLength(qreal arg);
    void setAngle(qreal arg);

};
#endif // LINE_H
