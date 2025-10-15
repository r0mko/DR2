#include "font2qpicture.h"
#include <QBuffer>
#include <QDebug>
#include <QMetaEnum>
#include <QPainter>
#include <QPicture>
#include <QTemporaryFile>

Font2QPicture::Font2QPicture(QObject *parent) :
    QObject(parent)
{
//    connect(this, &Font2QPicture::fontFileChanged, this, &Font2QPicture::loadFont, Qt::QueuedConnection);
}

void Font2QPicture::componentComplete()
{
    complete = true;
    if (!m_fontFile.isEmpty() && m_font.isValid())
        loadFont();
}

QUrl Font2QPicture::fontFile() const
{
    return m_fontFile;
}

int Font2QPicture::pixelSize() const
{
    return m_pixelSize;
}

QString Font2QPicture::familyName() const
{
    return m_font.familyName();
}

QString Font2QPicture::styleName() const
{
    return m_font.styleName();
}

QUrl Font2QPicture::outputFolder() const
{
    return m_outputFolder;
}

int Font2QPicture::glyphCount() const
{
    QByteArray data = m_font.fontTable("loca");
    return (data.size() >> 1) - 1;
}

void Font2QPicture::setFontFile(QUrl fontFile)
{
    if (m_fontFile.isValid())
        return;
    m_fontFile = fontFile;
    if (!m_font.isValid())
        loadFont();
    emit fontFileChanged(fontFile);
}

void Font2QPicture::setPixelSize(int pixelSize)
{
    if (m_pixelSize == pixelSize)
        return;
    m_pixelSize = pixelSize;
    emit pixelSizeChanged(pixelSize);
}


void Font2QPicture::setOutputFolder(QUrl outputFolder)
{
    if (m_outputFolder == outputFolder)
        return;

    m_outputFolder = outputFolder;
    emit outputFolderChanged(outputFolder);
}

QRectF Font2QPicture::maxRect() const
{
    return m_maxRect;
}

bool Font2QPicture::isValid() const
{
    return m_font.isValid();
}

bool Font2QPicture::createPicForGlyph(quint32 index) const
{
    QString fileName;
    if (!m_outputFolder.isValid()) {
        qWarning() << "No output folder set!";
        return false;
    }
    QString name = QString("%1%2").arg(m_font.familyName()).arg(index);
    fileName = m_outputFolder.toLocalFile() + QString("/%1.pic").arg(name);
    QPainterPath path = m_font.pathForGlyph(index);
    if (path.isEmpty()) {
        qWarning() << "Invalid glyph index" << index;
        return false;
    }
    path.translate(-path.boundingRect().topLeft());
    QRectF src = path.boundingRect();
    QRectF dst(0, 0, m_pixelSize, m_pixelSize);
    QPicture pic;
    QPainter p;
    qreal sc = qMin(m_pixelSize / src.height(), m_pixelSize / src.width());
    if (sc > 1)
        sc = 1;
    p.begin(&pic);
    p.scale(sc, sc);
    p.setRenderHint(QPainter::Antialiasing);
//    pic.setBoundingRect(src.marginsAdded(QMarginsF(1,1,1,1)).toRect());
    p.fillPath(path, Qt::black);
    p.end();
    pic.save(fileName);
    qDebug() << index << src << sc << pic.boundingRect();
    emit pictureSaved(index, name, QUrl::fromLocalFile(fileName));
    return true;
}

QByteArray Font2QPicture::getGlyphAsPicture( int index ) const
{
    QPainterPath path = m_font.pathForGlyph(index);
    if (path.isEmpty()) {
        qWarning() << "empty glyph index" << index;
    }
    path.translate(-path.boundingRect().topLeft() + QPointF(0.5, 0.5));
    QRectF src = path.boundingRect().marginsAdded(QMarginsF(2, 2, 2, 2));
    QPicture pic;
    pic.setBoundingRect(src.toRect());
    QPainter p;
    p.begin(&pic);
    p.fillPath(path, Qt::black);
    p.end();
    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);
    pic.save(&buf);
    buf.close();
    return ba;
}

QByteArray Font2QPicture::getGlyphAsPath( int index ) const
{
    QPainterPath path = m_font.pathForGlyph(index);
    if (path.isEmpty()) {
        qWarning() << "empty glyph index" << index;
    }
    path.translate(-path.boundingRect().topLeft() + QPointF(0.5, 0.5));

    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds << path;

    return ba;
}

QString Font2QPicture::getGlyphName(int index) const
{
    return QString{ m_glyphNames.at( index ) };
}

