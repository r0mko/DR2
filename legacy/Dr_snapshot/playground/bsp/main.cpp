
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QDir>

//#include "bspdata.h"
#include "bspobject.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );

    qmlRegisterType< BspObject >( "Test", 1, 0, "BspObject" );
//    qmlRegisterUncreatableType<BspData>("Test", 1, 0, BspData, "BspData is not valid QML type");
    QQmlApplicationEngine engine;
    QDir::setCurrent(INSTALLPATH);
    engine.load( QUrl( QStringLiteral( "main.qml" ) ) );
    return app.exec();
}
