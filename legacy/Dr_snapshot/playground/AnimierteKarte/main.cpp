#include <QApplication>
#include <QQmlApplicationEngine>
#include <qdir.h>
#include "TransportSimulator/transportsimulator.h"
#include <qqml.h>

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{function} - %{message}");
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // @uri Playground
    const char *uri = "Playground";
    qmlRegisterType<TransportSimulator>(uri, 1, 0, "TransportSimulator");
    QDir dir(ROOTPATH);
    dir.cd("editor");
    QDir::setCurrent(dir.path());
        engine.load(QUrl(QString("%1/qml/animap.qml").arg(ROOTPATH)));

    return app.exec();
}
