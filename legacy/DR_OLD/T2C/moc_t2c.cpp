/****************************************************************************
** Meta object code from reading C++ file 't2c.h'
**
** Created: Thu Aug 15 15:13:46 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "t2c.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 't2c.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PVListener[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   12,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PVListener[] = {
    "PVListener\0\0idf\0valueUpdated(int)\0"
    "onUpdate(int)\0"
};

void PVListener::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PVListener *_t = static_cast<PVListener *>(_o);
        switch (_id) {
        case 0: _t->valueUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->onUpdate((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PVListener::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PVListener::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PVListener,
      qt_meta_data_PVListener, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PVListener::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PVListener::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PVListener::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PVListener))
        return static_cast<void*>(const_cast< PVListener*>(this));
    return QObject::qt_metacast(_clname);
}

int PVListener::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void PVListener::valueUpdated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_T2CManager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   14, // methods
       4,  164, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      35,   31,   11,   11, 0x0a,
      56,   31,   11,   11, 0x0a,
      75,   31,   11,   11, 0x0a,
     111,   95,   92,   11, 0x0a,
     127,   12,   92,   11, 0x2a,
     147,   95,  138,   11, 0x0a,
     166,   12,  138,   11, 0x2a,
     200,  192,  180,   11, 0x0a,
     228,  222,   11,   11, 0x0a,
     273,  255,  250,   11, 0x0a,
     323,  313,  250,   11, 0x2a,
     358,  255,  250,   11, 0x0a,
     393,  313,  250,   11, 0x2a,
     451,  433,  423,   11, 0x0a,
     475,  192,  423,   11, 0x2a,
     510,  433,  494,   11, 0x0a,
     537,  192,  494,   11, 0x2a,
     559,  192,   11,   11, 0x0a,
     587,  192,   11,   11, 0x0a,
     637,  617,   11,   11, 0x0a,
     684,  671,   11,   11, 0x2a,
     726,  718,  710,   11, 0x0a,
     744,   11,  250,   11, 0x0a,
     757,   11,  250,   11, 0x0a,
     779,   11,  771,   11, 0x0a,
     799,  790,  250,   11, 0x0a,
     818,  790,  250,   11, 0x0a,
     837,   11,   11,   11, 0x0a,
     846,   11,   11,   11, 0x08,

 // properties: name, type, flags
     863,  858, 0x03095103,
     872,  710, 0x0a095103,
     881,  250, 0x01095003,
     897,  250, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_T2CManager[] = {
    "T2CManager\0\0idf\0PVupdated(int)\0arg\0"
    "setEncoding(QString)\0setImmediate(bool)\0"
    "setRawMode(bool)\0PV\0idf,forceCached\0"
    "getPV(int,bool)\0getPV(int)\0QVariant\0"
    "getValue(int,bool)\0getValue(int)\0"
    "PVListener*\0idfList\0listenPVs(QList<int>)\0"
    "query\0sendRawQuery(QString)\0bool\0"
    "addr,port,verbose\0"
    "connectToT2C(QHostAddress,quint16,bool)\0"
    "addr,port\0connectToT2C(QHostAddress,quint16)\0"
    "connectToT2C(QString,quint16,bool)\0"
    "connectToT2C(QString,quint16)\0QList<PV>\0"
    "idfList,subscribe\0getPVs(QList<int>,bool)\0"
    "getPVs(QList<int>)\0QList<QVariant>\0"
    "getValues(QList<int>,bool)\0"
    "getValues(QList<int>)\0subscribeValues(QList<int>)\0"
    "unsubscribeValues(QList<int>)\0"
    "idf,newValue,status\0"
    "updateValue(int,QVariant,QString)\0"
    "idf,newValue\0updateValue(int,QVariant)\0"
    "QString\0timeout\0readRawReply(int)\0"
    "hasRawData()\0canReadLine()\0PVCache\0"
    "getCache()\0filename\0saveCache(QString)\0"
    "loadCache(QString)\0commit()\0readReply()\0"
    "uint\0simLevel\0encoding\0immediateUpdate\0"
    "rawMode\0"
};

void T2CManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        T2CManager *_t = static_cast<T2CManager *>(_o);
        switch (_id) {
        case 0: _t->PVupdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setEncoding((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->setImmediate((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->setRawMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: { PV _r = _t->getPV((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< PV*>(_a[0]) = _r; }  break;
        case 5: { PV _r = _t->getPV((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PV*>(_a[0]) = _r; }  break;
        case 6: { QVariant _r = _t->getValue((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 7: { QVariant _r = _t->getValue((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 8: { PVListener* _r = _t->listenPVs((*reinterpret_cast< const QList<int>(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PVListener**>(_a[0]) = _r; }  break;
        case 9: _t->sendRawQuery((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: { bool _r = _t->connectToT2C((*reinterpret_cast< QHostAddress(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: { bool _r = _t->connectToT2C((*reinterpret_cast< QHostAddress(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: { bool _r = _t->connectToT2C((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { bool _r = _t->connectToT2C((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 14: { QList<PV> _r = _t->getPVs((*reinterpret_cast< const QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QList<PV>*>(_a[0]) = _r; }  break;
        case 15: { QList<PV> _r = _t->getPVs((*reinterpret_cast< const QList<int>(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QList<PV>*>(_a[0]) = _r; }  break;
        case 16: { QList<QVariant> _r = _t->getValues((*reinterpret_cast< const QList<int>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QList<QVariant>*>(_a[0]) = _r; }  break;
        case 17: { QList<QVariant> _r = _t->getValues((*reinterpret_cast< const QList<int>(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QList<QVariant>*>(_a[0]) = _r; }  break;
        case 18: _t->subscribeValues((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 19: _t->unsubscribeValues((*reinterpret_cast< const QList<int>(*)>(_a[1]))); break;
        case 20: _t->updateValue((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 21: _t->updateValue((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 22: { QString _r = _t->readRawReply((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 23: { bool _r = _t->hasRawData();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 24: { bool _r = _t->canReadLine();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 25: { PVCache _r = _t->getCache();
            if (_a[0]) *reinterpret_cast< PVCache*>(_a[0]) = _r; }  break;
        case 26: { bool _r = _t->saveCache((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 27: { bool _r = _t->loadCache((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 28: _t->commit(); break;
        case 29: _t->readReply(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData T2CManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject T2CManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_T2CManager,
      qt_meta_data_T2CManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &T2CManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *T2CManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *T2CManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_T2CManager))
        return static_cast<void*>(const_cast< T2CManager*>(this));
    return QObject::qt_metacast(_clname);
}

int T2CManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< uint*>(_v) = simLevel(); break;
        case 1: *reinterpret_cast< QString*>(_v) = encoding(); break;
        case 2: *reinterpret_cast< bool*>(_v) = immediate(); break;
        case 3: *reinterpret_cast< bool*>(_v) = rawMode(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSimLevel(*reinterpret_cast< uint*>(_v)); break;
        case 1: setEncoding(*reinterpret_cast< QString*>(_v)); break;
        case 2: setImmediate(*reinterpret_cast< bool*>(_v)); break;
        case 3: setRawMode(*reinterpret_cast< bool*>(_v)); break;
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

// SIGNAL 0
void T2CManager::PVupdated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_T2CFactory[] = {

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

static const char qt_meta_stringdata_T2CFactory[] = {
    "T2CFactory\0"
};

void T2CFactory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData T2CFactory::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject T2CFactory::staticMetaObject = {
    { &Plugin::staticMetaObject, qt_meta_stringdata_T2CFactory,
      qt_meta_data_T2CFactory, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &T2CFactory::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *T2CFactory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *T2CFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_T2CFactory))
        return static_cast<void*>(const_cast< T2CFactory*>(this));
    if (!strcmp(_clname, "com.r0mko.testlab.CorePlugin/1.0"))
        return static_cast< Plugin*>(const_cast< T2CFactory*>(this));
    return Plugin::qt_metacast(_clname);
}

int T2CFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Plugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
