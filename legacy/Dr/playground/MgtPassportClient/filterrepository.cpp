#include "filterrepository.h"

QQmlListProperty<FilterDefinition> FilterRepository::qmlFilters() {
    return QQmlListProperty<FilterDefinition>(this, 0,
                                              &FilterRepository::qmlFiltersAppend,
                                              &FilterRepository::qmlFiltersCount,
                                              &FilterRepository::qmlFiltersAt,
                                              QQmlListProperty<FilterDefinition>::ClearFunction());
}

void FilterRepository::qmlFiltersAppend(QQmlListProperty<FilterDefinition> *list, FilterDefinition *object)
{
    FilterRepository *o = qobject_cast<FilterRepository*>(list->object);
    o->m_filters.append(object);
    emit o->filtersChanged();
}

int FilterRepository::qmlFiltersCount(QQmlListProperty<FilterDefinition> *list)
{
    FilterRepository *o = qobject_cast<FilterRepository*>(list->object);
    return o->m_filters.size();
}

FilterDefinition *FilterRepository::qmlFiltersAt(QQmlListProperty<FilterDefinition> *list, int index)
{
    FilterRepository *o = qobject_cast<FilterRepository*>(list->object);
    return o->m_filters.at(index);
}

FilterRepository::FilterRepository(QObject *parent) : QObject(parent)
{

}

FilterRepository::~FilterRepository()
{

}
