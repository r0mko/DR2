#ifndef GRAPHMODELHELPER_H
#define GRAPHMODELHELPER_H

#include "qmetaobject.h"
#include <QObject>
#include <graphindex.h>
/**
 * @brief Синглтон с полезными функциями для GraphModel
 */
class GraphModelHelper : public QObject
{
    Q_OBJECT

public:
    explicit GraphModelHelper(QObject *parent = 0);

    Q_INVOKABLE inline GraphIndex graphIndex(const QModelIndex &index) const { return GraphIndex(index); }
    Q_INVOKABLE QStringList graphModels() const;
    Q_INVOKABLE GraphIndex resolve(const QUrl &mrl) const;
    Q_INVOKABLE QVariantList extractProperties(QVariant value, bool excludeSuper = true) const;
    Q_INVOKABLE QVariantList extractPropertiesFromClassName(QString className, bool excludeSuper = true) const;
    Q_INVOKABLE QVariantMap extractMetaObject(QObject *obj) const;
    Q_INVOKABLE QVariantMap extractMetaObjectFromClassName(QString className) const;

public slots:
private:
    QVariantList dumpProperties(const QMetaObject *mo, bool excludeSuper = true) const;
};

QVariantMap buildProperty(const QMetaProperty &prop);
QVariantMap buildEnum(const QMetaEnum &e);
QVariantMap buildMethod(const QMetaMethod &mm);
QVariantMap buildClassInfo(const QMetaClassInfo &ci);
QVariantMap buildTree(const QMetaObject *metaObject);


#endif // GRAPHMODELHELPER_H
