#ifndef HANDLE_H
#define HANDLE_H

#include <QQmlPropertyMap>


class HandleType : public QQmlPropertyMap
{
public:
    HandleType(QObject *parent = 0);
//    void addKey(QString key, QVariant value);
    void notifyChange(QString key);
    QVariant updateValue(const QString &key, const QVariant &input);
};

#endif // HANDLE_H
