#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QtQml>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_NativeWindows);
    app.setApplicationName("RpcTestRig");
    app.setApplicationVersion("1.0");
    app.setOrganizationDomain("romko.ru");
    app.setOrganizationName("romko");
    QQmlApplicationEngine engine;
//    MyFilter filter;
//    app.installNativeEventFilter(&filter);
//    engine.rootContext()->setContextProperty("GlobalMouse", (int)app.mouseButtons());
//    engine.load(QUrl(QStringLiteral("qrc:///qml/test.qml")));
//    engine.load(QUrl(QStringLiteral("qrc:///qml/topo.qml")));
    engine.load(QUrl(QStringLiteral("qrc:///qml/main.qml")));
//    qRegisterMetaType<QJsonValue>();
//    qRegisterMetaType<QJsonObject>();
//    int mt = qMetaTypeId<QJsonValue>();
//    qDebug() << z << mt << z2;
    return app.exec();
}
