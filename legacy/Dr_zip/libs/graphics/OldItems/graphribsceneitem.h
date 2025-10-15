#ifndef GRAPHRIBSCENEITEM_H
#define GRAPHRIBSCENEITEM_H
#include "shapesceneitem.h"
#include <QtMath>
#include "vectorpath.h"
#include "groove.h"
class RibArrow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(qreal step READ step WRITE setStep NOTIFY stepChanged)
    Q_PROPERTY(Pen* pen READ pen WRITE setPen NOTIFY penChanged)

    qreal m_width;
    qreal m_offset;
    qreal m_length;
    qreal m_step;
    Pen* m_pen;

public:
    RibArrow(QObject *parent = 0);
    virtual ~RibArrow() {}
    qreal width() const { return m_width; }
    qreal offset() const { return m_offset; }
    qreal length() const { return m_length; }
    qreal step() const { return m_step; }
    Pen* pen() const { return m_pen; }

    void setWidth(qreal arg);
    void setOffset(qreal arg);
    void setLength(qreal arg);
    void setStep(qreal arg);
    void setPen(Pen* arg);

signals:
    void widthChanged(qreal arg);
    void offsetChanged(qreal arg);
    void lengthChanged(qreal arg);
    void stepChanged(qreal arg);
    void penChanged(Pen* arg);
};

class GraphRibSceneItem : public ShapeSceneItem
{
    Q_OBJECT
    Q_PROPERTY(QPolygonF polygon READ polygon WRITE setPolygon NOTIFY polygonChanged)
    Q_PROPERTY(bool oneWay READ oneWay WRITE setOneWay NOTIFY oneWayChanged)
    Q_PROPERTY(qreal length READ length NOTIFY lengthChanged)
    Q_PROPERTY(bool showArrows READ showArrows WRITE setShowArrows NOTIFY showArrowsChanged)
    Q_PROPERTY(Pen* strokePen READ strokePen WRITE setStrokePen NOTIFY strokePenChanged)
    Q_PROPERTY(RibArrow* arrow READ arrow CONSTANT)
    Q_PROPERTY(int node1 READ node1 WRITE setNode1 NOTIFY node1Changed)
    Q_PROPERTY(int node2 READ node2 WRITE setNode2 NOTIFY node2Changed)
    Q_PROPERTY(bool highlight READ highlight WRITE setHighlight NOTIFY highlightChanged)
    Q_PROPERTY(int modelIdx READ modelIdx WRITE setModelIdx NOTIFY modelIdxChanged)
    Q_PROPERTY(Groove* guide READ guide CONSTANT)


public:
    explicit GraphRibSceneItem(SceneItem *parent = 0);

    QPolygonF polygon() const;

    bool oneWay() const;
    qreal length() const;
    bool showArrows() const;
    Pen *strokePen() const;
    RibArrow* arrow() const;
    int node1() const;
    int node2() const;
    bool highlight() const;
    int modelIdx() const;

    void setPolygon(QPolygonF arg);
    void setOneWay(bool arg);
    void setShowArrows(bool arg);
    void setStrokePen(Pen* arg);
    void setNode1(int arg);
    void setNode2(int arg);
    void setHighlight(bool arg);
    void setModelIdx(int arg);
    Q_INVOKABLE QVariantMap nearestPoint(QPointF point) const;
    Groove* guide() const;

public slots:
    void highlightRange(qreal startPos, qreal endPos);
    void clearRangeHighlight();

signals:
    void polygonChanged(QPolygonF polygon);
    void lengthChanged(qreal length);
    void oneWayChanged(bool oneWay);
    void showArrowsChanged(bool arg);
    void strokePenChanged(Pen* arg);
    void node1Changed(int arg);
    void node2Changed(int arg);
    void highlightChanged(bool arg);
    void modelIdxChanged(int arg);

protected:
    void afterDraw(QPainter *painter);

private:
    bool                m_oneWay;
    bool                m_showArrows;
    Pen*                m_strokePen;
    RibArrow*           m_arrow;
    int                 m_node1;
    int                 m_node2;
    bool                m_highlight;
    int                 m_modelIdx;
    Groove*             m_guide;
    qreal               m_length;
    qreal               m_inPosition;
    qreal               m_outPosition;
};
#endif // GRAPHRIBSCENEITEM_H
