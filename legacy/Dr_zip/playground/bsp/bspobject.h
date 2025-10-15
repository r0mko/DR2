#ifndef BSPOBJECT_H
#define BSPOBJECT_H
#include <QObject>
#include <QVariantList>
#include "../../libs/graphics/tools/bsp.h"

class BspObject: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged)
    Q_PROPERTY(int maxDepth READ maxDepth WRITE setMaxDepth NOTIFY maxDepthChanged)

    BspTree<int> m_tree;

public:
    BspObject(QObject *parent = nullptr);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantList dumpRects() const;
    Q_INVOKABLE void insertPoint(const QPointF &point, const int &data);
    Q_INVOKABLE void insertPoints(const QPolygonF &polygon, const int &data);

    QRectF rect() const;
    int maxDepth() const;

public slots:
    void setRect(QRectF rect);
    void setMaxDepth(int maxDepth);

signals:
    void cleared();

    void rectChanged(QRectF rect);
    void maxDepthChanged(int maxDepth);
};

#endif // BSPOBJECT_H
