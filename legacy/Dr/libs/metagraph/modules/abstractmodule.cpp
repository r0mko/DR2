#include "abstractmodule.h"
#include "graphmodel.h"







AbstractModule::AbstractModule(QObject *parent) :
    QObject(parent)
{}

GraphModel *AbstractModule::model() const
{
    return m_model;
}

const QModelIndex &AbstractModule::root() const
{
    return m_root.operator const QModelIndex &();
}

void AbstractModule::setRoot(const QModelIndex &root)
{
    m_root = root;
}

void AbstractModule::componentComplete()
{
    m_model = qobject_cast<GraphModel*>(parent());
}

void AbstractModule::classBegin()
{
    m_model = qobject_cast<GraphModel*>(parent());
}

