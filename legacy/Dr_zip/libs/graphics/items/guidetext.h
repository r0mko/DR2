#ifndef GUIDETEXT_H
#define GUIDETEXT_H

#include "visualinstance.h"
#include <guide.h>
#include <qrawfont.h>

class GuideText : public VisualInstance
{
    GR_INSTANCE(GuideText)
    QFont font;
    QString text;
    QColor textColor;
    QColor outlineColor;
    Qt::Orientation orientation = Qt::Horizontal;
    QRawFont m_rawFont { QRawFont::fromFont(QFont("Arial", 12)) };
    qreal ascent = 0;
    qreal descent = 0;
    QVector< quint32 > glyphIndexList;
    qreal labelWidth = 0;
    Guide path;
//        QPainterPath path;
    QHash< quint32, QPainterPath > m_glyphPathCache;
    QVector< QPointF > advances;
    qreal position = 0;
    QPolygonF mesh;
    void updateText();
    void updateMesh();

public:
    GuideText(VisualInstance * parent = nullptr) : VisualInstance( parent ) { ascent = m_rawFont.ascent(); descent = m_rawFont.descent(); }
    using VisualInstance::VisualInstance;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    QRectF boundingRect() const;
};

class GuideTextHandle : public VisualInstanceHandle
{
    Q_GADGET
    GR_HANDLE(GuideText)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
    Q_PROPERTY(QColor outlineColor READ outlineColor WRITE setOutlineColor)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(qreal position READ position WRITE setPosition)
    Q_PROPERTY(Guide path READ path WRITE setPath)
    Q_PROPERTY(QPolygonF mesh READ mesh)


public:
    using VisualInstanceHandle::VisualInstanceHandle;

    QString text() const;
    QFont font() const;
    QColor textColor() const;
    QColor outlineColor() const;
    Qt::Orientation orientation() const;
    qreal position() const;
    Guide path() const;


    void setText(QString text);
    void setFont(QFont font);
    void setTextColor(QColor textColor);
    void setOutlineColor(QColor outlineColor);
    void setOrientation(Qt::Orientation orientation);
    void setPosition(qreal position);
    void setPath(Guide path)
    {
        qDebug() << "Set path size" << path.count() << "length" << path.length();
        item()->path = path;
        item()->update();
    }

    QPolygonF mesh() const
    {
        item()->updateMesh();
        return item()->mesh;
    }

};

DECLARE_ITEM(GuideText)

#endif // GUIDETEXT_H
