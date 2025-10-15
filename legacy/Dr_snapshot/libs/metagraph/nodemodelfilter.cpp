#include "nodemodelfilter.h"

#include <node.h>

NodeFilter::NodeFilter(QObject *parent) : QObject(parent) { }

NodeTypeFilter::NodeTypeFilter(QObject *parent) : NodeFilter(parent) { }

NodeTypeFilter::~NodeTypeFilter() {}

QString NodeTypeFilter::type() const
{
    return m_type;
}

void NodeTypeFilter::setType(QString type)
{
    if (m_type == type)
        return;
    m_type = type;
    emit typeChanged(type);
}

NodeAttributeFilter::NodeAttributeFilter(QObject *parent) : NodeFilter(parent) { }

NodeAttributeFilter::~NodeAttributeFilter() { }

QString NodeAttributeFilter::attributeName() const
{
    return m_attributeName;
}

void NodeAttributeFilter::setAttributeName(QString attributeName)
{
    if (m_attributeName == attributeName)
        return;

    m_attributeName = attributeName;
    emit attributeNameChanged(attributeName);
}

QVariant NodeAttributeFilter::attributeValue() const
{
    return m_attributeValue;
}

void NodeAttributeFilter::setAttributeValue(QVariant attributeValue)
{
    if (m_attributeValue == attributeValue)
        return;

    m_attributeValue = attributeValue;
    emit attributeValueChanged(attributeValue);
}


bool NodeTypeFilter::applyFilter(const Node *node)
{
//    qDebug() << "Applying node filter" << this << m_type << QMetaType::typeName(node->typeId());
    return qstrcmp(QMetaType::typeName(node->typeId()), m_type.toLatin1()) == 0;
}


bool NodeAttributeFilter::applyFilter(const Node *node)
{
//    qDebug() << "Applying node filter" << this << m_attributeName << "value" << m_attributeValue;
    if (m_attributeName.isEmpty())
        return true;
    QVariantMap m = node->data().toMap();
    if (m.contains(m_attributeName)) {
        if (!m_attributeValue.isValid())
            return true;
        return m.value(m_attributeName) == m_attributeValue;
    }
    return false;
}
