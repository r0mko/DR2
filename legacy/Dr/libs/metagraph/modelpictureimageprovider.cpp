#include "modelpictureimageprovider.h"
#include "graphmodel.h"
#include <QBuffer>
#include <QDebug>
#include <QPainter>
#include <QPicture>
#include <QUrlQuery>
#include <iostream>
#include <QString>

ModelPictureImageProvider::ModelPictureImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Pixmap),
    picture_t(BlobFactory::typeHash("picture"))
{

}

QPixmap ModelPictureImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QString modelId = id.section('/', 0, 0);
    QStringList qelements = id.section('/', 1).split("?");

    QString mrl = qelements.first();
    QUrlQuery query;
    QVariantMap params;
    if (qelements.size() == 2) {
        query.setQuery(qelements.at(1));
        for (auto p : query.queryItems())
            params.insert(p.first, p.second);
    }
    QSize targetSize;
    if (!requestedSize.isValid()) {
        if (params.contains("size"))
            targetSize = parseSize(params["size"].toString());
    } else
        targetSize = requestedSize;

    GraphModel *model = GraphModel::graphModels.value(modelId);
    if (!model) {
        qWarning() << qPrintable(QString("Model %1 not found!").arg(modelId));
        return QPixmap();
    }

    auto n = model->getNode(mrl);
    if (!n) {
        qWarning() << qPrintable(QString("Blob mrl:/%2 not found in model %1").arg(modelId).arg(mrl));
        return QPixmap();
    }
    if (n->typeId() != qMetaTypeId<Blob>()) {
        qWarning() << qPrintable(QString("image must be Blob type, but provided mrl %1 has type %2").arg(mrl).arg(QString(QMetaType::typeName(n->typeId()))));
        return QPixmap();
    }
    Blob *b = static_cast<Blob*>(n);
    if (b->type == picture_t) {
        QColor colorize;
        if (params.contains("color"))
            colorize = parseColor(params["color"].toString());
        bool antialias = true;
        if (params.contains("noaa"))
            antialias = false;
        QPixmap pix = rasterizePictureBlob(b->rawdata, targetSize, colorize, antialias, Qt::KeepAspectRatio);
        if (size)
            *size = pix.size();
        return pix;
    } else {
        qWarning() << qPrintable(QString("Can not load pictures from blob type %1. Blob must be of type \"picture\"").arg(BlobFactory::resolveBlobType(b->type)));
    }
    return QPixmap();
}
//extern void rasterizeQPicture(QPainter *painter, const QSizeF &requestedSize, const QPicture &picture, QColor colorize, bool antialiasing = true, Qt::AspectRatioMode aspectRatio = Qt::KeepAspectRatio, qreal adjustX = 0, qreal adjustY = 0);
QPixmap ModelPictureImageProvider::rasterizePictureBlob(QByteArray rawQPicture, QSize requestedSize, QColor mColor, bool antialiasing, Qt::AspectRatioMode aspectRatioMode)
{
    QPicture mPicture;
    QBuffer buffer( &rawQPicture );
    buffer.open( QIODevice::ReadOnly );
    bool res = mPicture.load( &buffer );
    buffer.close();
    if (!res) {
//        qWarning() << "Failed to load QPicture";
        return QPixmap();
    }
    const QSizeF srcSize = QSizeF(mPicture.boundingRect().size());
    QSize size;
    if (requestedSize.isValid())
        size = requestedSize;
    else
        size = mPicture.boundingRect().size();
    QSizeF trueSize = srcSize.scaled(size, aspectRatioMode);
    qreal hmargin = (size.width() - trueSize.width()) / 2;
    qreal vmargin = (size.height() - trueSize.height()) / 2;
    QPainter painter;
    if (size.width() <= 0 || size.height() <= 0) {
        return QPixmap(QSize(1, 1));
    }
    QPixmap img(size);

    img.fill(Qt::transparent);
    painter.begin(&img);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing, antialiasing);
    painter.translate(hmargin, vmargin);
    painter.scale(trueSize.width() / srcSize.width(), trueSize.height() / srcSize.height());
    painter.drawPicture(0, 0, mPicture);
    painter.restore();
    if (mColor.isValid()) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(0, 0, size.width(), size.height(), mColor);
    }
    painter.end();
    return img;
}

QSize ModelPictureImageProvider::parseSize(const QString &sizeString) const
{
    QStringList pars = sizeString.split("x");
    if (pars.size() != 2)
        return QSize();
    bool ok = false;
    int w = pars.first().toInt(&ok);
    ok = false;
    int h = pars.last().toInt(&ok);
    if (!ok)
        return QSize();
    return QSize(w, h);
}

QColor ModelPictureImageProvider::parseColor(const QString &colorString) const
{
    return QColor(colorString);
}
