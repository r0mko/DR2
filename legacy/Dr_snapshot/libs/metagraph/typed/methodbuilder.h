#ifndef METHODBUILDER_H
#define METHODBUILDER_H

#include <QObject>
#include "type.h"

class MethodBuilder;
class Method : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Access access READ access CONSTANT)
    Q_PROPERTY(MethodType methodType READ methodType CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString returnType READ returnType CONSTANT)
    Q_PROPERTY(QString tag READ tag CONSTANT)
    Q_PROPERTY(QStringList parameterNames READ parameterNames CONSTANT)
    Q_PROPERTY(QStringList parameterTypes READ parameterTypes CONSTANT)
    Q_PROPERTY(int revision READ revision CONSTANT)
    Q_PROPERTY(QString signature READ signature CONSTANT)

    Q_ENUMS(Access)
    Q_ENUMS(MethodType)
    void setupObject();

public:
    enum Access { Private = QMetaMethod::Private, Protected, Public };
    enum MethodType { InvokableMethod = QMetaMethod::Method, Signal, Slot, Constructor };
    explicit Method(QObject *parent = 0);
    explicit Method(QMetaMethodBuilder builder, QObject *parent = 0);
    Access access() const;
    MethodType methodType() const;
    QString name() const;
    QString returnType() const;
    QString tag() const;
    QStringList parameterNames() const;
    QStringList parameterTypes() const;
    int revision() const;
    QString signature() const;
    int index() const;
    MethodBuilder *builder();

protected:
    TypeFactory *d() const;
    static const int _accessIdx;
    static const int _methodTypeIdx;
    Access m_access;
    MethodType m_methodType;
    QString m_name;
    QString m_returnType;
    QString m_tag;
    QStringList m_parameterNames;
    QStringList m_parameterTypes;
    int m_revision = 0;
};

class MethodBuilder : public Method
{
    Q_OBJECT
    Q_PROPERTY(Access access READ access WRITE setAccess NOTIFY accessChanged)
    Q_PROPERTY(MethodType methodType READ methodType NOTIFY methodTypeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString returnType READ returnType WRITE setReturnType NOTIFY returnTypeChanged)
    Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged)
    Q_PROPERTY(QStringList parameterNames READ parameterNames WRITE setParameterNames NOTIFY parameterNamesChanged)
    Q_PROPERTY(QStringList parameterTypes READ parameterTypes NOTIFY parameterTypesChanged)
    Q_PROPERTY(int revision READ revision WRITE setRevision NOTIFY revisionChanged)
    Q_PROPERTY(QString signature READ signature NOTIFY signatureChanged)

    Q_ENUMS(Access)
    Q_ENUMS(MethodType)

public:
    using Method::Method;


    void setAccess(Access arg);
    void setName(QString arg);
    void setReturnType(QString arg);
    void setTag(QString arg);
    void setParameterNames(QStringList arg);
    void setRevision(int arg);



    int methodTypeAsInt() const;

signals:
    void accessChanged(Access arg);
    void methodTypeChanged(MethodType arg);
    void returnTypeChanged(QString arg);
    void tagChanged(QString arg);
    void parameterNamesChanged(QStringList arg);
    void parameterTypesChanged(QStringList arg);
    void revisionChanged(int arg);
    void signatureChanged();
    void nameChanged(QString arg);


};

Q_DECLARE_METATYPE(MethodBuilder::MethodType)
Q_DECLARE_METATYPE(MethodBuilder::Access)

#endif // METHODBUILDER_H
