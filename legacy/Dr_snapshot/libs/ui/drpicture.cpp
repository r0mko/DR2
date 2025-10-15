/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Extras module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "drpicture.h"

#include <QQmlFile>
#include <QDebug>
#include <QImageWriter>
#include <QIcon>

DRPicture::DRPicture(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
//    setAntialiasing(true);
//    setSmooth(false);
    const qreal defaultFontHeight = QFontMetricsF(QFont()).height();
    setImplicitWidth(defaultFontHeight * 4);
    setImplicitHeight(defaultFontHeight * 4);
}

DRPicture::~DRPicture()
{
}

void DRPicture::paint(QPainter *painter)
{
//    painter->setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing, m_antialiasing);

    exec(painter, boundingRect().size());
////    qDebug() << m_adjustX << m_adjuxtY;
//    if (mColor.isValid()) {
//        painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
//        painter->fillRect(0, 0, dstSize.width(), dstSize.height(), mColor);
//    }
}


QUrl DRPicture::source() const
{
    return mSource;
}

void DRPicture::setSource(const QUrl &source)
{
    if (mSource != source) {
        mSource = source;
        const QString fileName = QQmlFile::urlToLocalFileOrQrc(source);
        if (!mPicture.load(fileName)) {
            qWarning().nospace() << "Failed to load " << fileName << "; does it exist?";
            mPicture = QPicture();
        }

        setImplicitWidth(mPicture.boundingRect().width());
        setImplicitHeight(mPicture.boundingRect().height());

        update();
        emit sourceChanged();
    }
}

/*!
    \qmlproperty color Picture::color

    This property specifies the color of the indicator.

    The default value is \c "black".
*/
QColor DRPicture::color() const
{
    return mColor;
}

void DRPicture::setColor(const QColor &color)
{
    if (mColor != color) {
        mColor = color;
        update();
        emit colorChanged();
    }
}

void DRPicture::resetColor()
{
    setColor(QColor());
}

void DRPicture::setSourceData(QByteArray data)
{

    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadOnly );
    bool res = mPicture.load( &buffer );
    buffer.close();
    if (!res)
        qWarning() << "Failed to load QPicture" << data.size();
    setImplicitWidth(mPicture.boundingRect().width());
    setImplicitHeight(mPicture.boundingRect().height());
    update();
    emit sourceDataChanged();
}

Qt::AspectRatioMode DRPicture::aspectRatio() const
{
    return m_aspectRatio;
}

void DRPicture::setAspectRatio(Qt::AspectRatioMode aspectRatio)
{
    if (m_aspectRatio == aspectRatio)
        return;

    m_aspectRatio = aspectRatio;
    emit aspectRatioChanged(aspectRatio);
}

qreal DRPicture::adjustX() const
{
    return m_adjustX;
}

qreal DRPicture::adjustY() const
{
    return m_adjustY;
}

void DRPicture::setAdjustX(qreal adjustX)
{
    if (m_adjustX == adjustX)
        return;

    m_adjustX = adjustX;
    emit adjustXChanged(adjustX);
}

void DRPicture::setAdjustY(qreal adjuxtY)
{
    if (m_adjustY == adjuxtY)
        return;

    m_adjustY = adjuxtY;
    emit adjustYChanged(adjuxtY);
}

bool DRPicture::antialiasing() const
{
    return m_antialiasing;
}

QByteArray DRPicture::sourceData()
{
    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::WriteOnly );
    bool res = mPicture.save( &buffer );
    buffer.close();
    if (!res)
        qWarning() << "Failed to save QPicture";
    return data;
}

void DRPicture::saveToFile(const QUrl &source)
{
    if (!mPicture.save(source.toLocalFile()))
        qWarning() << "Error saving picture to file" << source;
}

void DRPicture::exportToFile(const QUrl &file, QString format)
{
    QImageWriter writer(QQmlFile::urlToLocalFileOrQrc(file), format.toLocal8Bit());
    if (!writer.canWrite()) {
        qWarning() << "unable to save image to" << file.fileName() << "format" << format;
        return;
    }

    if (!writer.write(toImage()))
        qWarning() << "Failed to write image:" << writer.errorString();

}

void DRPicture::exec(QPainter *painter, const QSizeF &requestedSize) const
{
    const QSizeF srcSize = QSizeF(mPicture.boundingRect().size());
    QSizeF trueSize = srcSize.scaled(requestedSize, m_aspectRatio);
    qreal hmargin = (requestedSize.width() - trueSize.width()) / 2;
    qreal vmargin = (requestedSize.height() - trueSize.height()) / 2;
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing, m_antialiasing);
    painter->translate(hmargin, vmargin);
    painter->translate(m_adjustX / 64, m_adjustY / 64);
    painter->scale(trueSize.width() / srcSize.width(), trueSize.height() / srcSize.height());
    painter->drawPicture(0, 0, mPicture);
    painter->restore();
    if (mColor.isValid()) {
        painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter->fillRect(0, 0, requestedSize.width(), requestedSize.height(), mColor);
    }
}



void DRPicture::rasterize(QPaintDevice *device) const
{
    QPainter painter;
    painter.begin(device);
    exec(&painter, QSizeF(device->width(), device->height()));
    painter.end();
}

void DRPicture::setAntialiasing(bool antialiasing)
{
    if (m_antialiasing == antialiasing)
        return;
    QQuickPaintedItem::setAntialiasing(false);
    m_antialiasing = antialiasing;
    update();
    //    qDebug() << "AA changed";
    emit antialiasingChanged(antialiasing);
}

QVariant DRPicture::toIcon(const QSize &requestedSize) const
{
    QIcon ico(toPixmap(requestedSize));
    return ico;
}

QImage DRPicture::toImage(const QSize &requestedSize) const
{
    QSize size;
    if (requestedSize.isValid())
        size = requestedSize;
    else
        size = mPicture.boundingRect().size();
    QImage img(size, QImage::Format_RGBA8888_Premultiplied);
    img.fill(Qt::transparent);
    rasterize(&img);
    return img;
}

QPixmap DRPicture::toPixmap(const QSize &requestedSize) const
{
    QSize size;
    if (requestedSize.isValid())
        size = requestedSize;
    else
        size = mPicture.boundingRect().size();
    QPixmap img(size);
    img.fill(Qt::transparent);
    rasterize(&img);
    return img;
}




