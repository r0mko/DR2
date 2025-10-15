#ifndef DGSHAPE_H
#define DGSHAPE_H

#include <QObject>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QQmlParserStatus>
#include <QDebug>


class DGPen : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged STORED false)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged STORED false)
    Q_PROPERTY(bool cosmetic READ cosmetic WRITE setCosmetic NOTIFY cosmeticChanged STORED false)
    Q_PROPERTY(Qt::PenStyle strokeStyle READ strokeStyle WRITE setStrokeStyle NOTIFY strokeStyleChanged STORED false)
    Q_PROPERTY(Qt::PenJoinStyle joinStyle READ joinStyle WRITE setJoinStyle NOTIFY joinStyleChanged STORED false)
    Q_PROPERTY(Qt::PenCapStyle capStyle READ capStyle WRITE setCapStyle NOTIFY capStyleChanged STORED false)
    Q_PROPERTY(qreal dashOffset READ dashOffset WRITE setDashOffset NOTIFY dashOffsetChanged STORED false)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)

public:
    explicit DGPen(QObject *parent = 0) : QObject(parent), m_pen(QPen()), m_visible(true) {}
    virtual ~DGPen() {}

    qreal width() const;
    QColor color() const;
    bool cosmetic() const;
    Qt::PenStyle strokeStyle() const;
    Qt::PenJoinStyle joinStyle() const;
    Qt::PenCapStyle capStyle() const;
    QPen pen() const;
    qreal dashOffset() const;

    void setWidth(qreal arg);
    void setColor(QColor arg);
    void setCosmetic(bool arg);
    void setStrokeStyle(Qt::PenStyle arg);
    void setJoinStyle(Qt::PenJoinStyle arg);
    void setCapStyle(Qt::PenCapStyle arg);
    void setDashOffset(qreal arg);
    bool visible() const;

public slots:

signals:
    void widthChanged(qreal arg);
    void colorChanged(QColor arg);
    void cosmeticChanged(bool arg);
    void strokeStyleChanged(Qt::PenStyle arg);
    void joinStyleChanged(Qt::PenJoinStyle arg);
    void capStyleChanged(Qt::PenCapStyle arg);
    void dashOffsetChanged(qreal arg);

    void visibleChanged(bool arg);

private:
    QPen m_pen;
    bool m_visible;
};

class DGBrush : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(Qt::BrushStyle style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)

public:
    explicit DGBrush(QObject *parent = 0) : QObject(parent), m_brush(QBrush()) {}
    virtual ~DGBrush() {}
    QBrush brush() const;
    QColor color() const;
    Qt::BrushStyle style() const;

    void setColor(QColor arg);
    void setStyle(Qt::BrushStyle arg);

    bool visible() const;

public slots:

signals:
    void colorChanged(QColor arg);
    void styleChanged(Qt::BrushStyle arg);

    void visibleChanged(bool arg);

private:
    QBrush m_brush;
    bool m_visible;
};


class DGShape : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_ENUMS(ElementType)
    Q_PROPERTY(DGPen* pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(DGBrush* brush READ brush WRITE setBrush NOTIFY brushChanged)
    Q_PROPERTY(int elementCount READ elementCount NOTIFY elementCountChanged STORED false)
    Q_PROPERTY(int subpathCount READ subpathCount NOTIFY subpathCountChanged STORED false)
    Q_PROPERTY(QRectF boundingRect READ boundingRect NOTIFY boundingRectChanged STORED false)
    Q_PROPERTY(bool polygonCache READ polygonCache WRITE setPolygonCache NOTIFY polygonCacheChanged)
    Q_PROPERTY(bool hasFill READ hasFill NOTIFY hasFillChanged)
    Q_PROPERTY(bool hasStroke READ hasStroke NOTIFY hasStrokeChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit DGShape(QObject *parent = 0);
    DGShape(QPainterPath path, DGPen *pen = 0, DGBrush *brush = 0);
    enum ElementType {
        Invalid = -1,
        MoveToElement = 0,
        LineToElement = 1,
        CurveToElement = 2,
        CurveToDataElement = 3
    };

    QPainterPath path() const;
    QList<QPolygonF> fillPolygons() const;
    QList<QPolygonF> pathPolygons() const;


    DGPen* pen() const;
    DGBrush* brush() const;
    int elementCount() const;
    int subpathCount() const;
    QRectF boundingRect() const;
    bool polygonCache() const;

    void setPen(DGPen* arg);
    void setBrush(DGBrush* arg);
    void setPolygonCache(bool arg);

    bool hasFill() const;
    bool hasStroke() const;

    void classBegin() { }
    void componentComplete();

signals:
    void pathChanged();
    void penChanged(DGPen* arg);
    void brushChanged(DGBrush* arg);
    void elementCountChanged(int arg);
    void subpathCountChanged(int arg);
    void boundingRectChanged(QRectF arg);
    void polygonCacheChanged(bool arg);

    void hasFillChanged(bool arg);

    void hasStrokeChanged(bool arg);


    void paramsChanged(QList<qreal> arg);

public slots:
    void begin();
    void end();
    void close();
    void moveTo(qreal x, qreal y);
    void lineTo(qreal x, qreal y);
    void cubicTo(qreal cp1x, qreal cp1y, qreal cp2x, qreal cp2y, qreal x, qreal y);
    void scale(qreal scaleX, qreal scaleY, QPointF origin);
    void translate(qreal dx, qreal dy);
    void setElementPosition(int elementIndex, qreal x, qreal y);
    ElementType getElementType(int elementIndex) const;
    QPointF getElementPos(int elementIndex) const;
    void setPath(const QPainterPath &path);


private:
    void updatePolygons();
    void notifyPathUpdate();
    QPainterPath m_path;
    QList<QPolygonF> m_pathPolygons;
    QList<QPolygonF> m_fillPolygons;
    bool m_polygonCache;
    QRectF m_boundingRect;
    DGPen* m_pen;
    DGBrush* m_brush;
    bool batch;
    bool pathDirty;
    int lastElementCount;
    int lastSubpathCount;
    bool m_hasFill;
    bool m_hasStroke;
};

#endif // DGSHAPE_H
