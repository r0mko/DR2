#ifndef MODELPICTUREIMAGEPROVIDER_H
#define MODELPICTUREIMAGEPROVIDER_H

#include <QQuickImageProvider>

class ModelPictureImageProvider : public QQuickImageProvider
{
    const int picture_t;
public:
    ModelPictureImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    static QPixmap rasterizePictureBlob(QByteArray pictureData, QSize targetSize = QSize(), QColor colorize = QColor(), bool antialiasing = true, Qt::AspectRatioMode = Qt::KeepAspectRatio);
private:
    QSize parseSize(const QString &sizeString) const;
    QColor parseColor(const QString &colorString) const;
};





#endif // MODELPICTUREIMAGEPROVIDER_H
