
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlContext>
#include <QDir>
#include "qqml.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);




    QQmlApplicationEngine engine;
    QDir dir(ROOTPATH);
    dir.cd("editor");
    QDir::setCurrent(dir.path());
        engine.load(QUrl(QString("%1/main.qml").arg(ROOTPATH)));
    return app.exec();
}
