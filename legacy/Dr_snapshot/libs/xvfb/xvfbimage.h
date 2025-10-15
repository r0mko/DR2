#ifndef XVFBIMAGE_H
#define XVFBIMAGE_H

#include <QQuickImageProvider>

class XvfbImage : public QQuickImageProvider
{
public:
    explicit XvfbImage();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

signals:

public slots:
private:

};

#endif // XVFBIMAGE_H
