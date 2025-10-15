#ifndef STREETLAYER_H
#define STREETLAYER_H

#include <QQuickPaintedItem>
#include <QFont>
#include "snipfilter.h"
class QPainter;
class MapView;

class StreetLayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(MapView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
    Q_PROPERTY(QQmlListProperty<SnipFilter> snipFilterList READ qmlSnipFilterList NOTIFY snipFilterListChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)


    Q_CLASSINFO("DefaultProperty", "snipFilterList")

public:
    explicit StreetLayer(QQuickItem *parent = 0);
    void paint(QPainter *painter);
    void componentComplete();
    void classBegin() {}

    QFont font() const;
    MapView* mapView() const;
    QQuickItem* scene() const;

    void setFont(QFont arg);
    void setMapView(MapView* arg);
    void setScene(QQuickItem *arg);
    QRectF viewPortRect() const;

    QColor color() const;

signals:
    void fontChanged(QFont arg);
    void mapViewChanged(MapView* arg);
    void snipFilterListChanged();
    void sceneChanged();
    void colorChanged(QColor arg);
    void layoutChanged();

public slots:
    void initPaths();
    void updateLabels();
    void setColor(QColor arg);

private:
    QQuickItem *m_scene = nullptr;
    QFont m_font;
    MapView* m_mapView;
    QTransform m_viewTransform;
    QRectF m_viewPortRect;
    QList<SnipFilter*> m_snipFilterList;
    void readDB();

    QQmlListProperty<SnipFilter> qmlSnipFilterList();
    static void qmlSnipFilterListAppend(QQmlListProperty<SnipFilter> *list, SnipFilter *object);
    static int qmlSnipFilterListCount(QQmlListProperty<SnipFilter> *list);
    static SnipFilter *qmlSnipFilterListAt(QQmlListProperty<SnipFilter> *list, int index);



    QColor m_color;
};



#endif // STREETLAYER_H
