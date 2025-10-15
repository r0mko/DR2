#ifndef METAPROPERTY_H
#define METAPROPERTY_H

#include <QObject>
#include "qmetaobject.h"
class MetaEnum;
class MetaMethod;
class MetaProperty : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT STORED false)
    Q_PROPERTY(QString typeName READ typeName CONSTANT STORED false)
    Q_PROPERTY(int userType READ userType CONSTANT STORED false)
    Q_PROPERTY(int propertyIndex READ propertyIndex CONSTANT STORED false)
    Q_PROPERTY(bool isReadable READ isReadable CONSTANT STORED false)
    Q_PROPERTY(bool isWritable READ isWritable CONSTANT STORED false)
    Q_PROPERTY(bool isResettable READ isResettable CONSTANT STORED false)
    Q_PROPERTY(bool isDesignable READ isDesignable CONSTANT STORED false)
    Q_PROPERTY(bool isScriptable READ isScriptable CONSTANT STORED false)
    Q_PROPERTY(bool isStored READ isStored CONSTANT STORED false)
    Q_PROPERTY(bool isEditable READ isEditable CONSTANT STORED false)
    Q_PROPERTY(bool isUser READ isUser CONSTANT STORED false)
    Q_PROPERTY(bool isConstant READ isConstant CONSTANT STORED false)
    Q_PROPERTY(bool isFinal READ isFinal CONSTANT STORED false)
    Q_PROPERTY(bool isFlagType READ isFlagType CONSTANT STORED false)
    Q_PROPERTY(bool isEnumType READ isEnumType CONSTANT STORED false)
    Q_PROPERTY(MetaEnum* enumerator READ enumerator CONSTANT STORED false)
    Q_PROPERTY(bool hasNotifySignal READ hasNotifySignal CONSTANT STORED false)
    Q_PROPERTY(MetaMethod* notifySignal READ notifySignal CONSTANT STORED false)
    Q_PROPERTY(int notifySignalIndex READ notifySignalIndex CONSTANT STORED false)
    Q_PROPERTY(int revision READ revision CONSTANT STORED false)
    Q_PROPERTY(int inheritLevel READ inheritLevel CONSTANT STORED false)
    Q_PROPERTY(QString inheritClassName READ inheritClassName CONSTANT)
    Q_PROPERTY(QObject* object READ object CONSTANT)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    MetaProperty(QObject *parent = 0);
    MetaProperty(QMetaProperty prop, QObject *object, MetaMethod* notifySignal = 0, MetaEnum* metaEnum = 0, QObject *parent = 0);
    QString name() const { return p.name(); }
    QString typeName() const { return p.typeName(); }
    int userType() const { return p.userType(); }
    int propertyIndex() const { return p.propertyIndex(); }

    bool isReadable() const { return p.isReadable(); }
    bool isWritable() const { return p.isWritable(); }
    bool isResettable() const { return p.isResettable(); }
    bool isDesignable() const { return p.isDesignable(); }
    bool isScriptable() const { return p.isScriptable(); }
    bool isStored() const { return p.isStored(); }
    bool isEditable() const { return p.isEditable(); }
    bool isUser() const { return p.isUser(); }
    bool isConstant() const { return p.isConstant(); }
    bool isFinal() const { return p.isFinal(); }

    bool isFlagType() const { return p.isFlagType(); }
    bool isEnumType() const { return p.isEnumType(); }
    MetaEnum* enumerator() const { return m_enumerator; }
    bool hasNotifySignal() const { return p.hasNotifySignal(); }
    MetaMethod* notifySignal() const { return m_notifySignal; }
    int notifySignalIndex() const { return p.notifySignalIndex(); }
    int revision() const { return p.revision(); }
    QObject* object() const;
    QVariant value() const;
    int inheritLevel() const;
    QString inheritClassName() const;

signals:
    void valueChanged();

public slots:
    bool setValue(QVariant arg);

private slots:
    void updateValue();

private:
    QMetaProperty p;
    MetaEnum* m_enumerator;
    MetaMethod* m_notifySignal;
    QObject *m_object;
    int valuePropertyIdx;
    int m_inheritLevel;
    QString m_inheritClassName;
};

#endif // METAPROPERTY_H
