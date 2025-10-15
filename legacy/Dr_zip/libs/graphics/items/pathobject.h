#ifndef PATHOBJECT_H
#define PATHOBJECT_H
#include "graphicsobject.h"
#include <QPainterPath>
#include <gbrush.h>
#include <gpen.h>

class PathObject : public GraphicsObject
{
    Q_OBJECT
    Q_ENUMS(ShapeType)
    Q_PROPERTY(QPainterPath path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(ShapeType shapeType READ shapeType WRITE setShapeType NOTIFY shapeTypeChanged)
    Q_PROPERTY(GPen pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(GBrush brush READ brush WRITE setBrush NOTIFY brushChanged)
    Q_PROPERTY(Qt::FillRule fillRule READ fillRule WRITE setFillRule NOTIFY fillRuleChanged)



public:
    enum ShapeType {
        Invalid   = 0,
        Linear    = 1,
        Polygonal = 2
    };
    Q_ENUM(ShapeType)

    PathObject(QGraphicsItem *parent = nullptr);
    QPainterPath path() const;
    ShapeType shapeType() const;
    GPen pen() const;
    GBrush brush() const;
    Qt::FillRule fillRule() const;
    QPainterPath shape() const;
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
public slots:
    void setPath(QPainterPath path);
    void setShapeType(ShapeType shapeType);
    void setPen(GPen pen);
    void setBrush(GBrush brush);
    void setFillRule(Qt::FillRule fillRule);
    void setBlob(QByteArray ba);


signals:
    void pathChanged(QPainterPath path);
    void shapeTypeChanged(ShapeType shapeType);
    void penChanged(GPen pen);
    void brushChanged(GBrush brush);
    void fillRuleChanged(Qt::FillRule fillRule);

private:
    QPainterPath    m_path;
    ShapeType       m_shapeType;
    GPen            m_pen;
    GBrush          m_brush;
    Qt::FillRule    m_fillRule;
};

#endif // PATHOBJECT_H
