#include "filterdefinition.h"
#include "filterobject.h"
#include <QQmlEngine>
#include <qqml.h>

FilterDefinition::FilterDefinition(QObject *parent) : QObject(parent)
{

}

FilterDefinition::~FilterDefinition()
{

}

bool FilterDefinition::evalCompare(FilterObject *instance)
{
    if (!m_compareFunction.isCallable()) return true;
    QJSValue thisobj = qmlEngine(this)->newQObject(instance);
    return m_compareFunction.callWithInstance(thisobj).toBool();
}

QJSValue FilterDefinition::compareFunction() const
{
    return m_compareFunction;
}

QString FilterDefinition::text() const
{
    return m_text;
}

void FilterDefinition::setCompareFunction(QJSValue arg)
{
    if (m_compareFunction.isCallable()) return;
    m_compareFunction = arg;
    emit compareFunctionChanged();
}

void FilterDefinition::setText(QString arg)
{
    if (m_text == arg)
        return;

    m_text = arg;
    emit textChanged(arg);
}

