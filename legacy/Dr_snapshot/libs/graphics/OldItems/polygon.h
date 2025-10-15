#ifndef POLYGON_H
#define POLYGON_H

#include <QObject>
#include <QVariant>
#include <QPolygonF>
class GraphicsPath;

class GPolygon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool closed READ closed NOTIFY closedChanged)
    Q_PROPERTY(int nodeCount READ nodeCount NOTIFY nodeCountChanged)
    Q_PROPERTY(QRectF boundingRect READ boundingRect NOTIFY boundingRectChanged)
    Q_PROPERTY(QPolygonF polygon READ polygon WRITE setPolygon NOTIFY polygonChanged)

    QPolygonF m_polygon;
    QRectF oldBoundingRect;
    int oldSize;
    bool batch:1;
    bool wasClosed:1;

public:
    explicit GPolygon(QObject *parent = 0);
    bool closed() const;
    int nodeCount() const;
    QRectF boundingRect() const;

    QPolygonF polygon() const;
    void setPolygon(const QPolygonF &polygon);

    Q_INVOKABLE GraphicsPath* toPath() const;
    Q_INVOKABLE qreal length() const;
    Q_INVOKABLE bool containsPoint(const QPointF &point, Qt::FillRule rule);
    Q_INVOKABLE QPointF node(int index);

signals:
    void closedChanged();
    void nodeCountChanged();
    void boundingRectChanged();
    void polygonChanged();

public slots:
    void begin();
    void end();
    void clear();
    void append(const QPointF &pt);
    void append(qreal x, qreal y);
    void insert(int index, const QPointF &pt);
    void removeAt(int index);
    void setPointPosition(int index, const QPointF &pt);
    void setPointPosition(int index, qreal x, qreal y);
    void setXAt(int index, qreal x);
    void setYAt(int index, qreal y);

    void unite(GPolygon *other);
    void substract(GPolygon *other);
    void intersect(GPolygon *other);

    void translate(const QPointF &offset);
    void translate(qreal dx, qreal dy);

    void fromQPolygonF(QVariant pol);

private:
    void startBatch();
    void finishBatch();
};



#endif // POLYGON_H
