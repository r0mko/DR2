#include <QApplication>
#include <QQmlApplicationEngine>
#include "font2qpicture.h"
#include <qqml.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // @uri Font2Pic
    const char *uri = "Font2Pic";
    qmlRegisterType<Font2QPicture>(uri, 1, 0, "Font2Picture");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
