#include "xvfbitem.h"
#include "xvfb.h"
#include <QDebug>

XvfbItem::XvfbItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    connect(Xvfb::instance(), &Xvfb::updated, this, &XvfbItem::onUpdated);
}

void XvfbItem::paint(QPainter *painter)
{
    QImage img = Xvfb::instance()->getFullImage();
    //qDebug() << "Painter" << painter->window() << boundingRect();
    QRect rect = painter->window();
    painter->drawImage(contentsBoundingRect(), img.scaled(rect.width(), rect.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void XvfbItem::componentComplete()
{
    QQuickPaintedItem::componentComplete();
    update();
}

void XvfbItem::onUpdated()
{
    update();
}

