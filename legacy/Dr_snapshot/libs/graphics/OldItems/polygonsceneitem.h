#ifndef POLYGONSCENEITEM_H
#define POLYGONSCENEITEM_H
#include "sceneitem.h"
#include "polygon.h"
#include "shapesceneitem.h"

class PolylineSceneItem : public ShapeSceneItem
{
    Q_OBJECT
    Q_PROPERTY(GPolygon* polygon READ polygon WRITE setPolygon NOTIFY polygonChanged)

    GPolygon* m_polygon;

public:
    explicit PolylineSceneItem(SceneItem *parent = 0);
    GPolygon *polygon() const;
    void setPolygon(GPolygon* arg);

    void afterDraw(QPainter *painter);
signals:
    void polygonChanged();

private slots:
    void updatePath();
};

#endif // POLYGONSCENEITEM_H
