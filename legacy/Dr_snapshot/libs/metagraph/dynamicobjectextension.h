#ifndef DYNAMICOBJECTEXTENSION_H
#define DYNAMICOBJECTEXTENSION_H
#include <QDebug>
#include <QObject>
#include <QVector>
#include <QMetaProperty>
#include "private/qobject_p.h"
/**
 * @brief Базовый шаблон для создания динамических объектов.
 *
 */
template<class T>
class DynamicObjectExtension : public T
{
public:
    DynamicObjectExtension(QObject *parent = 0) : T(parent) {}
    virtual ~DynamicObjectExtension() {}

    const QMetaObject *metaObject() const;
    virtual int qt_metacall(QMetaObject::Call c, int id, void **argv);
    void emitSignal(int id, QVariantList args = QVariantList());

protected:
    QMetaObject *m_metaObject = nullptr;
    void setMetaObject(const QMetaObject *metaObject) { m_metaObject = const_cast<QMetaObject*>(metaObject); }

    virtual void setProperty(int index, QVariant value) = 0;
    virtual QVariant getProperty(int index) const = 0;
    virtual QVariant callMethod(int index, QVariantList args) = 0;

private:
    inline void writeProperty(int id, void **argv)
    {
        setProperty(id, QVariant(m_metaObject->property(id).userType(), argv[0]));
        int sig = m_metaObject->property(id).notifySignalIndex();
        *reinterpret_cast<int*>(argv[2]) = (int)true;
        emitSignal(sig);
    }
    void readProperty(int id, void **argv) const;
    void invokeMethod(int id, void **argv);
};


template<class T>
const QMetaObject *DynamicObjectExtension<T>::metaObject() const
{
    if (!m_metaObject)
        return &T::staticMetaObject;
    return m_metaObject;
}

template<class T>
int DynamicObjectExtension<T>::qt_metacall(QMetaObject::Call c, int id, void **argv)
{
    id = T::qt_metacall(c, id, argv);
    if (id < 0)
        return id;
    if (!m_metaObject)
        return id;
    switch (c) {
    case QMetaObject::InvokeMetaMethod:
        qDebug() << "Invokemethod";
        if (id < m_metaObject->methodCount())
            invokeMethod(id + m_metaObject->methodOffset(), argv);
        id -= m_metaObject->methodCount();
        break;
    case QMetaObject::ReadProperty:
        if (id < m_metaObject->propertyCount())
            readProperty(id + m_metaObject->propertyOffset(), argv);
        else
            qDebug() << "readProperty" << m_metaObject->property(id).name() ;
        id -= m_metaObject->propertyCount();
        break;
    case QMetaObject::WriteProperty:
        if (id < m_metaObject->propertyCount())
            writeProperty(id + m_metaObject->propertyOffset(), argv);
        id -= m_metaObject->propertyCount();
        break;
    case QMetaObject::ResetProperty:
        qDebug() << "ResetProperty";
    case QMetaObject::QueryPropertyDesignable:
        qDebug() << "QueryPropertyDesignable";
    case QMetaObject::QueryPropertyScriptable:
        qDebug() << "QueryPropertyScriptable";
    case QMetaObject::QueryPropertyEditable:
        qDebug() << "QueryPropertyEditable";
    case QMetaObject::QueryPropertyStored:
        qDebug() << "QueryPropertyStored";
    case QMetaObject::QueryPropertyUser:
        qDebug() << "QueryPropertyUser";
        id -= m_metaObject->propertyCount();
        break;
    case QMetaObject::CreateInstance:
        qDebug() << "CreateInstance";
        break;
    case QMetaObject::IndexOfMethod:
        qDebug() << "IndexOfMethod";
        break;
    case QMetaObject::RegisterPropertyMetaType:
        qDebug() << "RegisterPropertyMetaType";
        break;
    case QMetaObject::RegisterMethodArgumentMetaType:
        qDebug() << "RegisterMethodArgumentMetaType";
        break;
    }
    return id;
}

template<class T>
void DynamicObjectExtension<T>::emitSignal(int id, QVariantList args)
{
    if (args.size() != m_metaObject->method(id).parameterCount()) {
        qWarning() << "incorrect argument count" << args.size() << "for signal" << m_metaObject->method(id).methodSignature() << "sign with offset" << m_metaObject->method(id + m_metaObject->methodOffset()).methodSignature();
    }
    QVarLengthArray<void*> argv(11);
    argv[0] = nullptr;
    for (int i = 1; i < args.size(); ++i) {
        argv[i] = const_cast<void*>(args.at(i - 1).data());
    }
    QMetaObject::activate(this, m_metaObject, id - m_metaObject->methodOffset(), argv.data());
}

template<class T>
void DynamicObjectExtension<T>::readProperty(int id, void **argv) const
{
    //    qDebug() << "reading property" << id << m_metaObject->property(id).name();
    int tProp = m_metaObject->property(id).userType();
    if (tProp == qMetaTypeId<QVariant>()) {
        *reinterpret_cast<QVariant*>(argv[0]) = getProperty(id);
        return;
    }
    QVariant pval = getProperty(id);
    int tRet = pval.userType();
    if( !pval.isValid() ) {
        QMetaType::construct(tProp, argv[0], nullptr);
        return;
    }
    if (tRet == tProp) {
        QMetaType::construct(tProp, argv[0], pval.data());
    } else {
        if (QMetaType::hasRegisteredConverterFunction(tRet, tProp)) {
            QMetaType::construct(tRet, argv[0], pval.data());
        } else {
            QMetaType::construct(tProp, argv[0], nullptr);
            qWarning() << "incomaptible return type" << pval.typeName()
                       << "of property getter" << m_metaObject->property(id).name()
                       << "id" << id
                       << "type" << m_metaObject->property(id).typeName();
        }
    }
}

template<class T>
void DynamicObjectExtension<T>::invokeMethod(int id, void **argv)
{
    QVariantList arguments;
    for (int i = 0; i < m_metaObject->method(id).parameterCount(); ++i) {
        arguments.append(QVariant(m_metaObject->method(id).parameterType(i), argv[i+1]));
    }
    QVariant retval = callMethod(id, arguments);
    if (!retval.isValid()) {
        QMetaType::construct(m_metaObject->method(id).returnType(), argv[0], nullptr);
        return;
    }
    if (retval.userType() != m_metaObject->method(id).returnType()) {
        qDebug() << "method returns type" << retval.typeName() << "which differs from declared method type" << m_metaObject->method(id).typeName();
        QMetaType::convert(retval.data(), retval.userType(), argv[0], (m_metaObject->method(id).returnType()));
    } else {
        QMetaType::construct(m_metaObject->method(id).returnType(), argv[0], retval.data());
    }
    argv[0] = retval.data();
}



#endif // DYNAMICOBJECTEXTENSION_H

