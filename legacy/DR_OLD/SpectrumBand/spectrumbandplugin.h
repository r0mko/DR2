#ifndef PLUGINTEMPLATE_H
#define PLUGINTEMPLATE_H
#include <QGraphicsObject>
#include <QGradientStops>
#include "../Core/plugininterface.h"
#include "../Core/core.h"
#include "spectrumbandplugin_global.h"
#define LOG_CORE(x) UF2DeviceFactory::instance()->core()->log(0, UF2DeviceFactory::instance(), x)

class PolyBand : public QGraphicsObject
{
    Q_OBJECT
    struct PolyBandPrivate
    {
        QPainterPath path; // собственно путь
        QList<QVector<qreal> > bandData; // массив срезов гармоник
        int bandWidth; // количество гармоник
        int bandSegmentCount; // количество отрезков, которыми строится путь
        qreal width; // ширина ленты
        QGradientStops palette; // градиент для раскраски значений
        bool smooth; // флаг сглаживания значений вдоль пути
        QPolygonF pointCache;
        // ---
        PolyBandPrivate() : bandWidth(64), bandSegmentCount(100) {}
    };
    PolyBandPrivate p;
    Q_PROPERTY (int segmentCount READ segmentCount WRITE setSegmentCount)
    Q_PROPERTY (int spectrumSize READ getSpectrumSize WRITE setSpectrumSize)
    Q_PROPERTY (qreal tubeWidth READ getWidth WRITE setWidth)
    Q_PROPERTY (bool smooth READ getSmooth WRITE setSmooth)

public:
    explicit PolyBand(QObject *parent = 0);
    Q_INVOKABLE void setPath (const QPainterPath& path);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    Q_INVOKABLE void setBandData(const QList<QVector<qreal> > &data) { p.bandData = data; update(); }
    Q_INVOKABLE void addSpectrumStop(const QVector<qreal> &spectrum) { p.bandData.append(spectrum); update(); } // добавляет срез спектра в массив
    QRectF boundingRect() const { return p.path.controlPointRect().adjusted(-p.width,-p.width, p.width, p.width); }
    Q_INVOKABLE void addColorStop(qreal val, const QColor &color); // добавляет в градиент раскраски новую отметку. Диапазон val считается от 0 до 1
    Q_INVOKABLE QColor getColorAt(qreal val); // возвращает цвет из градиента раскраски в произвольной его точке
    Q_INVOKABLE qreal getSampleValue(int harmonic, qreal pos); // возвращает значение гармоники harmonic в произвольной точке, считая весь массив гармоник за диапазон [0.0 ... 1.0]
    Q_INVOKABLE void updateBand() { update(); }

    // Q_PROPERTY getters/setters
    int segmentCount() const { return p.bandSegmentCount; }
    void setSegmentCount(int arg) {p.bandSegmentCount = arg; }
    bool getSmooth() const { return p.smooth; }
    void setSmooth(bool arg) { p.smooth = arg; update(); }
    int getSpectrumSize() const { return p.bandWidth; }
    void setSpectrumSize(int arg) { p.bandWidth = arg; }
    qreal getWidth() const { return p.width; }
    void setWidth(qreal arg) { p.width = arg; }
    static void bindToJSEngine(QScriptEngine* e);

signals:

public slots:

private:

    qreal m_width;
};



class SpectrumBandFactory : public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID "org.PGE.DataReactor.UF2Device")
#endif
    Core *c;
    static SpectrumBandFactory* self;
public:
    SpectrumBandFactory();
    int init(QObject *core);
    QVariantHash getInfo();
    QObject* newInstance();
    Core* core() const { return c; }
    static SpectrumBandFactory* instance() { return self; }
private:
};

Q_DECLARE_METATYPE(QVector<qreal>)
Q_DECLARE_METATYPE(QList<QVector<qreal> >)



#endif // PLUGINTEMPLATE_H
