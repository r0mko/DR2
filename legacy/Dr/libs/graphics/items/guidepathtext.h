#ifndef GUIDEPATHTEXT_H
#define GUIDEPATHTEXT_H

#include "visualinstance.h"
#include <guidepath.h>
#include <qrawfont.h>



class GuidePathText : public VisualInstance
{
    GR_INSTANCE(GuidePathText)
    QFont font;
    QString text;
    QColor textColor;
    QColor outlineColor;
    Qt::Orientation orientation = Qt::Horizontal;
    QRawFont m_rawFont { QRawFont::fromFont(QFont("Arial", 12)) };
    QVector< quint32 > glyphIndexList;
    qreal labelWidth = 0;
    GuidePath path;
//        QPainterPath path;
    QHash< quint32, QPainterPath > m_glyphPathCache;
    QVector< QPointF > advances;
    qreal position = 0;

    void updateText();


public:
    using VisualInstance::VisualInstance;

    // QGraphicsItem interface
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    int type() const;


    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
};

class GuidePathTextHandle : public VisualInstanceHandle
{
    Q_GADGET
    GR_HANDLE(GuidePathText)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
    Q_PROPERTY(QColor outlineColor READ outlineColor WRITE setOutlineColor)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(qreal position READ position WRITE setPosition)
    Q_PROPERTY(GuidePath path READ path WRITE setPath)


public:
    using VisualInstanceHandle::VisualInstanceHandle;

    QString text() const;
    QFont font() const;
    QColor textColor() const;
    QColor outlineColor() const;
    Qt::Orientation orientation() const;
    qreal position() const;
    GuidePath path() const
    {
        return item()->path;
    }

    void setText(QString text);
    void setFont(QFont font);
    void setTextColor(QColor textColor);
    void setOutlineColor(QColor outlineColor);
    void setOrientation(Qt::Orientation orientation);
    void setPosition(qreal position);
    void setPath(GuidePath path)
    {
        qDebug() << "Set path size" << path.count() << "length" << path.length();
        item()->path = path;
        item()->update();
    }

};

DECLARE_ITEM(GuidePathText)

#endif // GUIDEPATHTEXT_H
