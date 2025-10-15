#ifndef ENUMBUILDER_H
#define ENUMBUILDER_H

#include <QObject>
#include "private/qmetaobjectbuilder_p.h"

class TypeFactory;
class EnumBuilder;

class Enum : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QList<QString> keys READ keys CONSTANT)
    Q_PROPERTY(QList<int> values READ values CONSTANT)
    Q_PROPERTY(bool isFlag READ isFlag CONSTANT)

public:
    Enum(QObject *parent = 0) : QObject(parent) {}
    Enum(QMetaEnumBuilder d, QObject *parent = 0);
    QString name() const;
    QList<QString> keys() const;
    QList<int> values() const;
    bool isFlag() const;
    operator EnumBuilder*();
protected:
    QString m_name;
    QList<QString> m_keys;
    QList<int> m_values;
    bool m_isFlag = false;
};

class EnumBuilder : public Enum
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QList<QString> keys READ keys WRITE setKeys NOTIFY keysChanged)
    Q_PROPERTY(QList<int> values READ values WRITE setValues NOTIFY valuesChanged)
    Q_PROPERTY(bool isFlag READ isFlag WRITE setIsFlag NOTIFY isFlagChanged)

    TypeFactory *d() const;

public:
    using Enum::Enum;
    ~EnumBuilder();

    int index() const;
    void setName(QString arg);
    void setKeys(QList<QString> arg);
    void setValues(QList<int> arg);
    void setIsFlag(bool arg);


signals:
    void nameChanged(QString arg);
    void keysChanged(QList<QString> arg);
    void valuesChanged(QList<int> arg);
    void isFlagChanged(bool arg);
};

#endif // ENUMBUILDER_H
