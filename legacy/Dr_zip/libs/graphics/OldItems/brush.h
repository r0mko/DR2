#ifndef BRUSH_H
#define BRUSH_H

#include <QObject>
#include <QBrush>
class Brush : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(Qt::BrushStyle style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)

public:
    explicit Brush(QObject *parent = 0);
    virtual ~Brush() {}
    QBrush brush() const;
    QColor color() const;
    Qt::BrushStyle style() const;

    void setColor(QColor arg);
    void setStyle(Qt::BrushStyle arg);

    bool visible() const;

public slots:

signals:
    void colorChanged(QColor arg);
    void styleChanged(Qt::BrushStyle arg);
    void visibleChanged(bool arg);
    void brushChanged();
private:
    QBrush m_brush;
    bool m_visible;
};
#endif // BRUSH_H
