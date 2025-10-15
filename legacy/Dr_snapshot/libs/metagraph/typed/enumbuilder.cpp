#include "enumbuilder.h"
#include "type.h"
#include <QQmlEngine>

TypeFactory *EnumBuilder::d() const
{
    return qobject_cast<TypeFactory*>(parent());
}


EnumBuilder::~EnumBuilder()
{}

int EnumBuilder::index() const
{
    return d()->m_enums.indexOf(const_cast<EnumBuilder*>(this));
}









void EnumBuilder::setName(QString arg)
{
    if (m_name == arg)
        return;

    m_name = arg;
    emit nameChanged(arg);
}

void EnumBuilder::setKeys(QList<QString> arg)
{
    if (m_keys == arg)
        return;

    m_keys = arg;
    emit keysChanged(arg);
}

void EnumBuilder::setValues(QList<int> arg)
{
    if (m_values == arg)
        return;

    m_values = arg;
    emit valuesChanged(arg);
}

void EnumBuilder::setIsFlag(bool arg)
{
    if (m_isFlag == arg)
        return;

    m_isFlag = arg;
    emit isFlagChanged(arg);
}




Enum::Enum(QMetaEnumBuilder d, QObject *parent) :
    QObject(parent)
{
    for (int i = 0; i < d.keyCount(); ++i) {
        m_keys.append(d.key(i));
        m_values.append(d.value(i));
    }
    m_name = d.name();
    m_isFlag = d.isFlag();
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

QString Enum::name() const
{
    return m_name;
}

QList<QString> Enum::keys() const
{
    return m_keys;
}

QList<int> Enum::values() const
{
    return m_values;
}

bool Enum::isFlag() const
{
    return m_isFlag;
}

Enum::operator EnumBuilder*()
{
    return static_cast<EnumBuilder*>(this);
}
