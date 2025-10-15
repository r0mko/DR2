#ifndef STYLEPREVIEWITEM_H
#define STYLEPREVIEWITEM_H

#include "defs.h"
#include "style.h"
#include "stylenode.h"
#include <QQuickItem>

class StylePreviewItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int shapeSides READ shapeSides WRITE setShapeSides NOTIFY shapeSidesChanged)
    Q_PROPERTY(Style style READ style WRITE setStyle NOTIFY styleChanged)

public:
    StylePreviewItem(QQuickItem *parent = 0);
    virtual ~StylePreviewItem() {}

    int shapeSides() const;
    void setShapeSides(int shapeSides);

    Style style() const;

public slots:
    void setStyle(Style style);

signals:
    void shapeSidesChanged(int shapeSides);
    void styleChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
private:
    void createGeometry();
    VertexBuffer        geometry;
    Style               m_style;
    int                 m_shapeSides = 6;
};

#endif // STYLEPREVIEWITEM_H
