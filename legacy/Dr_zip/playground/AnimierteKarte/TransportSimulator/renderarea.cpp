#include "renderarea.h"

RenderArea::RenderArea(QWidget *parent) : QGraphicsView(parent)
{
    setMouseTracking(true);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);    
    setDragMode(QGraphicsView::ScrollHandDrag);
    viewport()->setCursor(Qt::OpenHandCursor);

    zoomFactor = 1.14;
}

RenderArea::~RenderArea()
{

}

void RenderArea::wheelEvent(QWheelEvent *wheelEvent)
{
    double zoomValue;
    if(wheelEvent->delta() > 0)
    {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        zoomValue = zoomFactor;
        scale(zoomValue, zoomValue);
    }
    else
    {
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);
        zoomValue = 1.0 / zoomFactor;
    }

    scale(zoomValue, zoomValue);

    wheelEvent->accept();
}
