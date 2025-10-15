#include "shape.h"
#include "polygon.h"

Shape::Shape(QObject *parent) :
    GraphicsPath(parent)
{
}

void Shape::componentComplete()
{
    emit draw();
}

void Shape::closeSubpath()
{

    rpath().closeSubpath();
}

void Shape::moveTo(const QPointF &p)
{
    setBoundingRectDirty();
    rpath().moveTo(p);
    emit currentPositionChanged();
}

void Shape::moveTo(qreal x, qreal y)
{
    setBoundingRectDirty();
    rpath().moveTo(x, y);
    emit currentPositionChanged();
}

void Shape::lineTo(const QPointF &p)
{
    setBoundingRectDirty();
    rpath().lineTo(p);
    emit currentPositionChanged();
}

void Shape::lineTo(qreal x, qreal y)
{
    setBoundingRectDirty();
    rpath().lineTo(x, y);
    emit currentPositionChanged();
}

void Shape::arcMoveTo(const QRectF &rect, qreal angle)
{
    setBoundingRectDirty();
    rpath().arcMoveTo(rect, angle);
    emit currentPositionChanged();
}

void Shape::arcMoveTo(qreal x, qreal y, qreal w, qreal h, qreal angle)
{
    rpath().arcMoveTo(x, y, w, h, angle);
    emit currentPositionChanged();
}

void Shape::arcTo(const QRectF &rect, qreal startAngle, qreal arcLength)
{
    setBoundingRectDirty();
    rpath().arcTo(rect, startAngle, arcLength);
    emit currentPositionChanged();
}

void Shape::arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength)
{
    setBoundingRectDirty();
    rpath().arcTo(x, y, w, h, startAngle, arcLength);
    emit currentPositionChanged();
}

void Shape::cubicTo(const QPointF &ctrlPt1, const QPointF &ctrlPt2, const QPointF &endPt)
{
    setBoundingRectDirty();
    rpath().cubicTo(ctrlPt1,ctrlPt2,endPt);
    emit currentPositionChanged();
}

void Shape::cubicTo(qreal ctrlPt1x, qreal ctrlPt1y, qreal ctrlPt2x, qreal ctrlPt2y, qreal endPtx, qreal endPty)
{
    setBoundingRectDirty();
    rpath().cubicTo(ctrlPt1x, ctrlPt1y, ctrlPt2x, ctrlPt2y, endPtx, endPty);
    emit currentPositionChanged();
}

void Shape::quadTo(const QPointF &ctrlPt, const QPointF &endPt)
{
    setBoundingRectDirty();
    rpath().quadTo(ctrlPt,endPt);
    emit currentPositionChanged();
}

void Shape::quadTo(qreal ctrlPtx, qreal ctrlPty, qreal endPtx, qreal endPty)
{
    setBoundingRectDirty();
    rpath().quadTo(ctrlPtx, ctrlPty, endPtx, endPty);
    emit currentPositionChanged();
}

void Shape::addRect(const QRectF &rect)
{
    setBoundingRectDirty();
    rpath().addRect(rect);
    emit currentPositionChanged();
}

void Shape::addRect(qreal x, qreal y, qreal w, qreal h)
{
    setBoundingRectDirty();
    rpath().addRect(x, y, w, h);
    emit currentPositionChanged();
}

void Shape::addEllipse(const QRectF &rect)
{
    setBoundingRectDirty();
    rpath().addEllipse(rect);
    emit currentPositionChanged();
}

void Shape::addEllipse(qreal x, qreal y, qreal w, qreal h)
{
    setBoundingRectDirty();
    rpath().addEllipse(x, y, w, h);
    emit currentPositionChanged();
}

void Shape::addEllipse(const QPointF &center, qreal rx, qreal ry)
{
    setBoundingRectDirty();
    rpath().addEllipse(center, rx, ry);
    emit currentPositionChanged();
}

void Shape::addPolygon(GPolygon *polygon)
{
    setBoundingRectDirty();
    rpath().addPolygon(polygon->polygon());
}


void Shape::addText(const QPointF &point, const QFont &f, const QString &text)
{
    setBoundingRectDirty();
    rpath().addText(point,f,text);
    emit currentPositionChanged();
}

void Shape::addText(qreal x, qreal y, const QFont &f, const QString &text)
{
    setBoundingRectDirty();
    rpath().addText(x, y,f,text);
    emit currentPositionChanged();
}

void Shape::addRoundedRect(const QRectF &rect, qreal xRadius, qreal yRadius, Qt::SizeMode mode)
{
    setBoundingRectDirty();
    rpath().addRoundedRect(rect, xRadius, yRadius, mode);
    emit currentPositionChanged();
}

void Shape::addRoundedRect(qreal x, qreal y, qreal w, qreal h, qreal xRadius, qreal yRadius, Qt::SizeMode mode)
{
    setBoundingRectDirty();
    rpath().addRoundedRect(x, y, w, h, xRadius, yRadius, mode);
    emit currentPositionChanged();
}

void Shape::addRoundRect(const QRectF &rect, int xRnd, int yRnd)
{
    setBoundingRectDirty();
    rpath().addRoundRect(rect, xRnd, yRnd);
    emit currentPositionChanged();
}

void Shape::addRoundRect(qreal x, qreal y, qreal w, qreal h, int xRnd, int yRnd)
{
    setBoundingRectDirty();
    rpath().addRoundRect(x, y, w, h, xRnd, yRnd);
    emit currentPositionChanged();
}

void Shape::addRoundRect(const QRectF &rect, int roundness)
{
    setBoundingRectDirty();
    rpath().addRoundRect(rect, roundness);
    emit currentPositionChanged();
}

void Shape::addRoundRect(qreal x, qreal y, qreal w, qreal h, int roundness)
{
    setBoundingRectDirty();
    rpath().addRoundRect(x, y, w, h, roundness);
    emit currentPositionChanged();
}

void Shape::translate(qreal dx, qreal dy)
{
    setBoundingRectDirty();
    rpath().translate(dx, dy);
}

void Shape::translate(const QPointF &offset)
{
    setBoundingRectDirty();
    rpath().translate(offset);
}

