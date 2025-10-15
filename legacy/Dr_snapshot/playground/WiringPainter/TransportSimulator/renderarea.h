#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QFile>

class RenderArea : public QGraphicsView
{
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = 0);
    ~RenderArea();

signals:

private:
    double zoomFactor;

    // QWidget interface
protected:
    virtual void wheelEvent(QWheelEvent *);
};

#endif // RENDERAREA_H
