#include "metaenum.h"

MetaEnum::MetaEnum(QObject *parent) :
    QObject(parent)
{
    init();
}

MetaEnum::MetaEnum(QMetaEnum metaEnum, QObject *parent) :
    QObject(parent),
    m_enum(metaEnum)
{
    init();
}

QStringList MetaEnum::keys() const
{
    return m_keys;
}

QString MetaEnum::inheritClassName() const
{
    return m_enum.enclosingMetaObject()->className();
}

QList<int> MetaEnum::values() const
{
    return m_values;
}

QString MetaEnum::valueToKeys(int value) const
{
    return m_enum.valueToKeys(value);
}

QString MetaEnum::valueToKey(int value) const
{
    return m_enum.valueToKey(value);
}


QString MetaEnum::key(int index) const { return m_enum.key(index); }

int MetaEnum::value(int index) const { return m_enum.value(index); }

void MetaEnum::init()
{
    for (int i = 0; i < m_enum.keyCount(); ++i) {
        m_keys << m_enum.key(i);
        m_values << m_enum.value(i);
    }
}


