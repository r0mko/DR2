#include "spectrumbandplugin.h"
#include <QDebug>
#include <QPen>
#include <QPainter>
SpectrumBandFactory* SpectrumBandFactory::self = new SpectrumBandFactory();
PolyBand::PolyBand(QObject *parent)
{

}

void PolyBand::setPath(const QPainterPath &path)
{
    p.path = path;
    p.pointCache.clear();
    for(int i=0; i<p.bandSegmentCount; ++i) {
        p.pointCache.append(path.pointAtPercent((qreal)i/(qreal)p.bandSegmentCount));
    }
    update();
}


void PolyBand::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
//    pen.setJoinStyle(Qt::RoundJoin);
    for(int i = p.bandWidth-1; i>=0; --i)
    {
        qreal width = p.width*((qreal)(i+1)/p.bandWidth);
        pen.setWidthF(width);
        for(int j=0; j<p.bandSegmentCount-1; ++j) {
            qreal pos = (qreal)j/p.bandSegmentCount;
//            qreal posNext = qreal(j+1)/p.bandSegmentCount;
            pen.setColor(getColorAt(getSampleValue(i, pos)));
            painter->setPen(pen);
            //            painter->drawLine(p.path.pointAtPercent(pos), p.path.pointAtPercent(posNext));
            painter->drawLine(p.pointCache.at(j), p.pointCache.at(j+1));
        }
    }
}

bool colorStopLessThan(const QGradientStop &s1, const QGradientStop &s2)
{
    return s1.first < s2.first;
}

void PolyBand::addColorStop(qreal val, const QColor& color)
{
    p.palette.append(qMakePair(val, color));
    qSort(p.palette.begin(), p.palette.end(), colorStopLessThan); // сортируем градиент по возрастанию val
}

QColor PolyBand::getColorAt(qreal val)
{
    qreal less = 0, greater = 0;
    int lessIndex = 0, greaterIndex = 0;
    bool ok = false;
    int i=0;
    if(val>=1)
        return p.palette.last().second;
    while(!ok && i<p.palette.size()) {
        if(p.palette[i].first <= val) {
            lessIndex = i;
            less = p.palette[i].first;
        }
        if(p.palette[i].first > val) {
            greaterIndex = i;
            greater = p.palette[i].first;
        }
        i++;
        ok = (lessIndex != greaterIndex) && ((less <= val) && (val < greater));
    }
    QColor col(0, 0, 0);
    if(ok) {
        qreal delta = (val - less) / (greater - less);
        qreal r1, g1, b1, a1, r2, g2, b2, a2;
        p.palette[lessIndex].second.getRgbF(&r1, &g1, &b1, &a1);
        p.palette[greaterIndex].second.getRgbF(&r2, &g2, &b2, &a2);
        col.setRgbF((r2-r1)*delta + r1, (g2-g1)*delta + g1, (b2-b1)*delta + b1, (a2-a1)*delta + a1);
    }
    return col;
}

qreal PolyBand::getSampleValue(int harmonic, qreal pos)
{
    qreal index = (p.bandData.size()-1) * pos;
    if(harmonic > p.bandWidth)
        return 0;
    int idx = (int)index;
    if(p.smooth) {
        if(idx+1 == p.bandData.size())
            return p.bandData[idx][harmonic];
        return (p.bandData[idx+1][harmonic] - p.bandData[idx][harmonic]) * (index-idx) + p.bandData[idx][harmonic];
    }
    else
        return p.bandData[idx][harmonic];
}

QScriptValue JSCtor(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(context)
    QObject* obj = SpectrumBandFactory::instance()->newInstance();
    return engine->newQObject(obj, QScriptEngine::ScriptOwnership);
}

SpectrumBandFactory::SpectrumBandFactory()
{
    self = this;
}

int SpectrumBandFactory::init(QObject *core)
{
    c = (Core*)core;
    PolyBand::bindToJSEngine(c->engine());

    return 0;
}

QVariantHash SpectrumBandFactory::getInfo()
{
    QVariantHash i;
    i.insert("Name", "Spectrum Band graphics object");
    i.insert("Version", 100);
    i.insert("Author", "r0mko");
    return i;
}


QObject *SpectrumBandFactory::newInstance()
{
    QObject* o = new PolyBand(c);
    c->registerObject(o);
    return o;
}


void PolyBand::bindToJSEngine(QScriptEngine *e)
{
    QScriptValue metaObject = e->newQMetaObject(&PolyBand::staticMetaObject, e->newFunction(JSCtor));
    e->globalObject().setProperty("PolyBand", metaObject);
    qScriptRegisterSequenceMetaType<QVector<qreal> >(e);
    qScriptRegisterSequenceMetaType<QList<QVector<qreal> > >(e);
//    qScriptRegisterMetaType<PV>(engine, toScriptValue, fromScriptValue);
//    qScriptRegisterMetaType<PVListener*>(engine, PVListenerToSV, PVListenerFromSV);
//    qScriptRegisterMetaType<PVCache>(engine, PVCacheToSV, SVToPVCache);
//    qScriptRegisterSequenceMetaType<QList<PV> >(engine);
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2 ( SpectrumBand, SpectrumBandFactory )
#endif
