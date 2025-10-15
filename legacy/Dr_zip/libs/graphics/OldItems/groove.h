#ifndef GROOVE_H
#define GROOVE_H

#include <QObject>
#include "vectorpath.h"
#include "pathsplitter.h"
class GrooveRunner;

class Groove : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPolygonF polygon READ polygon WRITE setPolygon NOTIFY polygonChanged)
    Q_PROPERTY(qreal length READ length NOTIFY lengthChanged)
    Q_PROPERTY(bool isNull READ isNull NOTIFY isNullChanged)
    Q_PROPERTY(QVariantList polygonData READ polygonData WRITE setPolygonData NOTIFY polygonDataChanged)
    Q_PROPERTY(QObject* nodeModel READ nodeModel WRITE setNodeModel NOTIFY nodeModelChanged)
    VectorPath *v;
    QObject* m_nodeModel;
    bool ownsPath;

public:
    Groove(QObject *parent = 0);
    Groove(const VectorPath &vp, QObject *parent);
    Groove(VectorPath *vp, QObject *parent = 0);
    ~Groove();
    qreal length() const;
    bool isNull() const;
    QPolygonF polygon() const;
    Q_INVOKABLE QPointF pointAtLength(qreal length);
    Q_INVOKABLE QPointF nearestPoint(QPointF point);
    Q_INVOKABLE qreal nearestPos(QPointF point) const;
    Q_INVOKABLE GrooveRunner *setRunner(qreal pos) const;
    inline const VectorPath &vectorPath() const;
    inline VectorPath &vectorPath();
    inline VectorPath *vectorPathPtr() const;
    Q_INVOKABLE QVariantList partData(qreal inPos, qreal outPos);
    Q_INVOKABLE Groove *split(qreal pos);
    QVariantList toVaritantList(const VectorPath &vectorPath) const;
    QObject* nodeModel() const;
    QVariantList polygonData() const;

    void setVectorPath(const VectorPath &vp);

signals:
    void lengthChanged();
    void isNullChanged();
    void polygonChanged();
    void polygonDataChanged();
    void nodeModelChanged();

public slots:
    void setPolygon(QPolygonF arg);
    void addPolygon(QPolygonF polygon, QVariant &data);
    void setPolygonData(QVariantList arg);
    void setNodeModel(QObject *arg);
private:
    void setLength(qreal arg);
};


class GrooveRunner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(Groove* guide READ guide WRITE setGuide NOTIFY grooveChanged)

    Q_PROPERTY(bool isNull READ isNull NOTIFY isNullChanged)

    Q_PROPERTY(bool atEnd READ atEnd NOTIFY atEndChanged)
    Q_PROPERTY(bool atStart READ atStart NOTIFY atStartChanged)
    Q_PROPERTY(bool atNode READ atNode NOTIFY atNodeChanged)

    Q_PROPERTY(bool atFirstEdge READ atFirstEdge NOTIFY atFirstEdgeChanged)
    Q_PROPERTY(bool atLastEdge READ atLastEdge NOTIFY atLastEdgeChanged)

    Q_PROPERTY(QVariant data READ data WRITE setData NOTIFY dataChanged)

    Q_PROPERTY(qreal lengthBehind READ lengthBehind NOTIFY lengthBehindChanged)
    Q_PROPERTY(qreal lengthAhead READ lengthAhead NOTIFY lengthAheadChanged)
    Q_PROPERTY(QPointF point READ point NOTIFY pointChanged)
    Q_PROPERTY(qreal angle READ angle NOTIFY angleChanged)
    Q_PROPERTY(QVector2D normal READ normal NOTIFY normalChanged)

    Groove* m_guide;
    VectorPath::Runner r;

public:
    GrooveRunner() : m_guide(nullptr) {}
    GrooveRunner(Groove *guide, const VectorPath::Runner &r);
    virtual ~GrooveRunner();
    Groove* guide() const;
    qreal position() const;
    bool isNull() const;
    bool atEnd() const;
    bool atStart() const;
    bool atNode() const;
    bool atFirstEdge() const;
    bool atLastEdge() const;
    QVariant data() const;
    qreal lengthBehind() const;
    qreal lengthAhead() const;
    QPointF point() const;
    qreal angle() const;
    QVector2D normal() const;

    void setGuide(Groove* arg);
    void setPosition(qreal arg);
    void setData(QVariant arg);

    Q_INVOKABLE QPointF smoothedPoint(qreal radius) const;
    Q_INVOKABLE QVariantMap getWirePoint(qreal radius, qreal offset = 0) const;
    Q_INVOKABLE QVariant prevData() const;
    Q_INVOKABLE QVariant nextData() const;
public slots:
    void moveBy(qreal length);
    void refresh();

signals:
    void positionChanged();
    void grooveChanged();
    void isNullChanged();
    void atEndChanged();
    void atStartChanged();
    void atNodeChanged();
    void atFirstEdgeChanged();
    void atLastEdgeChanged();
    void dataChanged();
    void lengthBehindChanged();
    void lengthAheadChanged();
    void pointChanged();
    void angleChanged();
    void normalChanged();
    void moved(qreal distance);


private:
    void notifyChanges();
    bool m_isNull;
};

class GuideSplitter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Groove* left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(Groove* right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(bool atLeftEnd READ atLeftEnd NOTIFY atLeftEndChanged)
    Q_PROPERTY(bool atRightEnd READ atRightEnd NOTIFY atRightEndChanged)
    Q_PROPERTY(qreal x READ x NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y NOTIFY yChanged)
    Q_PROPERTY(QPointF position READ position NOTIFY positionChanged)

    Groove* m_left;
    Groove* m_right;

    PathSplitter splitter;

public:
    GuideSplitter();
    virtual ~GuideSplitter() {}

    void setLeft(Groove* arg);
    void setRight(Groove* arg);

    Groove* left() const;
    Groove* right() const;

    bool atLeftEnd() const;
    bool atRightEnd() const;
    Q_INVOKABLE qreal nearestPos(QPointF pos) const;
    qreal x() const;
    qreal y() const;
    QPointF position() const;

public slots:
    void setSplitter(Groove *other, qreal pos);
    void shiftLeft(qreal dist);
    void shiftRight(qreal dist);
    void shift(qreal dist);


signals:
    void leftChanged(Groove* arg);
    void rightChanged(Groove* arg);
    void atLeftEndChanged(bool arg);
    void atRightEndChanged(bool arg);
    void xChanged();
    void yChanged();
    void positionChanged();
};

const VectorPath &Groove::vectorPath() const
{
    return *v;
}

VectorPath &Groove::vectorPath()
{
    return *v;
}

VectorPath *Groove::vectorPathPtr() const
{
    return v;
}


#endif // GROOVE_H