void Font2QPicture::scan()
{
    int c = 0;
    for (int index = 0; index < glyphCount(); ++index) {
        QPainterPath path = m_font.pathForGlyph(index);
        if (path.isEmpty()) {
            qWarning() << "empty glyph index" << index;
            continue;
        }
        path.translate(-path.boundingRect().topLeft() + QPointF(0.5,0.5));
        QByteArray bap;
        QDataStream ds(&bap, QIODevice::WriteOnly);
        ds << path;

        QRectF src = path.boundingRect().marginsAdded(QMarginsF(2, 2, 2, 2));
//        if (m_maxRect.isNull())
//            m_maxRect = src;
//        else
//            m_maxRect = m_maxRect.united(src);
        QPicture pic;
        pic.setBoundingRect(src.toRect());
        QPainter p;
        p.begin(&pic);
//        p.setRenderHints(QPainter::Antialiasing, m_);
        p.fillPath(path, Qt::black);
        p.end();
        QByteArray ba;
        QBuffer buf(&ba);
        buf.open(QIODevice::WriteOnly);
        pic.save(&buf);
        buf.close();

        if (m_glyphNames.size() > c){
            emit iconBlobAdded(index, QString(m_glyphNames.at(c++)), ba, bap);
        }else{
            emit iconBlobAdded(index, m_fontFile.fileName().section('.', 0, -2), ba, bap);
        }
    }
}

void Font2QPicture::measure()
{
    QRectF oldrect = m_maxRect;
    m_maxRect = QRectF();
    for (int i = 0; i < glyphCount(); ++i) {
        QPainterPath p = m_font.pathForGlyph(i);
        if (p.isEmpty())
            continue;
        QRectF brct = p.boundingRect();
        brct.moveTopLeft(-brct.topLeft());
        if (m_maxRect.isNull())
            m_maxRect = brct;
        else
            m_maxRect = m_maxRect.united(brct);
    }
    if (oldrect == m_maxRect)
        return;
    m_scaleFactor = qMax(m_pixelSize / m_maxRect.width(), m_pixelSize / m_maxRect.height());
    emit maxRectChanged();
    emit scaleFactorChanged();
//    qDebug() << "Max width" << m_maxRect.width() << "max height" << m_maxRect.height() << "scale factor" << m_scaleFactor;
}

qreal Font2QPicture::scaleFactor() const
{
    return m_scaleFactor;
}

bool Font2QPicture::antialiasing() const
{
    return m_antialiasing;
}

void Font2QPicture::setAntialiasing(bool antialiasing)
{
    if (m_antialiasing == antialiasing)
        return;

    m_antialiasing = antialiasing;
    emit antialiasingChanged(antialiasing);
}

void Font2QPicture::loadFont()
{
    if (m_fontFile.isEmpty())
        return;
    m_font = QRawFont(m_fontFile.toLocalFile(), m_pixelSize);
    if (!m_font.isValid()) {
        qWarning() << "error loading font" << m_fontFile.toLocalFile();
        emit isValidChanged();
        return;
    }
    readPost();
    measure();
    emit styleNameChanged();
    emit familyNameChanged();
    emit glyphCountChanged();
    emit isValidChanged();
//    qDebug() << "Font loaded; complete =" << complete;
}

