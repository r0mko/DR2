#ifndef CREATIST_H
#define CREATIST_H

#include <QMetaEnum>
#include <QObject>
#include <QVariant>
#include <qjsonobject.h>
class Creatist: public QObject
{
    Q_OBJECT
//    struct Instance {
//        int type = QMetaType::UnknownType;
//        const QMetaObject *metaObject = nullptr;
//        void *ptr = nullptr;
//        void destroy();
//        inline operator bool() const { return ptr != nullptr; }
//        inline operator QVariant() const { return QVariant(type, ptr, 1); }
//        bool update(void *ptr, const QMetaObject *metaObject, const QVariantMap &attr) const;
//    };

public:
    Creatist(QObject* parent = nullptr);

    Q_INVOKABLE QVariant create(const QString &type, const QVariantMap &attr = QVariantMap());
    Q_INVOKABLE QVariant handle(const QString &type);
    bool update(void *ptr, const QMetaObject *metaObject, const QVariantMap &attr) const;
    Q_INVOKABLE QByteArray serialize(QVariant value) const;
    Q_INVOKABLE QVariant deserialize(const QString &type, const QByteArray &blob) const;
};

#endif // CREATIST_H
