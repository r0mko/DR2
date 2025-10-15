#include "metamethod.h"


MetaMethod::MetaMethod(QObject *parent) :
    QObject(parent)
{
}

MetaMethod::MetaMethod(const QMetaMethod &metaMethod, QObject *parent) :
    QObject(parent),
    m_method(metaMethod)
{

}

QList<QString> MetaMethod::parameterTypes() const
{
    QList<QString> ret;
    for (QByteArray ba:m_method.parameterTypes()) {
        ret << ba;
    }
    return ret;
}

QList<QString> MetaMethod::parameterNames() const
{
    QList<QString> ret;
    for (QByteArray ba:m_method.parameterNames()) {
        ret << ba;
    }
    return ret;
}

QString MetaMethod::tag() const
{
    return m_method.tag();
}
