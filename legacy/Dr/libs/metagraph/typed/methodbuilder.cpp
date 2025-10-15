#include "methodbuilder.h"
#include "type.h"
#include <QQmlEngine>
#include <QDebug>

const int Method::_accessIdx = MethodBuilder::staticMetaObject.indexOfEnumerator("Access");
const int Method::_methodTypeIdx = MethodBuilder::staticMetaObject.indexOfEnumerator("MethodType");





//MethodBuilder::MethodBuilder(QObject *parent):
//    QObject(parent)
//{}

//MethodBuilder::MethodBuilder(QMetaMethodBuilder b, QObject *parent) :
//    QObject(parent)
//{
//    for (const QByteArray &ba : b.parameterNames())
//        m_parameterNames.append(ba);
//    for (const QByteArray &ba : b.parameterTypes())
//        m_parameterTypes.append(ba);
//    m_returnType = b.returnType();
//    m_name = b.signature();
//    m_name.truncate(m_name.indexOf("("));
//    m_access = (Access)(int)b.access();
//    m_methodType = (MethodType)(int)b.methodType();
//    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
//    setupObject();
//}


int MethodBuilder::methodTypeAsInt() const
{
    return (int)m_methodType;
}

void MethodBuilder::setAccess(MethodBuilder::Access arg)
{
    if (m_access == arg)
        return;

    m_access = arg;
    emit accessChanged(arg);
}

void MethodBuilder::setName(QString arg)
{
    if (m_name == arg)
        return;
    m_name = arg;
    emit nameChanged(arg);
    emit signatureChanged();
    emit d()->methodsChanged();
}

void MethodBuilder::setReturnType(QString arg)
{
    if (m_returnType == arg)
        return;

    m_returnType = arg;
    emit returnTypeChanged(arg);
    emit signatureChanged();
}

void MethodBuilder::setTag(QString arg)
{
    if (m_tag == arg)
        return;

    m_tag = arg;
    emit tagChanged(arg);
}
void MethodBuilder::setParameterNames(QStringList arg)
{
    if (m_parameterNames == arg)
        return;

    m_parameterNames = arg;
    emit parameterNamesChanged(arg);
}


void MethodBuilder::setRevision(int arg)
{
    if (m_revision == arg)
        return;

    m_revision = arg;
    emit revisionChanged(arg);
}



void Method::setupObject()
{
    QByteArray typeText = staticMetaObject.enumerator(_methodTypeIdx).valueToKey((int)m_methodType);
    setObjectName(QString("%1_%2_%3").arg(index()).arg(QString(typeText)).arg(QString(signature())));
}

Method::Method(QObject *parent) :
    QObject(parent  )
{

}

Method::Method(QMetaMethodBuilder b, QObject *parent) :
    QObject(parent)
{
    for (const QByteArray &ba : b.parameterNames())
        m_parameterNames.append(ba);
    for (const QByteArray &ba : b.parameterTypes())
        m_parameterTypes.append(ba);
    m_returnType = b.returnType();
    m_name = b.signature();
    m_name.truncate(m_name.indexOf("("));
    m_access = (Access)(int)b.access();
    m_methodType = (MethodType)(int)b.methodType();
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    setupObject();
}

Method::Access Method::access() const
{
    return m_access;
}

Method::MethodType Method::methodType() const
{
    return m_methodType;
}

QString Method::name() const
{
    return m_name;
}

QString Method::returnType() const
{
    return m_returnType;
}

QString Method::tag() const
{
    return m_tag;
}

QStringList Method::parameterNames() const
{
    return m_parameterNames;
}

QStringList Method::parameterTypes() const
{
    return m_parameterTypes;
}

int Method::revision() const
{
    return m_revision;
}

QString Method::signature() const
{
    return QString("%1(%2)").arg(m_name).arg(m_parameterTypes.join(","));
}

int Method::index() const
{
    return d()->m_methods.indexOf(const_cast<Method*>(this));
}

MethodBuilder *Method::builder()
{
    return static_cast<MethodBuilder*>(this);
}

TypeFactory *Method::d() const
{
    return qobject_cast<TypeFactory*>(parent());
}
