#ifndef METAENUM_H
#define METAENUM_H

#include <QObject>
#include "qmetaobject.h"

class MetaEnum : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT STORED false)
    Q_PROPERTY(bool isFlag READ isFlag CONSTANT STORED false)
    Q_PROPERTY(int keyCount READ keyCount CONSTANT STORED false)
    Q_PROPERTY(QString scope READ scope CONSTANT STORED false)
    Q_PROPERTY(QStringList keys READ keys CONSTANT STORED false)
    Q_PROPERTY(QList<int> values READ values CONSTANT STORED false)
    Q_PROPERTY(QString inheritClassName READ inheritClassName CONSTANT)

public:
    explicit MetaEnum(QObject *parent = 0);
    explicit MetaEnum(QMetaEnum metaEnum, QObject *parent = 0);
    QString name() const { return m_enum.name(); }
    bool isFlag() const { return m_enum.isFlag(); }
    int keyCount() const { return m_enum.keyCount(); }
    QString scope() const { return m_enum.scope(); }
    QStringList keys() const;
    QString inheritClassName() const;
    QList<int> values() const;

    Q_INVOKABLE QString valueToKeys(int value) const;
    Q_INVOKABLE QString valueToKey(int value) const;

public slots:
    QString key(int index) const;
    int value(int index) const;

private:
    void init();
    QMetaEnum m_enum;
    QStringList m_keys;
    QList<int> m_values;
};

#endif // METAENUM_H
