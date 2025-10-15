//#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QDir>
#include "field.h"
#include "service.h"
#include "partmodel.h"
#include "userdata.h"
#include "printview.h"
#include "filterobject.h"
#include "filterdefinition.h"
#include "filterrepository.h"
#include "proxymodel.h"
#include "www.h"
#include "treeproxy.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    qmlRegisterType< FilterObject >( "Test", 1, 0, "FilterObject" );
    qmlRegisterType< Field >( "Test", 1, 0, "Field" );
    qmlRegisterType< PartModel >( "Test", 1, 0, "PartModel" );
    qmlRegisterType< Service >( "Test", 1, 0, "Service" );
    qmlRegisterType< PrintView >( "Test", 1, 0, "PrintView" );
    qmlRegisterType< ProxyModel >( "Test", 1, 0, "ProxyModel" );

    qmlRegisterType< Userdata >( "Test", 1, 0, "Userdata" );
    qmlRegisterType< TreeProxy >( "Test", 1, 0, "TreeProxy" );

    qmlRegisterType< FilterDefinition >( "Test", 1, 0, "FilterDefinition" );
    qmlRegisterType< FilterRepository >( "Test", 1, 0, "FilterRepository" );

    qmlRegisterType< WWW >( "Test", 1, 0, "WWW" );

    QQmlApplicationEngine engine;
    QDir::setCurrent(INSTALLPATH);
    engine.load( QUrl( QStringLiteral( "main.qml" ) ) );
    return app.exec();
}
