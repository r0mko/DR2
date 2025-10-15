#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QColor>
#include <QQmlContext>
#include <QObject>
#include <QDir>
#include "private/qmetaobjectbuilder_p.h"
#include "private/qmetaobject_p.h"
#include "cxxabi.h"
#include "handle.h"
#include "qqml.h"
#include <QPluginLoader>
typedef void (*MetaCallFn)(QObject *, QMetaObject::Call, int, void **);

template<class T>
struct HandlerData
{
    MetaCallFn metaCall = nullptr;
    QMetaType::Deleter del = nullptr;
    QMetaType::Creator create = &Create;
    QMetaType::Destructor dtor = QtMetaTypePrivate::QMetaTypeFunctionHelper<T*>::Destruct;
    QMetaType::Constructor ctor = &Construct;
    int size = sizeof(T);
    static QMetaObject *  metaObject;

    static void *Create(const void *t);
    static void *Construct(void *where, const void *t);

};
template<class T>
void *HandlerData<T>::Create(const void *t)
{
    T *ret;
    qDebug() << "Creating" << metaObject->className();
    if (t)
        ret = new T(*static_cast<const T*>(t));
    ret = new T();
    ret->metaObject = metaObject;
    return ret;
}

template<class T>
void *HandlerData<T>::Construct(void *where, const void *t)
{
    T *ret;
    qDebug() << "Constructing"  << metaObject->className();
    if (t)
        ret = new (where) T(*static_cast<const T*>(t));
    ret = new (where) T();
    ret->metaObject = metaObject;
    return ret;
}

template<class T>
QMetaObject * HandlerData<T>::metaObject = nullptr;

static inline const QMetaObjectPrivate *priv(const uint* data)
{ return reinterpret_cast<const QMetaObjectPrivate*>(data); }


int main(int argc, char *argv[])
{
//    qSetMessagePattern("%{if-warning}Warning:%{endif}%{if-critical}Critical:%{endif}%{function} - %{message}");
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    // @uri Playground
    const char *uri = "Playground";

    QDir dir(ROOTPATH);
    dir.cd("editor");
    QDir::setCurrent(dir.path());
//    qmlRegisterType<HandleType>(uri, 1, 0, "HandleTest");
    engine.load(QUrl(QString("%1/editor/main.qml").arg(ROOTPATH)));
    return app.exec();
}
