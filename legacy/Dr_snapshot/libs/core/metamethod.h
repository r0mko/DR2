#ifndef METAMETHOD_H
#define METAMETHOD_H

#include <QObject>
#include "qmetaobject.h"
class MetaMethod : public QObject
{
    Q_OBJECT
    Q_ENUMS(Access)
    Q_ENUMS(MethodType)
    Q_PROPERTY(QString methodSignature READ methodSignature NOTIFY methodSignatureChanged STORED false)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged STORED false)
    Q_PROPERTY(QString typeName READ typeName NOTIFY typeNameChanged STORED false)
    Q_PROPERTY(int returnType READ returnType NOTIFY returnTypeChanged STORED false)
    Q_PROPERTY(QString returnTypeName READ returnTypeName NOTIFY returnTypeNameChanged STORED false)
    Q_PROPERTY(QList<QString> parameterTypes READ parameterTypes NOTIFY parameterTypesChanged STORED false)
    Q_PROPERTY(QList<QString> parameterNames READ parameterNames NOTIFY parameterNamesChanged STORED false)
    Q_PROPERTY(QString tag READ tag NOTIFY tagChanged STORED false)
    Q_PROPERTY(Access access READ access NOTIFY accessChanged STORED false)
    Q_PROPERTY(MethodType methodType READ methodType NOTIFY methodTypeChanged STORED false)
    Q_PROPERTY(int attributes READ attributes NOTIFY attributesChanged STORED false)
    Q_PROPERTY(int methodIndex READ methodIndex NOTIFY methodIndexChanged STORED false)
    Q_PROPERTY(int revision READ revision NOTIFY revisionChanged STORED false)

public:
    enum Access { Private, Protected, Public };
    enum MethodType { Method, Signal, Slot, Constructor };
    MetaMethod(QObject *parent = 0);
    MetaMethod(const QMetaMethod &metaMethod, QObject *parent = 0);

    QString methodSignature() const { return m_method.methodSignature(); }
    QString name() const { return m_method.name(); }
    QString typeName() const { return m_method.typeName(); }
    int returnType() const { return m_method.returnType(); }
    QString returnTypeName() const { return QMetaType::typeName(m_method.returnType()); }
    QList<QString> parameterTypes() const;
    QList<QString> parameterNames() const;
    QString tag() const;
    Access access() const { return (Access)m_method.access(); }
    MethodType methodType() const { return (MethodType)m_method.methodType(); }
    int attributes() const { return m_method.attributes(); }
    int methodIndex() const { return m_method.methodIndex(); }
    int revision() const { return m_method.revision(); }

signals:
    void methodSignatureChanged();
    void nameChanged();
    void typeNameChanged();
    void returnTypeChanged();
    void returnTypeNameChanged();
    void parameterTypesChanged();
    void parameterNamesChanged();
    void tagChanged();
    void accessChanged();
    void methodTypeChanged();
    void attributesChanged();
    void methodIndexChanged();
    void revisionChanged();

public slots:
private:
    QMetaMethod m_method;
};

#endif // METAMETHOD_H
