#ifndef FILTEROBJECT_H
#define FILTEROBJECT_H

#include <QObject>
#include <QVariant>

class FilterDefinition;

class FilterObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant criteria READ criteria WRITE setCriteria NOTIFY criteriaChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(FilterDefinition* filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString role READ role WRITE setRole NOTIFY roleChanged)
    QVariant m_criteria;
    QVariant m_value;
    FilterDefinition *m_filter;
    QString m_role;

public:
    FilterObject( QObject *parent = nullptr );
    ~FilterObject();

    QVariant criteria() const;
    QVariant value() const;
    FilterDefinition *filter() const;
    QString role() const;

public slots:
    void setCriteria(QVariant arg);
    void setValue(QVariant arg);
    void setFilter(FilterDefinition *arg);
    void setRole(QString arg);

signals:
    void criteriaChanged(QVariant arg);
    void filterChanged(FilterDefinition *arg);
    void roleChanged(QString arg);
    void changed();
};
#endif // FILTEROBJECT_H
