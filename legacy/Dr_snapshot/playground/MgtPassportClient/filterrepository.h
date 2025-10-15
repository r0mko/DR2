#ifndef FILTERREPOSITORY_H
#define FILTERREPOSITORY_H

#include <QObject>
#include "filterdefinition.h"
#include <QQmlListProperty>

class FilterRepository : public QObject
{
    Q_OBJECT

    Q_CLASSINFO("DefaultProperty", "filters")

    Q_PROPERTY(QQmlListProperty<FilterDefinition> filters READ qmlFilters NOTIFY filtersChanged)

    QList<FilterDefinition*> m_filters;
    QQmlListProperty<FilterDefinition> qmlFilters();
    static void qmlFiltersAppend(QQmlListProperty<FilterDefinition> *list, FilterDefinition *object);
    static int qmlFiltersCount(QQmlListProperty<FilterDefinition> *list);
    static FilterDefinition *qmlFiltersAt(QQmlListProperty<FilterDefinition> *list, int index);

public:
    explicit FilterRepository(QObject *parent = 0);
    ~FilterRepository();

signals:
    void filtersChanged();

};

#endif // FILTERREPOSITORY_H
