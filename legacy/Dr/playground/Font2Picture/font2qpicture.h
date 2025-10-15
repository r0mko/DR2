#ifndef FONT2QPICTURE_H
#define FONT2QPICTURE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QRawFont>
#include <QUrl>
#include <QJSValue>
#include <QVariant>

class Picture
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QUrl source READ source)
    Q_PROPERTY(QRectF boundingRect READ boundingRect)
    Q_PROPERTY(quint32 glyphIndex READ glyphIndex)
    Q_PROPERTY(QUrl fontSource READ fontSource)

    QString m_name;
    QUrl m_source;
    QRectF m_boundingRect;
    quint32 m_glyphIndex;
    QUrl m_fontSource;

public:
    Picture() {}

    inline QString name() const { return m_name; }
    inline QUrl source() const { return m_source; }
    inline QRectF boundingRect() const { return m_boundingRect; }
    inline quint32 glyphIndex() const { return m_glyphIndex; }
    inline QUrl fontSource() const { return m_fontSource; }

    inline void setName(QString name) { m_name = name; }
};

Q_DECLARE_METATYPE(Picture)

class Font2QPicture : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QUrl fontFile READ fontFile WRITE setFontFile NOTIFY fontFileChanged)
    Q_PROPERTY(qreal pixelSize READ pixelSize WRITE setPixelSize NOTIFY pixelSizeChanged)
    Q_PROPERTY(QString familyName READ familyName NOTIFY familyNameChanged)
    Q_PROPERTY(QString styleName READ styleName NOTIFY styleNameChanged)
    Q_PROPERTY(QUrl outputFolder READ outputFolder WRITE setOutputFolder NOTIFY outputFolderChanged)
    Q_PROPERTY(QRectF maxRect READ maxRect NOTIFY maxRectChanged)
    Q_PROPERTY(qreal scaleFactor READ scaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(bool antialiasing READ antialiasing WRITE setAntialiasing NOTIFY antialiasingChanged)
//    Q_PROPERTY(QVariantList icons READ icons NOTIFY iconsChanged)

    Q_PROPERTY(int glyphCount READ glyphCount NOTIFY glyphCountChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)


    QRawFont m_font;
    QList<QByteArray> m_glyphNames;
    QUrl m_fontFile;
    qreal m_pixelSize = 16;
    QUrl m_outputFolder;
    bool complete = false;
    QRectF m_maxRect;
    qreal m_scaleFactor;
public:
    enum PlatformID {
        Unicode = 0,        // Indicates Unicode version.
        Macintosh = 1,      // Script Manager code.
        reserved = 2,       // do not use
        Microsoft = 3       // Microsoft encoding.
    };
//    Q_ENUM(PlatformID)

    explicit Font2QPicture(QObject *parent = 0);

    void classBegin() {}
    void componentComplete();

    QUrl fontFile() const;
    int pixelSize() const;
    QString familyName() const;
    QString styleName() const;
    QUrl outputFolder() const;
    int glyphCount() const;
    bool isValid() const;

    void setFontFile(QUrl fontFile);
    void setPixelSize(int pixelSize);
    void setOutputFolder(QUrl outputFolder);

    QRectF maxRect() const;
    void measure();

    qreal scaleFactor() const;

    bool antialiasing() const;
    void setAntialiasing(bool antialiasing);

public slots:
    bool createPicForGlyph(quint32 index) const;

    QByteArray getGlyphAsPicture(int index) const;
    QByteArray getGlyphAsPath(int index) const;
    QString getGlyphName(int index) const;

    void scan();

signals:
    void fontFileChanged(QUrl fontFile);
    void pixelSizeChanged(qreal pixelSize);
    void familyNameChanged();
    void styleNameChanged();
    void glyphCountChanged();
    void outputFolderChanged(QUrl outputFolder);
    void isValidChanged();
    void pictureSaved(int glyphIndex, QString name, QUrl fileName) const;
    void iconBlobAdded(int glyphIndex, QString name, QByteArray blob, QByteArray blobpath) const;

    void maxRectChanged();

    void scaleFactorChanged();

    void antialiasingChanged(bool antialiasing);

private:
    void loadFont();
    void readCmap();
    void readLoca();
    void readPost();



    bool m_antialiasing = true;
};

#endif // FONT2QPICTURE_H
