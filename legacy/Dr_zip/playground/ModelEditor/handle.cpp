#include "handle.h"

#include <QDebug>

HandleType::HandleType(QObject *parent) :
    QQmlPropertyMap(parent)
{
    insert("foo", "bar");
}

QVariant HandleType::updateValue(const QString &key, const QVariant &input)
{
    qDebug() << "Value" << key << "updated" << input;
    return input;
}

