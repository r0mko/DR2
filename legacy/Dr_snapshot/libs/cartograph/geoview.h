#ifndef GEOVIEW_H
#define GEOVIEW_H

#include "instantiatorcollection.h"
#include <QQuickItem>
#include <QTransform>
#include <QQuickPaintedItem>

class GeoLayer;
class GeoView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int targetSRID READ targetSRID WRITE setTargetSRID NOTIFY targetSRIDChanged)
    Q_PROPERTY(QRectF viewExtent READ viewExtent NOTIFY viewExtentChanged)
    Q_PROPERTY(bool isValid READ isValid WRITE setValid NOTIFY isValidChanged)
    Q_PROPERTY(QTransform viewTransform READ viewTransform NOTIFY viewTransformChanged)
    Q_PROPERTY(QStringList availableStyles READ availableStyles CONSTANT)
    Q_PROPERTY(qreal contentWidth READ contentWidth WRITE setContentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(qreal contentHeight READ contentHeight WRITE setContentHeight NOTIFY contentHeightChanged)


public:
    GeoView(QQuickItem *parent = 0);

    Q_INVOKABLE QVariantList getStyleProperties(QString styleType) const; // FIXME: get rid of this — move to global helper class

    void componentComplete();

    int targetSRID() const;
    void setTargetSRID(int targetSRID);

    QRectF viewExtent() const;

    bool isValid() const;

    QTransform viewTransform() const;

    inline QStringList availableStyles() const { return styleTypes; }


    inline qreal contentWidth() const { return m_contentWidth; }
    void setContentWidth(qreal contentWidth);

    inline qreal contentHeight() const { return m_contentHeight; }
    void setContentHeight(qreal contentHeight);

    static QStringList styleTypes;

signals:
    void targetSRIDChanged(int targetSRID);
    void viewExtentChanged(QRectF viewExtent);
    void isValidChanged();
    void viewTransformChanged();
    void contentHeightChanged(qreal contentHeight);
    void contentWidthChanged(qreal contentWidth);

public slots:
    void addLayer(GeoLayer *layer);

    QPointF getPoint(QPointF geo);

    QPolygonF morf(QPolygonF s){ // TODO: reomve that method and make saving and restoring routes in right transforms
        QTransform t1(11.012, 0,0,0,-11.012,0,4.09507e+06,7.56412e+06,1);
        QTransform t2(0.0760805,0,0,0,-0.0760805,0,-311555,578078,1);
        QTransform t3;
        t3.translate(0, -0.8);
        return t3.map( t2.map( t1.map(s) ) );
    }

private slots:
    void adjustTransform();
    void fitLayers();

protected:
    void itemChange(ItemChange change, const ItemChangeData &value);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
    int                 m_targetSRID = -1;
    QRectF              m_viewExtent;
    bool                m_isValid = false;
    QTransform          m_viewTransform; // from geographic coordinates to view bounds
    QList<GeoLayer *>   m_layers;
    qreal               m_contentWidth = 0;
    qreal               m_contentHeight = 0;

    void setValid(bool isValid);
    void setViewExtent(QRectF viewExtent);
    void initView();


};

#endif // GEOVIEW_H
