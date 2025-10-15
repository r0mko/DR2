#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlContext>
#include <QObject>
#include <QRectF>
#include <QDir>
#include "qqml.h"
#include "private/qqmlmetatype_p.h"
int main(int argc, char *argv[])
{
    //    qSetMessagePattern("%{if-warning}Warning:%{endif}%{if-critical}Critical:%{endif}%{function} - %{message}");
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QRectF r1;
    QRectF r2(2, 2, 8, 11);
    qDebug() << r1.united(r2);
    // @uri Test
    //    const char *uri = "Test";
    QDir dir(ROOTPATH);
    dir.cd("editor");
    QDir::setCurrent(dir.path());
        engine.load(QUrl(QString("%1/viewer/main.qml").arg(ROOTPATH)));
//    engine.load(QUrl(QString("%1/viewer/test.qml").arg(ROOTPATH)));
//    auto t = QQmlMetaType::qmlTypes();
//    for (QQmlType *i : t) {
//        auto d = qDebug();
//        d << i->sourceUrl();
//        d << i->elementName() << i->typeName();
//        if (i->metaObject())
//            d << i->metaObject()->className();
//    }
    return app.exec();
}
