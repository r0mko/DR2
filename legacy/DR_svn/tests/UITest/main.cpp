#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QMetaType>
#include <QWindow>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
//    engine.load(QUrl(QStringLiteral("qrc:/uni.qml")));
    engine.load(QUrl(QStringLiteral("qrc:///palette.qml")));
//    for (int i = QMetaType::FirstCoreType; i < QMetaType::LastWidgetsType + 2048; ++i) {
//        if (QMetaType::isRegistered(i)) {
//            qDebug() << i << QMetaType::typeName(i);
//        }
//    }
    return app.exec();
}
