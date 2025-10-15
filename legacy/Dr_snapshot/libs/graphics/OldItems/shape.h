#ifndef GSHAPE_H
#define GSHAPE_H

#include <QObject>
#include <qqml.h>
#include "graphicspath.h"

class GPolygon;

class Shape : public GraphicsPath, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(ElementType)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QPointF currentPosition READ currentPosition WRITE moveTo NOTIFY currentPositionChanged)

public:
    explicit Shape(QObject *parent = 0);
    void classBegin() {}
    void componentComplete();

signals:
    void draw();
    void currentPositionChanged();

public slots:

    void closeSubpath();

    void moveTo(const QPointF &p);
    void moveTo(qreal x, qreal y);

    void lineTo(const QPointF &p);
    void lineTo(qreal x, qreal y);

    void arcMoveTo(const QRectF &rect, qreal angle);
    void arcMoveTo(qreal x, qreal y, qreal w, qreal h, qreal angle);

    void arcTo(const QRectF &rect, qreal startAngle, qreal arcLength);
    void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength);

    void cubicTo(const QPointF &ctrlPt1, const QPointF &ctrlPt2, const QPointF &endPt);
    void cubicTo(qreal ctrlPt1x, qreal ctrlPt1y, qreal ctrlPt2x, qreal ctrlPt2y, qreal endPtx, qreal endPty);
    void quadTo(const QPointF &ctrlPt, const QPointF &endPt);
    void quadTo(qreal ctrlPtx, qreal ctrlPty, qreal endPtx, qreal endPty);

    void addRect(const QRectF &rect);
    void addRect(qreal x, qreal y, qreal w, qreal h);
    void addEllipse(const QRectF &rect);
    void addEllipse(qreal x, qreal y, qreal w, qreal h);
    void addEllipse(const QPointF &center, qreal rx, qreal ry);

    void addPolygon(GPolygon *polygon);

    void addText(const QPointF &point, const QFont &f, const QString &text);
    void addText(qreal x, qreal y, const QFont &f, const QString &text);

    void addRoundedRect(const QRectF &rect, qreal xRadius, qreal yRadius, Qt::SizeMode mode = Qt::AbsoluteSize);
    void addRoundedRect(qreal x, qreal y, qreal w, qreal h, qreal xRadius, qreal yRadius, Qt::SizeMode mode = Qt::AbsoluteSize);

    void addRoundRect(const QRectF &rect, int xRnd, int yRnd);
    void addRoundRect(qreal x, qreal y, qreal w, qreal h, int xRnd, int yRnd);
    void addRoundRect(const QRectF &rect, int roundness);
    void addRoundRect(qreal x, qreal y, qreal w, qreal h, int roundness);

    void translate(qreal dx, qreal dy);
    void translate(const QPointF &offset);

};
#endif // GSHAPE_H
