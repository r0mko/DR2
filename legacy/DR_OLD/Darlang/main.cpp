#include <QCoreApplication>
#include "darlangparser.h"
#include <QString>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList prog;
    //    prog << "double a;";
//    prog << "a = message.Summ;";
//    prog << "string s = \"a string testing\";";
//    prog << "popFunction (b, s, 49.152, preg_pop);";
    prog << "drawLine(-90.43+9, innerCall(90,\"foo\"));";
//    //    prog << "b = 52.3 / b;";
    //    prog << "c=52.3*3;";
    QString s = prog.join("\n");
//    QString m = "float func1_R (int, double, double, string)";
//    DarlangFunction f(m);
//    qDebug() << f.signature();
    //    QRegExp rx("((\\;|(?!\")\\b[A-Za-z_]+\\b(?!\")|\".+\"|\\(|\\)|,|\"|=|\\+|\\*|/|[-]?[0-9]*\\.?[0-9]+|\\.))");
    //    QStringList tokens;
    //    foreach(QString s, prog) {
    //        int pos = 0;
    //        while ((pos = rx.indexIn(s, pos)) != -1) {
    //            tokens << rx.cap(1);
    //            pos += rx.matchedLength();
    //        }
    //    }
    //    foreach(QString token, tokens)
    //    {
    //        qDebug() << token.toStdString().c_str();
    //    }
    //    QRegExp rx("((\"\\.+\"|(?!\")\\b[A-Za-z_]+\\b(?!\")|\\(|\\)|,|\"|=|\\+|\\*|/|[-]?[0-9]*\\.?[0-9]+|\\.|\\;))");

    //    rx.setMinimal(true);
    //    QRegExp rx("((\\b[A-Za-z_]+\\b|\".+?\"))");
    //    QRegExp rx("(([A-Za-z_]+|\".+\"|\\(|\\)|,|\"|=|\\+|\\*|/|[-]?[0-9]*\\.?[0-9]+|\\.|\\;))");
    //    QRegExp rx("((\".+\"))");
    //    QRegExp rx("((\"\\.+\"|[A-Za-z_]+|\\(|\\)|,|\"|=|\\+|\\*|/|[-]?[0-9]*\\.?[0-9]+|\\.|\\;))");

    DarlangCompiler c(s);
    qDebug() << c.parse();
    qDebug() << c.variables();
    qDebug() << c.unresolvedIdentifiers();
    return a.exec();
}
