#include <QApplication>
#include <QQmlApplicationEngine>
#include <qdir.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // @uri Test
    //    const char *uri = "Test";
    QDir dir(ROOTPATH);
    dir.cd("editor");
    QDir::setCurrent(dir.path());
        engine.load(QUrl(QString("%1/qml/animap.qml").arg(ROOTPATH)));

    return app.exec();
}
