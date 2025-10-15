#include "dgitem.h"



DGItem::DGItem(QQuickItem *parent)
    : QQuickItem(parent),
      m_customCursor(0)
{

}

QRectF DGItem::contentsBoundingRect() const
{
    return QRectF(0, 0, width(), height());
}
