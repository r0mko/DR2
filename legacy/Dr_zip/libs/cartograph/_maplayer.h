#ifndef MAPLAYER_H
#define MAPLAYER_H

#include <QQuickItem>
#include <spatialitedb.h>
//#include <private/qquickshadereffectsource_p.h>
#include "mapview.h"

class SpatiaLiteDB;
class Geometry;
class MapLayer : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(MapView* view READ view WRITE setView NOTIFY viewChanged)
    Q_PROPERTY(int sourceSRID READ sourceSRID WRITE setSourceSRID NOTIFY sourceSRIDChanged)

public:
    explicit MapLayer(QQuickItem *parent = 0);
    QSGNode* updatePaintNode(QSGNode *node, UpdatePaintNodeData *data);
    void componentComplete();

    MapView *view() const;
    void setView(MapView* arg);

    uint targetSRID() const;
    SpatiaLiteDB *database() const;

    int sourceSRID() const;
    void setSourceSRID(int sourceSRID);

    inline const QList<Geometry *> &nodes() const { return m_nodes; }

signals:
    void nodesChanged();
    void dataChanged();
    void viewChanged();
    void effectsChanged();
    void sourceSRIDChanged(int sourceSRID);

public slots:
    void addGeometry(Geometry* g);


private:
    QList<Geometry*> m_nodes;
    MapView* m_view = 0;
    int m_sourceSRID;
};

#endif // MAPLAYER_H
