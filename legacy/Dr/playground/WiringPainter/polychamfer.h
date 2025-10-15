#ifndef POLYCHAMFER_H
#define POLYCHAMFER_H
#include <QtMath>
#include <QObject>
#include <QMetaProperty>
#include <QPolygonF>
#include <QPainterPath>
#include <QVector2D>
class GPolygon;
class GraphicsPath;
struct WireNode {
    QPointF pt;
    QVector2D normal;
    inline QPointF point(qreal offset) const { return pt + (normal * offset).toPointF(); }
};


class SpringWire : public QVector<WireNode>
{
    qreal m_previousAngle;
    mutable qreal preparedAngle;
    mutable const WireNode * preparedNode;
public:

    SpringWire() : m_previousAngle(NAN), preparedNode(nullptr) {}

    void prepareAppend(const WireNode &node, qreal *distance = nullptr, qreal *angleDelta = nullptr) const;
    void commitAppend();

    inline qreal previousAngle() const { return m_previousAngle; }
};

class PolyChamfer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GPolygon* polygon READ polygon WRITE setPolygon NOTIFY polygonChanged)
    Q_PROPERTY(GraphicsPath* path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(qreal length READ length NOTIFY lengthChanged)
    Q_PROPERTY(int order READ order WRITE setOrder NOTIFY orderChanged)
    Q_PROPERTY(qreal strength READ strength WRITE setStrength NOTIFY strengthChanged)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(qreal curveThreshold READ curveThreshold WRITE setCurveThreshold NOTIFY curveThresholdChanged)
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)



public:
    PolyChamfer(QObject *parent = 0) :
        QObject(parent),
        m_gpolygon(nullptr),
        m_order(8),
        m_strength(50),
        m_offset(0),
        m_curveThreshold(5)
    {}
    virtual ~PolyChamfer() {}



    inline qreal length() const { return m_length; }

    inline GPolygon* polygon() const { return m_gpolygon; }
    inline GraphicsPath* path() const { return m_path; }

    void setPolygon(GPolygon* arg);
    void setPath(GraphicsPath* arg);
    void setOrder(int arg);
    void setStrength(qreal arg);
    void setOffset(qreal arg);
    void setCurveThreshold(qreal arg);

    Q_INVOKABLE QPointF pointAtLength(qreal pos, int *nodeNo = nullptr) const;
    Q_INVOKABLE QPointF pointAtPercent(qreal percent) const;
    QPointF getCurvePoint(qreal position, qreal range, int order, bool *sameSegment = nullptr, int *startNode = nullptr, int *endNode = nullptr) const;
    SpringWire getRounding(qreal smooth, int order, qreal minAngleThreshold = 0.1, qreal maxAngleThreshold = 10);
//    Q_INVOKABLE QPointF getTangent(qreal position, qreal range, int order, qreal *angle, bool *singleSegment, qreal npos = 0.5) const;
    QPainterPath getPath(qreal smooth, int order, qreal curveThreshold = 2);

    int order() const;
    qreal strength() const;
    qreal offset() const;
    qreal curveThreshold() const;

    QPolygonF pol() const;
    void setPol(const QPolygonF &pol);

//    QPolygonF getChamfer(qreal smooth, int order, qreal curveThreshold);
    QPolygonF calculateOffset(const SpringWire &wire, qreal offset) ;

    qreal t() const
    {
        return m_t;
    }

public slots:
    void init();
    void update();
    void updatePath(qreal smooth, qreal offset, int order, qreal curveThreshold);


    void setT(qreal arg)
    {
        if (m_t == arg)
            return;

        m_t = arg;
        update();
        emit tChanged(arg);
    }

signals:
    void lengthChanged(qreal arg);
    void polygonChanged(GPolygon* arg);
    void pathChanged(GraphicsPath* arg);
    void orderChanged(int arg);
    void strengthChanged(qreal arg);
    void offsetChanged(qreal arg);

    void curveThresholdChanged(qreal arg);


    void tChanged(qreal arg);

private:

//    QVector2D emitVector(int nodeIdx, qreal range, )

    struct PolNode {
        PolNode() : segLength(0), position(0), angle(0), azimuth(0)
        { }

        qreal   segLength;
        qreal   position;
        qreal   angle;
        qreal   azimuth;
        QVector2D   normal;
    };

    GPolygon*           m_gpolygon;
    mutable QPolygonF   m_polygon;
    GraphicsPath*       m_path;
    int                 m_order;
    qreal               m_strength;
    qreal               m_offset;
    qreal               m_curveThreshold;
    qreal               m_length;
    QMetaProperty       polProp;
    QMetaProperty       pathProp;
    QVector<PolNode>    nodes;


    QPolygonF   getPolygon() const;
    void        writePath(const QPainterPath &path) const;
    QPointF m_selfIntersectPoint;
    qreal m_t;
};
#endif // POLYCHAMFER_H
