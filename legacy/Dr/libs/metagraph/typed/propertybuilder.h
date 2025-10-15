#ifndef PROPERTYBUILDER_H
#define PROPERTYBUILDER_H

#include <QObject>
#include "type.h"

class Property : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(bool readable READ isReadable CONSTANT)
    Q_PROPERTY(bool writable READ isWritable CONSTANT)
    Q_PROPERTY(bool resettable READ isResettable CONSTANT)
    Q_PROPERTY(bool designable READ isDesignable CONSTANT)
    Q_PROPERTY(bool scriptable READ isScriptable CONSTANT)
    Q_PROPERTY(bool stored READ isStored CONSTANT)
    Q_PROPERTY(bool editable READ isEditable CONSTANT)
    Q_PROPERTY(bool user READ isUser CONSTANT)
    Q_PROPERTY(bool stdCppSet READ hasStdCppSet CONSTANT)
    Q_PROPERTY(bool enumOrFlag READ isEnumOrFlag CONSTANT)
    Q_PROPERTY(bool constant READ isConstant CONSTANT)
    Q_PROPERTY(bool final READ isFinal CONSTANT)
    Q_PROPERTY(int revision READ revision CONSTANT)
    Q_PROPERTY(bool notifyable READ notifyable CONSTANT)
    Q_PROPERTY(Method* notifySignal READ notifySignal CONSTANT)

public:
    Property(QMetaPropertyBuilder builder, QObject *parent = 0);
    Property(QObject *parent = 0);
    QString name() const;
    QString type() const;
    bool isReadable() const;
    bool isWritable() const;
    bool isResettable() const;
    bool isDesignable() const;
    bool isScriptable() const;
    bool isStored() const;
    bool isEditable() const;
    bool isUser() const;
    bool hasStdCppSet() const;
    bool isEnumOrFlag() const;
    bool isConstant() const;
    bool isFinal() const;
    int revision() const;
    Method *notifySignal() const;
    bool notifyable() const;

protected:
    QString m_type;
    QString m_name;
    Method *m_notifySignal = nullptr;

    union {
        struct {
            bool m_readable   :1;
            bool m_writable   :1;
            bool m_resettable :1;
            bool m_designable :1;
            bool m_scriptable :1;
            bool m_stored     :1;
            bool m_editable   :1;
            bool m_user       :1;
            bool m_stdCppSet  :1;
            bool m_enumOrFlag :1;
            bool m_constant   :1;
            bool m_final      :1;
        };
        int raw = 0;
    };
    int m_revision = 0;
};

class PropertyBuilder : public Property
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool readable READ isReadable WRITE setReadable NOTIFY readableChanged)
    Q_PROPERTY(bool writable READ isWritable WRITE setWritable NOTIFY writableChanged)
    Q_PROPERTY(bool resettable READ isResettable WRITE setResettable NOTIFY resettableChanged)
    Q_PROPERTY(bool designable READ isDesignable WRITE setDesignable NOTIFY designableChanged)
    Q_PROPERTY(bool scriptable READ isScriptable WRITE setScriptable NOTIFY scriptableChanged)
    Q_PROPERTY(bool stored READ isStored WRITE setStored NOTIFY storedChanged)
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable NOTIFY editableChanged)
    Q_PROPERTY(bool user READ isUser WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(bool stdCppSet READ hasStdCppSet WRITE setStdCppSet NOTIFY stdCppSetChanged)
    Q_PROPERTY(bool enumOrFlag READ isEnumOrFlag WRITE setEnumOrFlag NOTIFY enumOrFlagChanged)
    Q_PROPERTY(bool constant READ isConstant WRITE setConstant NOTIFY constantChanged)
    Q_PROPERTY(bool final READ isFinal WRITE setFinal NOTIFY finalChanged)
    Q_PROPERTY(int revision READ revision WRITE setRevision NOTIFY revisionChanged)
    Q_PROPERTY(bool notifyable READ notifyable WRITE setNotifyable NOTIFY notifyableChanged)
    Q_PROPERTY(Method* notifySignal READ notifySignal WRITE setNotifySignal NOTIFY notifySignalChanged)
    TypeFactory *d() const;

    void setupObject();
public:

    using Property::Property;

    ~PropertyBuilder();

    QByteArray getNotifierSignature() const;
    static QByteArray getNotifierSignature(const QByteArray &propertyName);
    QByteArray getNotifierName() const;

//    static QByteArray getNotifierSignature();

    void setType(QString arg);
    void setName(QString arg);
    void setReadable(bool arg);
    void setWritable(bool arg);
    void setResettable(bool arg);
    void setDesignable(bool arg);
    void setScriptable(bool arg);
    void setStored(bool arg);
    void setEditable(bool arg);
    void setUser(bool arg);
    void setStdCppSet(bool arg);
    void setEnumOrFlag(bool arg);
    void setConstant(bool arg);
    void setFinal(bool arg);
    void setRevision(int arg);
    void setNotifySignal(Method *arg);
    void setNotifyable(bool arg);

    int index() const;
signals:
    void readableChanged();
    void writableChanged();
    void resettableChanged();
    void designableChanged();
    void scriptableChanged();
    void storedChanged();
    void editableChanged();
    void userChanged();
    void stdCppSetChanged();
    void enumOrFlagChanged();
    void constantChanged();
    void finalChanged();
    void revisionChanged();
    void typeChanged(QString arg);
    void nameChanged(QString arg);
    void notifySignalChanged();
    void notifyableChanged(bool arg);
};

QString toCamelCase(QString arg) ;


#endif // PROPERTYBUILDER_H
