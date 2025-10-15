#ifndef UNILOADER_H
#define UNILOADER_H

#include <QObject>
#include <QQuickItem>
#include "targetdef.h"
#include "unidelegate.h"
#include <QQmlProperty>

class UniLoader : public QQuickItem
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(TargetDef* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString targetName READ targetName WRITE setTargetName NOTIFY targetNameChanged)
    Q_PROPERTY(QString variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(bool allowSuperClassDelegates READ allowSuperClassDelegates WRITE setAllowSuperClassDelegates NOTIFY allowSuperClassDelegatesChanged)

    Q_PROPERTY(UniDelegate* delegateItem READ delegateItem NOTIFY delegateItemChanged)

    Q_PROPERTY(bool isList READ isList NOTIFY isListChanged)
    Q_PROPERTY(QQmlListProperty<QObject> listValue READ qmlListValue NOTIFY listValueChanged)
    Q_PROPERTY(QString listItemType READ listItemType NOTIFY listItemTypeChanged)

    Q_PROPERTY(QString valueSource READ valueSource WRITE setValueSource NOTIFY valueSourceChanged)


public:
    explicit UniLoader(QQuickItem *parent = 0);
    ~UniLoader();
    void classBegin();
    void componentComplete();

    QVariant value() const;
    TargetDef* target() const;
    QString variant() const;
    bool allowSuperClassDelegates() const;
    UniDelegate *delegateItem() const;
    QString targetName() const;
    QQmlListProperty<QObject> qmlListValue();
    bool isList() const;
    QString listItemType() const;

    QString valueSource() const;

signals:
    void valueChanged();
    void targetChanged();
    void variantChanged();
    void allowSuperClassDelegatesChanged();
    void delegateItemChanged();
    void targetNameChanged();
    void listValueChanged();
    void isListChanged();
    void listItemTypeChanged();

    void valueSourceChanged(QString arg);

public slots:
    void setValue(QVariant arg);
    void setTarget(TargetDef *arg);
    void setVariant(QString arg);
    void setAllowSuperClassDelegates(bool arg);
    void setTargetName(QString arg);
    void setValueSource(QString arg);

private slots:
    void updateImplicitWidth();
    void updateImplicitHeight();

    void updateItem();
    void completeTarget();
    void updateValue();
    void writeValueSource();

private:
    QVariant m_value;
    TargetDef* m_target;

    QString m_variant;
    UniDelegate *m_delegateItem;
    bool m_allowSuperClassDelegates;
    QString m_className;
    QString m_propertyName;
    QString m_targetName;
    QQmlListReference listRef;
    bool m_isList;
    QString m_listItemType;

    static void qmlListDataAppend(QQmlListProperty<QObject> *list, QObject *object);
    static int qmlListDataCount(QQmlListProperty<QObject> *list);
    static QObject *qmlListDataAt(QQmlListProperty<QObject> *list, int index);

    QQmlProperty m_valueSource;
    QString m_valueSourceExpression;

    QObject *sourceObject;
};


#endif // UNILOADER_H
