#ifndef GADGETPATH_H
#define GADGETPATH_H
#include <QPainterPath>
#include <QObject>
#include <QDebug>

class PainterPath : private QPainterPath
{
    Q_GADGET

public:
    PainterPath() {}

    explicit PainterPath(const QPointF &startPoint):QPainterPath(startPoint) {}
    PainterPath(const QPainterPath &other):QPainterPath(other) {}


    Q_INVOKABLE void deserialize(QByteArray ba) {
        QDataStream d(&ba, QIODevice::ReadOnly);
        d >> *this;
    }
    Q_INVOKABLE QByteArray deserialize() {
        QByteArray ba;
        QDataStream d(&ba, QIODevice::WriteOnly);
        d << *this ;
        return ba;
    }

    Q_INVOKABLE inline void closeSubpath() { QPainterPath::closeSubpath(); }
    Q_INVOKABLE inline void moveTo(const QPointF &p) { QPainterPath::moveTo(p); }
    Q_INVOKABLE inline void moveTo(qreal x, qreal y) { QPainterPath::moveTo(x, y); }
    Q_INVOKABLE inline void lineTo(const QPointF &p) { QPainterPath::lineTo(p); }
    Q_INVOKABLE inline void lineTo(qreal x, qreal y) { QPainterPath::lineTo(x, y); }
    Q_INVOKABLE inline void arcMoveTo(const QRectF &rect, qreal angle) { QPainterPath::arcMoveTo(rect, angle); }
    Q_INVOKABLE inline void arcMoveTo(qreal x, qreal y, qreal w, qreal h, qreal angle) { QPainterPath::arcMoveTo(x, y, w, h, angle); }
    Q_INVOKABLE inline void arcTo(const QRectF &rect, qreal startAngle, qreal arcLength) { QPainterPath::arcTo(rect, startAngle, arcLength); }
    Q_INVOKABLE inline void arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength) { QPainterPath::arcTo(x, y, w, h, startAngle, arcLength); }
    Q_INVOKABLE inline void cubicTo(const QPointF &ctrlPt1, const QPointF &ctrlPt2, const QPointF &endPt) { QPainterPath::cubicTo(ctrlPt1, ctrlPt2, endPt); }
    Q_INVOKABLE inline void cubicTo(qreal ctrlPt1x, qreal ctrlPt1y, qreal ctrlPt2x, qreal ctrlPt2y, qreal endPtx, qreal endPty) { QPainterPath::cubicTo(ctrlPt1x, ctrlPt1y, ctrlPt2x, ctrlPt2y, endPtx, endPty); }
    Q_INVOKABLE inline void quadTo(const QPointF &ctrlPt, const QPointF &endPt) { QPainterPath::quadTo(ctrlPt, endPt); }
    Q_INVOKABLE inline void quadTo(qreal ctrlPtx, qreal ctrlPty, qreal endPtx, qreal endPty) { QPainterPath::quadTo(ctrlPtx, ctrlPty, endPtx, endPty); }
    Q_INVOKABLE inline QPointF currentPosition() const { return QPainterPath::currentPosition(); }
    Q_INVOKABLE inline void addRect(const QRectF &rect) { QPainterPath::addRect(rect); }
    Q_INVOKABLE inline void addRect(qreal x, qreal y, qreal w, qreal h) { QPainterPath::addRect(x, y, w, h); }
    Q_INVOKABLE inline void addEllipse(const QRectF &rect) { QPainterPath::addEllipse(rect); }
    Q_INVOKABLE inline void addEllipse(qreal x, qreal y, qreal w, qreal h) { QPainterPath::addEllipse(x, y, w, h); }
    Q_INVOKABLE inline void addEllipse(const QPointF &center, qreal rx, qreal ry) { QPainterPath::addEllipse(center, rx, ry); }
    Q_INVOKABLE inline void addPolygon(const QPolygonF &polygon) { QPainterPath::addPolygon(polygon); }
    Q_INVOKABLE inline void addText(const QPointF &point, const QFont &f, const QString &text) { QPainterPath::addText(point, f, text); }
    Q_INVOKABLE inline void addText(qreal x, qreal y, const QFont &f, const QString &text) { QPainterPath::addText(x, y, f, text); }
    Q_INVOKABLE inline void addPath(const QPainterPath &path) { QPainterPath::addPath(path); }
    Q_INVOKABLE inline void addRegion(const QRegion &region) { QPainterPath::addRegion(region); }
    Q_INVOKABLE inline void addRoundedRect(const QRectF &rect, qreal xRadius, qreal yRadius, Qt::SizeMode mode = Qt::AbsoluteSize) { QPainterPath::addRoundedRect(rect, xRadius, yRadius, mode); }
    Q_INVOKABLE inline void addRoundedRect(qreal x, qreal y, qreal w, qreal h, qreal xRadius, qreal yRadius, Qt::SizeMode mode = Qt::AbsoluteSize) { QPainterPath::addRoundedRect(x, y, w, h, xRadius, yRadius, mode); }
    Q_INVOKABLE inline void addRoundRect(const QRectF &rect, int xRnd, int yRnd) { QPainterPath::addRoundRect(rect, xRnd, yRnd); }
    Q_INVOKABLE inline void addRoundRect(qreal x, qreal y, qreal w, qreal h, int xRnd, int yRnd) { QPainterPath::addRoundRect(x, y, w, h, xRnd, yRnd); }
    Q_INVOKABLE inline void addRoundRect(const QRectF &rect, int roundness) { QPainterPath::addRoundRect(rect, roundness); }
    Q_INVOKABLE inline void addRoundRect(qreal x, qreal y, qreal w, qreal h, int roundness) { QPainterPath::addRoundRect(x, y, w, h, roundness); }
    Q_INVOKABLE inline void connectPath(const QPainterPath &path) { QPainterPath::connectPath(path); }
    Q_INVOKABLE inline bool contains(const QPointF &pt) const { return QPainterPath::contains(pt); }
    Q_INVOKABLE inline bool contains(const QRectF &rect) const { return QPainterPath::contains(rect); }
    Q_INVOKABLE inline bool intersects(const QRectF &rect) const { return QPainterPath::intersects(rect); }
    Q_INVOKABLE inline void translate(qreal dx, qreal dy) { QPainterPath::translate(dx, dy); }
    Q_INVOKABLE inline void translate(const QPointF &offset) { QPainterPath::translate(offset); }
    Q_INVOKABLE inline QPainterPath translated(qreal dx, qreal dy) const { return QPainterPath::translated(dx, dy); }
    Q_INVOKABLE inline QPainterPath translated(const QPointF &offset) const { return QPainterPath::translated(offset); }
    Q_INVOKABLE inline QRectF boundingRect() const { return QPainterPath::boundingRect(); }
    Q_INVOKABLE inline QRectF controlPointRect() const { return QPainterPath::controlPointRect(); }
    Q_INVOKABLE inline Qt::FillRule fillRule() const { return QPainterPath::fillRule(); }
    Q_INVOKABLE inline void setFillRule(Qt::FillRule fillRule) { QPainterPath::setFillRule(fillRule); }
    Q_INVOKABLE inline bool isEmpty() const { return QPainterPath::isEmpty(); }
    Q_INVOKABLE inline QPainterPath toReversed() const { return QPainterPath::toReversed(); }
    Q_INVOKABLE inline QList<QPolygonF> toSubpathPolygons(const QMatrix &matrix = QMatrix()) const { return QPainterPath::toSubpathPolygons(matrix); }
    Q_INVOKABLE inline QList<QPolygonF> toFillPolygons(const QMatrix &matrix = QMatrix()) const { return QPainterPath::toFillPolygons(matrix); }
    Q_INVOKABLE inline QPolygonF toFillPolygon(const QMatrix &matrix = QMatrix()) const { return QPainterPath::toFillPolygon(matrix); }
    Q_INVOKABLE inline QList<QPolygonF> toSubpathPolygons(const QTransform &matrix) const { return QPainterPath::toSubpathPolygons(matrix); }
    Q_INVOKABLE inline QList<QPolygonF> toFillPolygons(const QTransform &matrix) const { return QPainterPath::toFillPolygons(matrix); }
    Q_INVOKABLE inline QPolygonF toFillPolygon(const QTransform &matrix) const { return QPainterPath::toFillPolygon(matrix); }
    Q_INVOKABLE inline int elementCount() const { return QPainterPath::elementCount(); }
    Q_INVOKABLE inline QPainterPath::Element elementAt(int i) const { return QPainterPath::elementAt(i); }
    Q_INVOKABLE inline void setElementPositionAt(int i, qreal x, qreal y) { QPainterPath::setElementPositionAt(i, x, y); }
    Q_INVOKABLE inline qreal length() const { return QPainterPath::length(); }
    Q_INVOKABLE inline qreal percentAtLength(qreal t) const { return QPainterPath::percentAtLength(t); }
    Q_INVOKABLE inline QPointF pointAtPercent(qreal t) const { return QPainterPath::pointAtPercent(t); }
    Q_INVOKABLE inline qreal angleAtPercent(qreal t) const { return QPainterPath::angleAtPercent(t); }
    Q_INVOKABLE inline qreal slopeAtPercent(qreal t) const { return QPainterPath::slopeAtPercent(t); }
    Q_INVOKABLE inline bool intersects(const QPainterPath &p) const { return QPainterPath::intersects(p); }
    Q_INVOKABLE inline bool contains(const QPainterPath &p) const { return QPainterPath::contains(p); }
    Q_INVOKABLE inline QPainterPath united(const QPainterPath &r) const { return QPainterPath::united(r); }
    Q_INVOKABLE inline QPainterPath intersected(const QPainterPath &r) const { return QPainterPath::intersected(r); }
    Q_INVOKABLE inline QPainterPath subtracted(const QPainterPath &r) const { return QPainterPath::subtracted(r); }
    Q_INVOKABLE inline QPainterPath subtractedInverted(const QPainterPath &r) const { return QPainterPath::subtractedInverted(r); }
    Q_INVOKABLE inline QPainterPath simplified() const { return QPainterPath::simplified(); }

};


Q_DECLARE_METATYPE(PainterPath)

#endif // GADGETPATH_H
