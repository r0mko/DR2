#include <QApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>
#include "vchain.h"
#include "vnode.h"
#include <QDebug>
//#include "vrunner.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("WiringPainter");
    app.setApplicationVersion("1.0");
    app.setOrganizationDomain("romko.ru");
    app.setOrganizationName("romko");
    QQmlApplicationEngine engine;
    // @uri Playground
//    const char *uri = "Playground";
    QVariant v;
    v = M_PI;
    qDebug() << sizeof(v);
//    qmlRegisterType<PolygonSmoother>(uri, 1, 0, "PolygonSmoother");
//    engine.load(QUrl(QStringLiteral("qrc:/ed.qml")));
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
//    engine.load(QUrl(QStringLiteral("qrc:/guidepainter.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/wiringeditor.qml")));
    return app.exec();
}
