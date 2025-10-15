#include <QApplication>
//#include <qplatformdefs.h>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("WiringPainter");
    app.setApplicationVersion("1.0");
    app.setOrganizationDomain("romko.ru");
    app.setOrganizationName("romko");
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
