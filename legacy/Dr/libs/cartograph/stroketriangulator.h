#ifndef STROKETRIANGULATOR_H
#define STROKETRIANGULATOR_H
#include "defs.h"
#define CURVE_THRESHOLD 10
/**
 * @brief Триангулятор линий
 */
class StrokeTriangulator
{
    Q_GADGET
    Q_PROPERTY(qreal width READ width WRITE setWidth)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset)
    Q_PROPERTY(Qt::PenJoinStyle joinStyle READ joinStyle WRITE setJoinStyle)
    Q_PROPERTY(Qt::PenCapStyle capStyle READ capStyle WRITE setCapStyle)

public:
    StrokeTriangulator();
    StrokeTriangulator(VertexBuffer *vertexBuffer, qreal width, qreal offset, Qt::PenJoinStyle joinStyle = Qt::BevelJoin);
    void triangulate(const QPolygonF &polygon, qreal lod = 1.0);

    inline qreal width() const { return m_width; }
    void setWidth(const qreal &width);

    inline qreal offset() const { return m_offset; }
    void setOffset(const qreal &offset);

    inline Qt::PenJoinStyle joinStyle() const { return m_joinStyle; }
    void setJoinStyle(const Qt::PenJoinStyle &joinStyle);

    inline Qt::PenCapStyle capStyle() const { return m_capStyle; }
    void setCapStyle(const Qt::PenCapStyle &capStyle);

    QVector<QPointF> vertices() const;
    QVector<uint> indices() const;

    VertexBuffer *vertexBuffer() const;
    void setVertexBuffer(VertexBuffer *vertexBuffer);

private:
    QPointF emitRoundJoin(const QPointF &center, qreal radius, bool leftTurn, qreal startAngle, qreal endAngle, const QPointF &junction, qreal lod);

    QPair<QPolygonF, QPolygonF> getRounding(const QPointF &center, qreal radius, bool leftTurn, qreal startAngle, qreal endAngle, qreal lod);
    QPair<QPolygonF, QPolygonF> _getOffsetVertices(const QPolygonF polygon, qreal lod);

    void emitRoundCap(const QPointF &center, qreal angle, bool end, qreal lod);
    void emitSquareCap(const QPointF &left, const QPointF &right, qreal angle);
    VertexBuffer *m_vertexBuffer;
    Qt::PenCapStyle m_capStyle;
    Qt::PenJoinStyle m_joinStyle;
    qreal m_widthLeft;
    qreal m_widthRight;
    qreal m_offset;
    qreal m_width;
};

#endif // STROKETRIANGULATOR_H
