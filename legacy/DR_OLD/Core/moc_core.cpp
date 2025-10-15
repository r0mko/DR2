/****************************************************************************
** Meta object code from reading C++ file 'core.h'
**
** Created: Thu Aug 15 15:13:34 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "core.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'core.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: signature, parameters, type, tag, flags
      10,    5,    6,    5, 0x02,
      24,   19,    6,    5, 0x02,
      51,    5,   44,    5, 0x02,
      72,   70,   65,    5, 0x02,
      98,   95,   87,    5, 0x02,
     110,   19,   87,    5, 0x02,
     137,   19,    6,    5, 0x02,
     172,  165,  160,    5, 0x02,
     197,    5,    5,    5, 0x02,
     219,    5,    5,    5, 0x02,
     257,  247,  242,    5, 0x02,
     288,  283,  242,    5, 0x22,
     313,  308,  242,    5, 0x02,
     341,    5,  335,    5, 0x02,
     352,  247,  242,    5, 0x02,
     375,  283,  242,    5, 0x22,
     416,  392,    5,    5, 0x02,
     460,  448,  242,    5, 0x02,
     491,  484,  242,    5, 0x22,
     509,   95,  503,    5, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_Core[] = {
    "Core\0\0int\0coreId()\0name\0loadPlugin(QString)\0"
    "qint64\0currentMsec()\0bool\0e\0event(QEvent*)\0"
    "Plugin*\0id\0plugin(int)\0"
    "getPluginInstance(QString)\0"
    "registerEvent(QString)\0OUID\0object\0"
    "registerObject(QObject*)\0getRegisteredEvents()\0"
    "getRegisteredPlugins()\0RUID\0type,copy\0"
    "createResource(int,void*)\0type\0"
    "createResource(int)\0data\0addResource(QVariant)\0"
    "Core*\0instance()\0newResource(int,void*)\0"
    "newResource(int)\0loglevel,sender,message\0"
    "log(int,const QObject*,QString)\0"
    "typeId,copy\0newRsc(int,const void*)\0"
    "typeId\0newRsc(int)\0void*\0getRsc(RUID)\0"
};

void Core::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Core *_t = static_cast<Core *>(_o);
        switch (_id) {
        case 0: { int _r = _t->coreId();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: { int _r = _t->loadPlugin((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 2: { qint64 _r = _t->currentMsec();
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = _r; }  break;
        case 3: { bool _r = _t->event((*reinterpret_cast< QEvent*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: { Plugin* _r = _t->plugin((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Plugin**>(_a[0]) = _r; }  break;
        case 5: { Plugin* _r = _t->getPluginInstance((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Plugin**>(_a[0]) = _r; }  break;
        case 6: { int _r = _t->registerEvent((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: { OUID _r = _t->registerObject((*reinterpret_cast< QObject*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< OUID*>(_a[0]) = _r; }  break;
        case 8: _t->getRegisteredEvents(); break;
        case 9: _t->getRegisteredPlugins(); break;
        case 10: { RUID _r = _t->createResource((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< void*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< RUID*>(_a[0]) = _r; }  break;
        case 11: { RUID _r = _t->createResource((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< RUID*>(_a[0]) = _r; }  break;
        case 12: { RUID _r = _t->addResource((*reinterpret_cast< const QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< RUID*>(_a[0]) = _r; }  break;
        case 13: { Core* _r = _t->instance();
            if (_a[0]) *reinterpret_cast< Core**>(_a[0]) = _r; }  break;
        case 14: { RUID _r = _t->newResource((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< void*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< RUID*>(_a[0]) = _r; }  break;
        case 15: { RUID _r = _t->newResource((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< RUID*>(_a[0]) = _r; }  break;
        case 16: _t->log((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QObject*(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 17: { RUID _r = _t->newRsc((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const void*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< RUID*>(_a[0]) = _r; }  break;
        case 18: { RUID _r = _t->newRsc((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< RUID*>(_a[0]) = _r; }  break;
        case 19: { void* _r = _t->getRsc((*reinterpret_cast< const RUID(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< void**>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Core::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Core::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Core,
      qt_meta_data_Core, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Core::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Core::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Core::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core))
        return static_cast<void*>(const_cast< Core*>(this));
    if (!strcmp(_clname, "CoreInterface"))
        return static_cast< CoreInterface*>(const_cast< Core*>(this));
    return QObject::qt_metacast(_clname);
}

int Core::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
