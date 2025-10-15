#ifndef GUIDEVIEW_H
#define GUIDEVIEW_H

#include "graphicsobject.h"
#include <QObject>
#include <gpen.h>
#include <guide.h>
#include <guidegraph.h>
#include <qqmlpropertymap.h>
#include <boost/container/flat_map.hpp>
using namespace boost::container;

class FindPointReply
{
    Q_GADGET
    Q_PROPERTY(Runner sourceRunner READ sourceRunner)
    Q_PROPERTY(Runner posResultRunner READ posResultRunner)
    Q_PROPERTY(Runner negResultRunner READ negResultRunner)
    Q_PROPERTY(qreal distance READ distance)
    Q_PROPERTY(Segment segment READ segment)

    Runner m_sourceRunner;
    Runner m_posResultRunner;
    Runner m_negResultRunner;
    qreal m_distance;
    Segment m_segment;

    friend class GuideView;

public:
    FindPointReply() {}
    inline Runner sourceRunner() const { return m_sourceRunner; }
    inline Runner posResultRunner() const { return m_posResultRunner; }
    inline Runner negResultRunner() const { return m_negResultRunner; }
    inline qreal distance() const { return m_distance; }
    inline Segment segment() const { return m_segment; }
};
Q_DECLARE_METATYPE(FindPointReply)

class GuideView : public GraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QVariantList segments READ segments NOTIFY segmentsChanged)

    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(qreal precision READ precision WRITE setPrecision NOTIFY precisionChanged)

    Q_PROPERTY(qreal scalingFactor READ scalingFactor WRITE setScalingFactor NOTIFY offsetScaleChanged)
    Q_PROPERTY(GPen negPen READ negPen WRITE setNegPen NOTIFY negPenChanged)
    Q_PROPERTY(GPen posPen READ posPen WRITE setPosPen NOTIFY posPenChanged)

    Q_PROPERTY(bool doubleWire READ doubleWire WRITE setDoubleWire NOTIFY doubleWireChanged)

    struct BoundItem {
        QGraphicsItem       *item;
        QTransform          baseTransform;
        int                 guideType;
        Segment             segment;
        qreal               pos;
        qreal               xOffset;
        qreal               yOffset;
        bool                followAngle;
        bool                fixedScale;
    };


    QMap<uint, Segment>         m_segments;
    flat_map<uint, BoundItem>   m_boundItems;
    QRectF                      bndRect;
    QPainterPath                outline;
    qreal                       m_radius = 8;
    qreal                       m_scale = 1;
    qreal                       m_precision = 1;
    qreal                       m_spacing = 3;

    GPen                        m_negPen;
    GPen                        m_posPen;

    bool                        m_debugNodes = false;
    bool                        m_shapeReady = false;
    bool                        m_doubleWire = false;

    friend class Segment;


public:
    GuideView(QGraphicsItem *parent = nullptr);
    QQmlPropertyMap* items() const;
    Q_INVOKABLE Segment segment(int id) const;

    int count() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    QVariantList segments() const;

    Q_INVOKABLE bool hasSegment(uint uid) const;
    Q_INVOKABLE Segment addSegment(uint uid, Guide path, qreal offset);
    Q_INVOKABLE Runner findNearestPoint(QPointF pos) const;
    Q_INVOKABLE FindPointReply findAtPoint(QPointF pos) const;

    qreal radius() const;
    void setRadius(qreal radius);
    qreal scalingFactor() const;
    void setScalingFactor(qreal scalingFactor);
    bool isShapeReady() const;
    GPen negPen() const;
    GPen posPen() const;
    qreal spacing() const;
    qreal precision() const;
    bool doubleWire() const;

    void setNegPen(GPen negPen);
    void setPosPen(GPen posPen);
    void setSpacing(qreal spacing);
    void setPrecision(qreal precision);
    void setDoubleWire(bool doubleWire);

    QRectF boundingRect() const;
    QPainterPath shape() const;

    static void createLink(Segment from, Segment to, GuideNode::NodeType type);

public slots:
    void removeSegment(uint id);
    void clearLinks(uint id);
    void addLink(uint from, uint to, int type);

    void addOutBoundary(uint localSegmentUid, GuideView *nextSection, uint remoteSegmentUid);
    void addInBoundary(uint localSegmentUid, GuideView *prevSection, uint remoteSegmentUid);

    void setDebugMode(bool debug);
    void updateGeometry();

    int bindItem(VisualInstanceHandle item, Segment segment, int type, qreal position, qreal xOffset, qreal yOffset, bool fixedScale, bool followAngle);
    void unbindItem(int bindingId);


signals:
    void itemsChanged();
    void countChanged();
    void segmentsChanged();
    void radiusChanged(qreal radius);
    void offsetScaleChanged(qreal scalingFactor);
    void shapeReady();

    void negPenChanged(GPen negPen);

    void posPenChanged(GPen posPen);

    void spacingChanged(qreal spacing);
    void doubleWireChanged(bool doubleWire);
    void boundItemChanged(int bindingId, QPointF position, qreal rotation, qreal scale);



    void precisionChanged(qreal precision);

private:

    // QGraphicsItem interface


};


#endif // GUIDEVIEW_H
