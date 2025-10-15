#ifndef SMOOTHLINE_H
#define SMOOTHLINE_H
#include "vectorpath.h"
#include "sceneitem.h"
#include "pen.h"
class Groove;
class SmoothLine : public SceneItem
{
    Q_OBJECT
    Q_PROPERTY(Pen* pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(bool displayDebugNodes READ displayDebugNodes WRITE setDisplayDebugNodes NOTIFY displayDebugNodesChanged)
    Q_PROPERTY(qreal precision READ precision WRITE setPrecision NOTIFY precisionChanged)
    Q_PROPERTY(qreal startPos READ startPos WRITE setStartPos NOTIFY startPosChanged)
    Q_PROPERTY(qreal endPos READ endPos WRITE setEndPos NOTIFY endPosChanged)
    Q_PROPERTY(qreal length READ length NOTIFY lengthChanged)
    Q_PROPERTY(Groove* guide READ guide CONSTANT)
    Q_PROPERTY(Groove* result READ result CONSTANT)
    Q_PROPERTY(qreal minimumWidth READ minimumWidth WRITE setMinimumWidth NOTIFY minimumWidthChanged)


    class SmoothLineItem : public QuickGraphicsShapeItem
    {
        QPolygonF polygon;

        qreal m_offset;
        qreal m_zoomFactor;
        qreal m_radius;
        qreal m_precision;
        bool m_displayDebugNodes;
        qreal m_startPos;
        qreal m_endPos;
        Groove* m_guide;
        Groove* m_result;

    public:
        QVector<QLineF> lines;
        SmoothLineItem(SceneItem *item, QGraphicsItem *parent = 0);
        qreal offset() const;
        qreal zoomFactor() const;
        qreal radius() const;
        qreal precision() const;
        qreal startPos() const;
        qreal endPos() const;
        Groove *guide() const;
        Groove *result() const;

        bool displayDebugNodes() const;
        void setOffset(const qreal &offset);
        void setZoomFactor(qreal arg);
        void setRadius(qreal arg);
        void setPrecision(qreal arg);
        void setDisplayDebugNodes(bool arg);
        void setStartPos(qreal arg);
        void setEndPos(qreal arg);
        void setGuide(Groove *guide);
        void setResult(Groove *result);

        void build(qreal startPos = 0, qreal endPos = HUGE);
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    };

    PROTO_ITEM(SmoothLineItem);

public:
    SmoothLine(SceneItem *parent = 0);
    ~SmoothLine();

    void setPen(Pen* arg);
    void setOffset(qreal arg);
    void setZoomFactor(qreal arg);
    void setRadius(qreal arg);
    void setPrecision(qreal arg);
    void setStartPos(qreal arg);
    void setEndPos(qreal arg);
    void setMinimumWidth(qreal arg);
    const VectorPath &vectorPath() const;
    Pen* pen() const;
    qreal offset() const;
    qreal zoomFactor() const;
    qreal radius() const;
    qreal precision() const;
    qreal startPos() const;
    qreal endPos() const;
    bool displayDebugNodes() const;
    qreal length() const;

    Groove* guide() const;
    Groove* result() const;

    qreal minimumWidth() const;

public slots:
    void appendRib(Groove *guide, qreal inPosition, qreal outPosition, QVariant data = QVariant());
    void clear();
    void init();
    void setDisplayDebugNodes(bool arg);

signals:
    void penChanged(Pen* arg);
    void offsetChanged(qreal arg);
    void zoomFactorChanged(qreal arg);
    void radiusChanged(qreal arg);
    void displayDebugNodesChanged(bool arg);
    void precisionChanged(qreal arg);
    void startPosChanged(qreal arg);
    void endPosChanged(qreal arg);
    void lengthChanged(qreal arg);
    void minimumWidthChanged(qreal arg);
    void ready();
    void resultReady();
};



#endif // SMOOTHLINE_H
