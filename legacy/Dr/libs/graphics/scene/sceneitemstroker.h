#ifndef SCENEITEMSTROKER_H
#define SCENEITEMSTROKER_H
#include <QPainterPath>
#include <QQuickPaintedItem>


class SceneView;
class SceneItem;
class Scene;
class SceneItemStroker : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Scene* scene READ scene WRITE setScene NOTIFY sceneChanged)

public:
    SceneItemStroker(SceneItem *parent = 0);
    void paint(QPainter *painter);
    Scene* scene() const;

signals:
    void sceneChanged(Scene* arg);

public slots:
    void setScene(Scene *arg);
    void updateShape();

private:
    Scene* m_scene;
    QPainterPath shape;


};


#endif // SCENEITEMSTROKER_H
