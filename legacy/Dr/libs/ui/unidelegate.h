#ifndef UNIDELEGATE_H
#define UNIDELEGATE_H
#include <QObject>
#include <QQuickItem>
#include <QVariant>

//#include "uniloader.h"
class UniLoader;
class UniDelegate : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVariant extraData READ extraData NOTIFY extraDataChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged STORED false)
    Q_PROPERTY(bool isList READ isList NOTIFY isListChanged STORED false)
    Q_PROPERTY(QQmlListProperty<QObject> listValue READ qmlListValue NOTIFY listValueChanged STORED false)
    Q_PROPERTY(QString listItemType READ listItemType NOTIFY listItemTypeChanged STORED false)
    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY isCompletedChanged)



public:
    UniDelegate(QQuickItem *parent = 0);
    ~UniDelegate() {}

    void classBegin();
    void componentComplete();

    QVariant extraData() const;
    QVariant value() const;
    bool isList() const;
    QQmlListProperty<QObject> qmlListValue() const;
    QString listItemType() const;
    void setExtraData(QVariant arg);

    bool isCompleted() const;

public slots:
    void setValue(QVariant arg);

signals:
    void extraDataChanged();
    void valueChanged();
    void isListChanged();
    void listValueChanged();
    void listItemTypeChanged();
    void isCompletedChanged(bool arg);

private:
    UniLoader *loader;
    QVariant m_extraData;
};

#endif // UNIDELEGATE_H
