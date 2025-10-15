#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <QGraphicsScene>

class RenderScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit RenderScene(QObject *parent = 0);
    ~RenderScene();

signals:

public slots:

};

#endif // RENDERSCENE_H
