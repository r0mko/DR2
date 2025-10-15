#ifndef DGITEM_H
#define DGITEM_H

#include <QQuickItem>
#include "dgcursor.h"

class DGItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(DGCursor* customCursor READ customCursor WRITE setCustomCursor)

public:
    explicit DGItem(QQuickItem *parent = 0);

    void setCustomCursor(DGCursor *arg) { m_customCursor = arg; setCursor(arg->cursor()); }
    DGCursor *customCursor() const { return m_customCursor; }
    QRectF contentsBoundingRect() const;

signals:
    void customCursorChanged(DGCursor* arg);
private:
    DGCursor* m_customCursor;

};

#endif // DGITEM_H
