#ifndef XVFBITEM_H
#define XVFBITEM_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPainter>

class XvfbItem : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit XvfbItem(QQuickItem *parent = 0);
    void paint(QPainter *painter);

    void componentComplete();

signals:

public slots:
    void onUpdated();

};

#endif // XVFBITEM_H
