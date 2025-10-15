#include "xvfbimage.h"
#include "xvfb.h"
#include "X11/keysymdef.h"

XvfbImage::XvfbImage() : QQuickImageProvider(QQuickImageProvider::Pixmap) {
}

QPixmap XvfbImage::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    QImage img = Xvfb::instance()->getFullImage();
    if (size)
        *size = QSize(img.width(), img.height());

    int width  = requestedSize.width() > 0 ? requestedSize.width(): img.width();
    int height = requestedSize.height() > 0 ? requestedSize.height(): img.height();

    if ((width != img.width()) || (height != img.height()))
        return QPixmap::fromImage(img.scaled(width, height, Qt::IgnoreAspectRatio, Qt::FastTransformation));

    return QPixmap::fromImage(img);
}


