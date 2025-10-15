#ifndef FILTERDEFINITION_H
#define FILTERDEFINITION_H

#include <QObject>
#include <QJSValue>

class FilterObject;

class FilterDefinition : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QJSValue compareFunction READ compareFunction WRITE setCompareFunction NOTIFY compareFunctionChanged)

public:
    explicit FilterDefinition(QObject *parent = 0);
    ~FilterDefinition();

    bool evalCompare(FilterObject *instance);


private: // properties
    QJSValue m_compareFunction;
    QString m_text;

public: // getters
    QJSValue compareFunction() const;
    QString text() const;

signals: // signals
    void compareFunctionChanged();
    void textChanged(QString arg);

public slots: // setters
    void setCompareFunction(QJSValue arg);
    void setText(QString arg);
};

#endif // FILTERDEFINITION_H
