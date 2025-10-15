#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QDebug>
#include <QMetaType>

int main(int argc, char *argv[])
{
    QGuiApplication app( argc, argv );
    QQmlApplicationEngine engine;
    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );
    return app.exec();
}
