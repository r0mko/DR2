#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "visualinstance.h"
#include <QPainter>
#include <QStaticText>


class TextItem : public VisualInstance
{
    GR_INSTANCE(TextItem)
public:
    QStaticText stext;
    QFont font;


    Qt::AlignmentFlag horizontalAlignment = Qt::AlignLeft;
    Qt::AlignmentFlag verticalAlignment = Qt::AlignBottom;
    QColor color;
    using VisualInstance::VisualInstance;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
    QRectF boundingRect() const;
    void updateText();
};

class TextItemHandle : public VisualInstanceHandle
{
    Q_GADGET
    GR_HANDLE(TextItem)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(int horizontalAlignment READ horizontalAlignment WRITE setHorizontalAlignment)
    Q_PROPERTY(int verticalAlignment READ verticalAlignment WRITE setVerticalAlignment)

public:
    using VisualInstanceHandle::VisualInstanceHandle;


    QString text() const;
    QFont font() const;

    QColor color() const;

    void setText(QString text);
    void setFont(QFont font);

    void setColor(QColor color);
    int horizontalAlignment() const
    {
        return (int)item()->horizontalAlignment;
    }
    int verticalAlignment() const
    {
        return (int)item()->verticalAlignment;
    }

    void setHorizontalAlignment(int horizontalAlignment)
    {
        item()->horizontalAlignment = (Qt::AlignmentFlag)horizontalAlignment;
        update();
    }
    void setVerticalAlignment(int verticalAlignment)
    {
        item()->verticalAlignment = (Qt::AlignmentFlag)verticalAlignment;
        update();
    }
};

DECLARE_ITEM(TextItem)

#endif // TEXTITEM_H
