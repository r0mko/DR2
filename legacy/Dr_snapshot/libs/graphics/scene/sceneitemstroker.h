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
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(Qt::PenStyle penStyle READ penStyle WRITE setPenStyle NOTIFY penStyleChanged)
    Q_PROPERTY(Qt::PenCapStyle capStyle READ capStyle WRITE setCapStyle NOTIFY capStyleChanged)

public:
    SceneItemStroker(SceneItem *parent = 0);
    void paint(QPainter *painter);
    Scene* scene() const;

    QColor color() const;
    void setColor(QColor color);

    Qt::PenStyle penStyle() const;
    void setPenStyle(Qt::PenStyle penStyle);

    Qt::PenCapStyle capStyle() const;
    void setCapStyle(Qt::PenCapStyle capStyle);

signals:
    void sceneChanged(Scene* arg);

    void colorChanged(QColor color);
    void penStyleChanged(Qt::PenStyle penStyle);

    void capStyleChanged(Qt::PenCapStyle capStyle);

public slots:
    void setScene(Scene *arg);
    void updateShape();



private:
    Scene* m_scene;
    QPainterPath shape;

    QColor m_color { QColor(255, 0, 255) };
    Qt::PenStyle m_penStyle = Qt::SolidLine;
    Qt::PenCapStyle m_capStyle = Qt::FlatCap;
};


#endif // SCENEITEMSTROKER_H
