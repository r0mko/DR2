/****************************************************************************
** Meta object code from reading C++ file 'spectrumbandplugin.h'
**
** Created: Thu Aug 15 15:13:50 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "spectrumbandplugin.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spectrumbandplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PolyBand[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       4,   49, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: signature, parameters, type, tag, flags
      15,   10,    9,    9, 0x02,
      42,   37,    9,    9, 0x02,
      87,   78,    9,    9, 0x02,
     129,  119,    9,    9, 0x02,
     167,  163,  156,    9, 0x02,
     204,  191,  185,    9, 0x02,
     230,    9,    9,    9, 0x02,

 // properties: name, type, flags
     247,  243, 0x02095103,
     260,  243, 0x02095103,
     273,  185, ((uint)QMetaType::QReal << 24) | 0x00095003,
     288,  283, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_PolyBand[] = {
    "PolyBand\0\0path\0setPath(QPainterPath)\0"
    "data\0setBandData(QList<QVector<qreal> >)\0"
    "spectrum\0addSpectrumStop(QVector<qreal>)\0"
    "val,color\0addColorStop(qreal,QColor)\0"
    "QColor\0val\0getColorAt(qreal)\0qreal\0"
    "harmonic,pos\0getSampleValue(int,qreal)\0"
    "updateBand()\0int\0segmentCount\0"
    "spectrumSize\0tubeWidth\0bool\0smooth\0"
};

void PolyBand::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PolyBand *_t = static_cast<PolyBand *>(_o);
        switch (_id) {
        case 0: _t->setPath((*reinterpret_cast< const QPainterPath(*)>(_a[1]))); break;
        case 1: _t->setBandData((*reinterpret_cast< const QList<QVector<qreal> >(*)>(_a[1]))); break;
        case 2: _t->addSpectrumStop((*reinterpret_cast< const QVector<qreal>(*)>(_a[1]))); break;
        case 3: _t->addColorStop((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 4: { QColor _r = _t->getColorAt((*reinterpret_cast< qreal(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QColor*>(_a[0]) = _r; }  break;
        case 5: { qreal _r = _t->getSampleValue((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qreal*>(_a[0]) = _r; }  break;
        case 6: _t->updateBand(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PolyBand::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PolyBand::staticMetaObject = {
    { &QGraphicsObject::staticMetaObject, qt_meta_stringdata_PolyBand,
      qt_meta_data_PolyBand, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PolyBand::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PolyBand::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PolyBand::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PolyBand))
        return static_cast<void*>(const_cast< PolyBand*>(this));
    return QGraphicsObject::qt_metacast(_clname);
}

int PolyBand::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = segmentCount(); break;
        case 1: *reinterpret_cast< int*>(_v) = getSpectrumSize(); break;
        case 2: *reinterpret_cast< qreal*>(_v) = getWidth(); break;
        case 3: *reinterpret_cast< bool*>(_v) = getSmooth(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSegmentCount(*reinterpret_cast< int*>(_v)); break;
        case 1: setSpectrumSize(*reinterpret_cast< int*>(_v)); break;
        case 2: setWidth(*reinterpret_cast< qreal*>(_v)); break;
        case 3: setSmooth(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
static const uint qt_meta_data_SpectrumBandFactory[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_SpectrumBandFactory[] = {
    "SpectrumBandFactory\0"
};

void SpectrumBandFactory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SpectrumBandFactory::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpectrumBandFactory::staticMetaObject = {
    { &Plugin::staticMetaObject, qt_meta_stringdata_SpectrumBandFactory,
      qt_meta_data_SpectrumBandFactory, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpectrumBandFactory::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpectrumBandFactory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpectrumBandFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpectrumBandFactory))
        return static_cast<void*>(const_cast< SpectrumBandFactory*>(this));
    if (!strcmp(_clname, "com.r0mko.testlab.CorePlugin/1.0"))
        return static_cast< Plugin*>(const_cast< SpectrumBandFactory*>(this));
    return Plugin::qt_metacast(_clname);
}

int SpectrumBandFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Plugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
