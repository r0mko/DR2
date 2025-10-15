#include "propertybuilder.h"
#include "methodbuilder.h"
#include <QQmlEngine>
#include <QRegularExpression>
#include <QDebug>

QString toCamelCase(QString arg)
{
    QString ret;
    QRegularExpression re("[ _]");
    QStringList parts = arg.split(re);

    if (parts.isEmpty())
        return ret;
    for (QString &s:parts) {
//        s = s.toLower();
        s[0] = s[0].toUpper();
    }
    parts.first()[0] = parts.first()[0].toLower();
    return parts.join("");
}

TypeFactory *PropertyBuilder::d() const
{
    return qobject_cast<TypeFactory*>(parent());
}


void PropertyBuilder::setupObject()
{
    setObjectName(QString("property_%1_%2").arg(index()).arg(QString(name())));
}
int PropertyBuilder::index() const
{
    return d()->m_properties.indexOf(const_cast<PropertyBuilder*>(this));
}

PropertyBuilder::~PropertyBuilder()
{}

QByteArray PropertyBuilder::getNotifierName() const
{
    return QString("%1Changed").arg(toCamelCase(name())).toLocal8Bit();
}

void PropertyBuilder::setType(QString arg)
{
    if (QMetaType::type(arg.toLocal8Bit().data()) == QMetaType::UnknownType) {
        qWarning() << "Wrong type name" << arg;
        return;
    }
    if (m_type == arg)
        return;

    m_type = arg;
    emit typeChanged(arg);
}

QByteArray PropertyBuilder::getNotifierSignature() const
{
    return getNotifierName().append("()");
}

QByteArray PropertyBuilder::getNotifierSignature(const QByteArray &propertyName)
{
    return QString("%1Changed()").arg(toCamelCase(propertyName)).toLocal8Bit();
}

void PropertyBuilder::setName(QString arg)
{

    if (m_name == arg)
        return;

    m_name = arg;
    if (m_notifySignal) {
        m_notifySignal->builder()->setName(getNotifierName());
    }
    emit nameChanged(arg);
}

void PropertyBuilder::setReadable(bool arg)
{
    if (m_readable == arg)
        return;

    m_readable = arg;
    emit readableChanged();
}

void PropertyBuilder::setWritable(bool arg)
{
    if (m_writable == arg)
        return;

    m_writable = arg;
    emit writableChanged();
}

void PropertyBuilder::setResettable(bool arg)
{
    if (m_resettable == arg)
        return;

    m_resettable = arg;
    emit resettableChanged();
}

void PropertyBuilder::setDesignable(bool arg)
{
    if (m_designable == arg)
        return;

    m_designable = arg;
    emit designableChanged();
}

void PropertyBuilder::setScriptable(bool arg)
{
    if (m_scriptable == arg)
        return;

    m_scriptable = arg;
    emit scriptableChanged();
}

void PropertyBuilder::setStored(bool arg)
{
    if (m_stored == arg)
        return;

    m_stored = arg;
    emit storedChanged();
}

void PropertyBuilder::setEditable(bool arg)
{
    if (m_editable == arg)
        return;

    m_editable = arg;
    emit editableChanged();
}

void PropertyBuilder::setUser(bool arg)
{
    if (m_user == arg)
        return;

    m_user = arg;
    emit userChanged();
}

void PropertyBuilder::setStdCppSet(bool arg)
{
    if (m_stdCppSet == arg)
        return;

    m_stdCppSet = arg;
    emit stdCppSetChanged();
}

void PropertyBuilder::setEnumOrFlag(bool arg)
{
    if (m_enumOrFlag == arg)
        return;

    m_enumOrFlag = arg;
    emit enumOrFlagChanged();
}

void PropertyBuilder::setConstant(bool arg)
{
    if (m_constant == arg)
        return;

    if (arg)
        setNotifyable(false);
    m_constant = arg;
    emit constantChanged();
}

void PropertyBuilder::setFinal(bool arg)
{
    if (m_final == arg)
        return;

    m_final = arg;
    emit finalChanged();
}

void PropertyBuilder::setRevision(int arg)
{
    if (m_revision == arg)
        return;

    m_revision = arg;
    emit revisionChanged();
}

void PropertyBuilder::setNotifySignal(Method *arg)
{
    if (m_notifySignal == arg)
        return;

    m_notifySignal = arg;
    emit notifySignalChanged();
}

void PropertyBuilder::setNotifyable(bool arg)
{
    if (arg == (m_notifySignal != nullptr))
        return;
    if (arg) {
        setConstant(false);
        setNotifySignal(d()->addSignal(getNotifierSignature()));

    } else {
        d()->removeSignal(m_notifySignal);
        setNotifySignal(nullptr);
    }
    emit notifyableChanged(arg);
}




Property::Property(QMetaPropertyBuilder builder, QObject *parent) :
    QObject(parent)
{
    setObjectName(QString("property_%1_%2").arg(builder.index()).arg(QString(builder.name())));
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    m_readable    = builder.isReadable();
    m_writable    = builder.isWritable();
    m_resettable  = builder.isResettable();
    m_designable  = builder.isDesignable();
    m_scriptable  = builder.isScriptable();
    m_stored      = builder.isStored();
    m_editable    = builder.isEditable();
    m_user        = builder.isUser();
    m_stdCppSet   = builder.hasStdCppSet();
    m_enumOrFlag  = builder.isEnumOrFlag();
    m_constant    = builder.isConstant();
    m_final       = builder.isFinal();
    m_name        = builder.name();
    m_type        = builder.type();
    m_revision    = builder.revision();
}

Property::Property(QObject *parent) :
    QObject(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    m_readable    = true;
    m_writable    = true;
    m_resettable  = false;
    m_designable  = false;
    m_scriptable  = true;
    m_stored      = true;
    m_editable    = false;
    m_user        = false;
    m_stdCppSet   = false;
    m_enumOrFlag  = false;
    m_constant    = false;
    m_final       = false;
}

QString Property::name() const
{
    return m_name;
}

QString Property::type() const
{
    return m_type;
}

bool Property::isReadable() const
{
    return m_readable;
}

bool Property::isWritable() const
{
    return m_writable;
}

bool Property::isResettable() const
{
    return m_resettable;
}

bool Property::isDesignable() const
{
    return m_designable;
}

bool Property::isScriptable() const
{
    return m_scriptable;
}

bool Property::isStored() const
{
    return m_stored;
}

bool Property::isEditable() const
{
    return m_editable;
}

bool Property::isUser() const
{
    return m_user;
}

bool Property::hasStdCppSet() const
{
    return m_stdCppSet;
}

bool Property::isEnumOrFlag() const
{
    return m_enumOrFlag;
}

bool Property::isConstant() const
{
    return m_constant;
}

bool Property::isFinal() const
{
    return m_final;
}

int Property::revision() const
{
    return m_revision;
}

Method *Property::notifySignal() const
{
    return m_notifySignal;
}

bool Property::notifyable() const
{
    return m_notifySignal != nullptr;
}
