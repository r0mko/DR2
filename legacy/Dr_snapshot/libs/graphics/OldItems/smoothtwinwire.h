#ifndef SMOOTHTWINWIRE_H
#define SMOOTHTWINWIRE_H
#include "smoothwire.h"

class SmoothTwinWire : public SmoothWire
{
    Q_OBJECT
    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(QColor auxColor READ auxColor WRITE setAuxColor NOTIFY auxColorChanged)

    QPolygonF wire1;
    QPolygonF wire2;
    qreal m_spacing;

    QColor m_auxColor;

public:
    SmoothTwinWire(SceneItem *parent = 0);
    ~SmoothTwinWire();

    // WiringElement interface
    qreal spacing() const
    {
        return m_spacing;
    }

    QColor auxColor() const
    {
        return m_auxColor;
    }

    void setSpacing(qreal arg)
    {
        if (m_spacing == arg)
            return;

        m_spacing = arg;
        emit spacingChanged(arg);
    }

    void setAuxColor(QColor arg)
    {
        if (m_auxColor == arg)
            return;

        m_auxColor = arg;
        emit auxColorChanged(arg);
    }
public slots:

signals:
    void spacingChanged(qreal arg);

    void auxColorChanged(QColor arg);

protected:
    void paint(QPainter *painter);
    QRectF getBoundingRect() const;

    // SmoothWire interface
protected:
    void build();
};

#endif // SMOOTHTWINWIRE_H
