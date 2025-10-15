#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDir>
//#include <QJsonArray>
//#include <QJsonValue>
//#include <QJsonObject>
//#include <QJsonDocument>
#include "qqml.h"
//#include <QDebug>
//#include <QDataStream>

//QDataStream &operator <<(QDataStream &out, const QJsonValue &v);
//QDataStream &operator >>(QDataStream &in, QJsonValue &v);
int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    QQmlApplicationEngine engine;
    QDir::setCurrent(ROOTPATH);
    engine.load(QUrl("main.qml"));


//    qRegisterMetaTypeStreamOperators<QJsonValue>("QJsonValue");
//    QJsonDocument x = QJsonDocument::fromJson( R"({ "a": 12 })" );
//    qDebug() << x.toJson();
//    QJsonValue v = x.object();
//    QVariant z( v );
//    QByteArray ba;
//    QDataStream ds( &ba, QIODevice::WriteOnly );
//    ds << z;
//    ds << x.toBinaryData();



    return app.exec();
}

//QDataStream &operator <<(QDataStream &out, const QJsonValue &v)
//{
//    QJsonDocument doc;
//    if (v.isObject())
//        doc.setObject(v.toObject());
//    else if (v.isArray())
//        doc.setArray(v.toArray());
//    out << doc.toBinaryData();
//    return out;
//}


//QDataStream &operator >>(QDataStream &in, QJsonValue &v)
//{
//    QByteArray bin;
//    in >> bin;
//    QJsonDocument doc = QJsonDocument::fromBinaryData(bin);
//    if (doc.isArray())
//        v = doc.array();
//    else if (doc.isObject())
//        v = doc.object();
//    return in;
//}
