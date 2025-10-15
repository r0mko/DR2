
#ifndef QQUICKPICTURE_H
#define QQUICKPICTURE_H

#include <QQuickPaintedItem>
#include <QPainter>
#include <QPicture>
#include <QBuffer>


class DRPicture : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QByteArray sourceData READ sourceData WRITE setSourceData NOTIFY sourceDataChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged RESET resetColor)
    Q_PROPERTY(Qt::AspectRatioMode aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(qreal adjustX READ adjustX WRITE setAdjustX NOTIFY adjustXChanged)
    Q_PROPERTY(qreal adjustY READ adjustY WRITE setAdjustY NOTIFY adjustYChanged)
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing NOTIFY antialiasingChanged)

public:
    explicit DRPicture(QQuickItem *parent = 0);
    ~DRPicture();

    void paint(QPainter *painter) Q_DECL_OVERRIDE;

    QUrl source() const;
    void setSource(const QUrl &source);

    QColor color() const;
    void setColor(const QColor &color);
    void resetColor();

    QByteArray sourceData();
    void setSourceData(QByteArray arg);

    Qt::AspectRatioMode aspectRatio() const;
    void setAspectRatio(Qt::AspectRatioMode aspectRatio);

    qreal adjustX() const;
    void setAdjustX(qreal adjustX);

    qreal adjustY() const;
    void setAdjustY(qreal adjustY);

    bool antialiasing() const;
    void setAntialiasing(bool antialiasing);

    Q_INVOKABLE QVariant toIcon(const QSize &requestedSize = QSize()) const;

public slots:
    void saveToFile(const QUrl &source);
    void exportToFile(const QUrl &file, QString format = QString());

signals:
    void sourceChanged();
    void colorChanged();
    void sourceDataChanged();
    void aspectRatioChanged(Qt::AspectRatioMode aspectRatio);
    void adjustXChanged(qreal adjustX);
    void adjustYChanged(qreal adjustY);
    void antialiasingChanged(bool antialiasing);

private:
    void exec(QPainter *painter, const QSizeF &requestedSize) const;
    void rasterize(QPaintDevice *device) const;
    QImage toImage(const QSize &requestedSize = QSize()) const;
    QPixmap toPixmap(const QSize &requestedSize = QSize()) const;
    QUrl mSource;
    QColor mColor;
    QPicture mPicture;
    Qt::AspectRatioMode m_aspectRatio = Qt::KeepAspectRatio;
    qreal m_adjustX = 0;
    qreal m_adjustY = 0;
    bool m_antialiasing = true;
};


#endif // QQUICKPICTURE_H
