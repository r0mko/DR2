#ifndef GRAPHICSPATH_H
#define GRAPHICSPATH_H

#include <QObject>
#include <QPainterPath>
class GraphicsPath : public QObject
{
    Q_OBJECT
    Q_ENUMS(ElementType)

    Q_PROPERTY(QPainterPath path READ path WRITE setPath)


public:
    enum ElementType {
        Invalid = -1,
        MoveToElement = 0,
        LineToElement = 1,
        CurveToElement = 2,
        CurveToDataElement = 3
    };

    explicit GraphicsPath(QObject *parent = 0);

    QPainterPath path() const;

    Q_INVOKABLE QRectF boundingRect() const;
    Q_INVOKABLE QPointF currentPosition() const;

    Q_INVOKABLE int elementCount() const;

    Q_INVOKABLE QPointF begin() const;
    Q_INVOKABLE QPointF end() const;

    Q_INVOKABLE bool contains(const QPointF &pt) const;
    Q_INVOKABLE bool contains(const QRectF &rect) const;
    Q_INVOKABLE bool contains(const GraphicsPath *other) const;

    Q_INVOKABLE bool intersects(const QRectF &rect) const;
    Q_INVOKABLE bool intersects(const GraphicsPath *other) const;

    Q_INVOKABLE QPointF getElementPosition(int elementIndex) const;

    Q_INVOKABLE QPointF pointAtPercent(qreal t) const;
    Q_INVOKABLE qreal slopeAtPercent(qreal t) const;

    Q_INVOKABLE qreal length() const;
    Q_INVOKABLE qreal percentAtLength(qreal t) const;


signals:
    void pathChanged();

public slots:
    void setPath(QPainterPath path);
    void setElementPosition(int elementIndex, qreal x, qreal y);
    void setElementPosition(int elementIndex, QPointF p);
    void substract(GraphicsPath *other);
    void intersect(GraphicsPath *other);
    void unite(GraphicsPath *other);
    void add(GraphicsPath *other);

    void simplify();


protected:
    friend class ShapeSceneItem;
    void setBoundingRectDirty();
    QPainterPath &rpath();
    QPainterPath *pathPtr();

private:
    mutable bool boundingRectDirty;
    mutable QRectF m_boundingRect;
    QPainterPath m_path;
};

Q_DECLARE_METATYPE(QPainterPath)
#endif // GRAPHICSPATH_H
