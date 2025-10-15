#include "filterobject.h"
#include <QDebug>

FilterObject::FilterObject(QObject *parent): QObject( parent ) {
//    qDebug() << " ==> FO created()" << this;
}

FilterObject::~FilterObject() {
//    qDebug() << " <== FO destroyed()" << this;
}

QVariant FilterObject::criteria() const
{
    return m_criteria;
}

QVariant FilterObject::value() const
{
    return m_value;
}

FilterDefinition *FilterObject::filter() const
{
    return m_filter;
}

QString FilterObject::role() const
{
    return m_role;
}

void FilterObject::setCriteria(QVariant arg)
{
    if (m_criteria == arg)
        return;

    m_criteria = arg;
    emit criteriaChanged(arg);
    emit changed();
}

void FilterObject::setValue(QVariant arg)
{
    m_value = arg;
}

void FilterObject::setFilter(FilterDefinition *arg)
{
    if (m_filter == arg)
        return;

    m_filter = arg;
    emit filterChanged(arg);
    emit changed();
}

void FilterObject::setRole(QString arg)
{
    if (m_role == arg)
        return;

    m_role = arg;
    emit roleChanged(arg);
    emit changed();
}
