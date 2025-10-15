#include <QApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>
#include "TransportSimulator/transportsimulator.h"
#include <QDebug>


int main(int argc, char *argv[])
{
//    qSetMessagePattern("%{if-warning}Warning:%{endif}%{if-critical}Critical:%{endif}%{function} - %{message}");
    QApplication app(argc, argv);
    app.setApplicationName("WiringPainter");
    app.setApplicationVersion("1.0");
    app.setOrganizationDomain("romko.ru");
    app.setOrganizationName("romko");
    QQmlApplicationEngine engine;
    // @uri Playground
    const char *uri = "Playground";
    qmlRegisterType<TransportSimulator>(uri, 1, 0, "TransportSimulator");
//    engine.load(QUrl(QStringLiteral("qrc:/ed.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/wiringeditor.qml")));
//    engine.load(QUrl(QStringLiteral("qrc:/guidepainter.qml")));
//    engine.load(QUrl(QStringLiteral("qrc:/contactnetwork.qml")));
    return app.exec();
}
