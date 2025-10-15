#ifndef UIINTERFACE
#define UIINTERFACE
#include <QColor>
#include <QSize>
#include <qpixmap.h>


class UIInterface
{
public:
    virtual QPixmap rasterizePicture(QByteArray pictureData, QSize targetSize = QSize(), QColor colorize = QColor()) = 0;
};


#endif // UIINTERFACE