void Font2QPicture::readCmap()
{
    QByteArray data = m_font.fontTable("cmap");
    QDataStream in(data);
    in.setByteOrder(QDataStream::BigEndian);
    quint16 version;
    quint16 numberSubtables;

    in >> version >> numberSubtables;
    qDebug() << "version:" << version;
    qDebug() << "numberSubtables:" << numberSubtables;
    int i = 1;
    quint16 platformSpecificID;         // Platform-specific encoding identifier
    while (numberSubtables--) {
        quint16 platformID_i;
        quint32 offset;                     // Offset of the mapping table
        in >> platformID_i >> platformSpecificID >> offset;
        qDebug() << "subtable" << i++;
//        qDebug() << "platformID:" << QMetaEnum::fromType<PlatformID>().key(platformID_i) << platformID_i;
        qDebug() << "platformSpecificID:" << platformSpecificID;
        qDebug() << "offset:" << offset;
        qDebug() << "pos:" << in.device()->pos();
        if (platformID_i == Unicode) {
            qDebug() << "jumping to offset" << offset;
            numberSubtables = 0;
            in.skipRawData(offset - in.device()->pos());
            break;
        }
    }
    qDebug() << "pos" << in.device()->pos() << platformSpecificID;
    quint16 format;
    in >> format;
    qDebug() << "table format" << format;
    if (format == 4) {
        quint16 length; // Length of subtable in bytes
        quint16 language; // Language code (see above)
        quint16 segCountX2; // 2 * segCount
        quint16 searchRange; // 2 * (2**FLOOR(log2(segCount)))
        quint16 entrySelector; // log2(searchRange/2)
        quint16 rangeShift; // (2 * segCount) - searchRange
        in >> length >> language >> segCountX2 >> searchRange >> entrySelector >> rangeShift;
        qDebug() << "length:" << length;
        qDebug() << "language:" << language;
        qDebug() << "segCount:" << (segCountX2 >> 1);
        qDebug() << "searchRange:" << searchRange;
        qDebug() << "entrySelector:" << entrySelector;
        qDebug() << "rangeShift:" << rangeShift;
        int segCount = segCountX2 >> 1;
        while (segCount--) {
            quint16 endCode; // [segCount]	Ending character code for each segment, last = 0xFFFF.
            in >> endCode;
            qDebug() << "seg" << (segCountX2 >> 1) - segCount << "endCode:" << endCode;
        }

        quint16 reservedPad; // This value should be zero
        in >> reservedPad;
        qDebug() << "reservedPad:" << reservedPad;
        segCount = segCountX2 >> 1;
        while (segCount--) {
            quint16 startCode; // [segCount]	Starting character code for each segment
            in >> startCode;
            qDebug() << "seg" << (segCountX2 >> 1) - segCount << "startCode:" << startCode;
        }
        segCount = segCountX2 >> 1;
        while (segCount--) {
            quint16 idDelta; // [segCount]	Delta for all character codes in segment
            in >> idDelta;
            qDebug() << "seg" << (segCountX2 >> 1) - segCount << "idDelta:" << idDelta;
        }
        segCount = segCountX2 >> 1;
        while (segCount--) {
            quint16 idRangeOffset; // [segCount]	Offset in bytes to glyph indexArray, or 0
            in >> idRangeOffset;
            qDebug() << "seg" << (segCountX2 >> 1) - segCount << "idRangeOffset:" << idRangeOffset;
        }

        qDebug() << data.size() - in.device()->pos();
        int id = 0;
        while (!in.atEnd()) {
            quint16 glyphIndex; // [variable]	Glyph index array
            in >> glyphIndex;
            qDebug() << id++ << "glyphIndex" << glyphIndex;
        }
    }
    //    }

}

void Font2QPicture::readPost()
{
    m_glyphNames.clear();
    QByteArray data = m_font.fontTable("post");
    QDataStream in(data);
    in.setByteOrder(QDataStream::BigEndian);
    quint32 ver;
    quint32	italicAngle; // Italic angle in degrees
    qint16	underlinePosition; // 	Underline position
    qint16	underlineThickness; // 	Underline thickness
    quint32	isFixedPitch; // 	Font is monospaced; set to 1 if the font is monospaced and 0 otherwise (N.B., to maintain compatibility with older versions of the TrueType spec, accept any non-zero value as meaning that the font is monospaced)
    quint32	minMemType42; // 	Minimum memory usage when a TrueType font is downloaded as a Type 42 font
    quint32	maxMemType42; // 	Maximum memory usage when a TrueType font is downloaded as a Type 42 font
    quint32	minMemType1; // 	Minimum memory usage when a TrueType font is downloaded as a Type 1 font
    quint32	maxMemType1; // 	Maximum memory usage when a TrueType font is downloaded as a Type 1 font
    in >> ver >> italicAngle >> underlinePosition >> underlineThickness >> isFixedPitch >> minMemType42 >> maxMemType42 >> minMemType1 >> maxMemType1;
    switch (ver >> 16) {
    case 2: {
        quint16 numberOfGlyphs; // 	number of glyphs
        in >> numberOfGlyphs;
        int c = 0;
        while (c++ < numberOfGlyphs) {
            quint16 glyphNameIndex;
            in >> glyphNameIndex;
            if (glyphNameIndex > 257 && glyphNameIndex < 32768) {
                glyphNameIndex -= 258;
            }
        }
        c = 0;
        while (c++ < numberOfGlyphs) {
            quint8 size;
            in >> size;
            QByteArray name;
            name.resize(size);
            in.readRawData(name.data(), size);
            m_glyphNames.append(name);
        }
        break;
    }
    default:
        qWarning() << "unable to read 'post' table format" << (ver >> 16);
        return;
        break;
    }
}

void Font2QPicture::readLoca()
{
    QByteArray data = m_font.fontTable("loca");
    qDebug() << "LOCA size" << data.size();
    QDataStream in(data);
    in.setByteOrder(QDataStream::BigEndian);
    int i = 0;
    while (!in.atEnd()) {
        quint16 offset;
        in >> offset;
//        qDebug() << "glyph" << i++ << "offset" << offset;
    }
}


