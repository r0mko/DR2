#ifndef TARGETDEF_H
#define TARGETDEF_H

#include <QObject>
#include <QQmlComponent>
#include <QQmlParserStatus>

class TypeDelegate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString typeName READ typeName WRITE setTypeName NOTIFY typeNameChanged)
    Q_PROPERTY(QQmlComponent* component READ component WRITE setComponent NOTIFY componentChanged)
    Q_PROPERTY(QString variantName READ variantName WRITE setVariantName NOTIFY variantNameChanged)
    Q_PROPERTY(QStringList variantNames READ variantNames WRITE setVariantNames NOTIFY variantNamesChanged)
    Q_PROPERTY(QVariant extraData READ extraData WRITE setExtraData NOTIFY extraDataChanged)

    QString m_typeName;
    QString m_variantName;
    QQmlComponent* m_component;
    QVariant m_extraData;
    QStringList m_variantNames;

public:
    TypeDelegate();
    virtual ~TypeDelegate() {}
    QString typeName() const;
    QQmlComponent *component() const;
    QString variantName() const;
    QVariant extraData() const;
    void setTypeName(QString arg);
    void setComponent(QQmlComponent* arg);
    void setVariantName(QString arg);
    void setVariantNames(QStringList arg);
    void setExtraData(QVariant arg);
    QStringList variantNames() const;

public slots:

signals:
    void typeNameChanged();
    void variantNameChanged();
    void componentChanged();
    void extraDataChanged();
    void variantNamesChanged();
};

class TargetDef : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<TypeDelegate> defs READ qmlDefs NOTIFY delegatesChanged)
    Q_PROPERTY(QString targetName READ targetName WRITE setTargetName NOTIFY targetNameChanged)

    Q_CLASSINFO("DefaultProperty", "defs")

public:
    explicit TargetDef(QObject *parent = 0);
    QQmlListProperty<TypeDelegate> qmlDefs();
    QString targetName() const;
    void componentComplete();
    void classBegin() {}
    void init() const;
    bool isComplete() const;

signals:
    void delegatesChanged();
    void targetNameChanged();
    void buildComplete();

public slots:
    void setTargetName(QString arg);

private:
    static void qmlAppendDelegate(QQmlListProperty<TypeDelegate> *list, TypeDelegate *delegate);
    static int qmlDelegatesCount(QQmlListProperty<TypeDelegate> *list);
    static TypeDelegate *qmlDelegateAt(QQmlListProperty<TypeDelegate> *list, int index);

    QString m_targetName;
    bool m_isComplete;
    QList<TypeDelegate*> m_delegates;
};

#endif // TARGETDEF_H
