#include "defs.h"
#include "stylenode.h"

#include <QColor>
#include <QJsonObject>
#include <QMetaProperty>
#include <QSGNode>

void StyleNode::init(const VertexBuffer &geometry)
{
//    qDebug() << "initialize geometry" << geometry.vertexCount() << geometry.fillIndexCount();
    initBuffer(geometry);
    for (QVariant v : d->children) {
        StyleNode *childStyle = fromVariant(v);
        childStyle->init(updateBuffer(geometry));
    }
}

QSGNode *StyleNode::updateNode(QSGNode *node, const VertexBuffer &geometry)
{
//    qDebug() << "Updating node" << node << geometry.vertexCount();
    node = updateStyleNode(node, geometry);
//    if node->childCount();
    QSGNode *childNode = nullptr;

    if (node->childCount() != d->children.size()) {
        qDebug() << "buffers and children mismatch, childcount" << node->childCount() << "geom count" << d->children.size();
        node->removeAllChildNodes();
    }
    if (node && node->childCount() > 0)
        childNode = node->firstChild();
    for (auto it = d->children.begin(); it != d->children.end(); ++it) {
        StyleNode *s = fromVariant(*it);
        if (s->deleted()) {
            qDebug() << "deleting deleted node" << *it << childNode;
            if (childNode) {
                node->removeChildNode(childNode);
                childNode = childNode->nextSibling();
            }
            it = d->children.erase(it);
            if (it == d->children.end())
                break;
            continue;
        }
        QSGNode *newChild = s->updateNode(childNode, updateBuffer(geometry));
        if (!childNode)
            node->appendChildNode(newChild);
        else if (childNode != newChild) {
            node->insertChildNodeAfter(newChild, childNode);
            node->removeChildNode(childNode);
        }
        childNode = newChild->nextSibling();
    }
    return node;
}

void StyleNode::appendChild(const QVariant &child)
{
    d->children.append(child);
    qDebug() << "Add child style" << child << child.canConvert<StyleNode>();
//    if (m_node)
//        m_node->markDirty(QSGNode::DirtyNodeAdded);
}

void StyleNode::insertChild(const QVariant &child, int index)
{
    d->children.insert(index, child);
}

void StyleNode::moveChildren(int start, int end, int newStart)
{
    qDebug() << "implement moving children";
}

void StyleNode::removeChildren(int start, int end)
{
    for (int i = start; i <= end; ++i) {
        const QVariant &v = d->children.at(i);
        fromVariant(v)->setDeleted(true);
    }
}

QSGNode *StyleNode::node() const
{
    return m_node;
}

void StyleNode::setDeleted(bool deleted)
{
    d->deleted = deleted;
}


QSGGeometryNode *StyleNode::getGeometryNode(QSGNode *node)
{
    QSGGeometryNode *n;
    if (!node || node->type() != QSGNode::GeometryNodeType) {
//        qDebug() << "Creating new geometry node" << node;
        m_node = new QSGGeometryNode;
        m_node->setFlags(QSGNode::OwnedByParent | QSGNode::OwnsGeometry | QSGNode::OwnsMaterial );
    } else {
        n = static_cast<QSGGeometryNode*>(node);
        if (m_node && m_node != n) {
            qWarning() << "Node live-changed!\n\told:" << m_node << "\n\tnew:" << n << "\nTHIS SHOULD NEVER HAPPEN!";
        }
        m_node = n;
    }
    return static_cast<QSGGeometryNode*>(m_node);
}

QSGOpacityNode *StyleNode::getOpacityNode(QSGNode *node)
{
    QSGOpacityNode *n{nullptr};
    if ( node == nullptr || node->type() != QSGNode::OpacityNodeType ) {
        qDebug() << "Creating new opacity node" << node;
        m_node = new QSGOpacityNode;
        m_node->setFlag(QSGNode::OwnedByParent);

    } else {
        n = static_cast<QSGOpacityNode*>(node);
        if (m_node && m_node != n) {
            qWarning() << "Node live-changed!\n\told:" << m_node << "\n\tnew:" << n << "\nTHIS SHOULD NEVER HAPPEN!";
        }
        m_node = n;
    }
    return static_cast<QSGOpacityNode*>(m_node);
}

StyleNode *StyleNode::fromVariant(const QVariant &variant)
{
    return const_cast<StyleNode*>(static_cast<const StyleNode*>(variant.data()));
}


//bool StyleNode::updateProperty(void *gadget, const QByteArray &property, const QJsonValue &value, const QMetaObject *metaObject) const
//{
//    int idx = metaObject->indexOfProperty(property.constData());
//    if (idx < 0) {
//        qWarning() << "no property" << property << "in" << metaObject->className();
//        return false;
//    }
//    QMetaProperty prop = metaObject->property(idx);
//    QVariant current = prop.readOnGadget(gadget);
//    QVariant newVal;
//    switch (prop.type()) {
//    case QVariant::Double:
//        newVal = value.toDouble();
//        break;
//    case QVariant::Color:
//        newVal = QColor(value.toString());
//        break;
//    case QVariant::Int:
//        newVal = value.toInt();
//        break;
//    default:
//        qWarning() << "updating property type" << prop.typeName() << "is not supported!";
//        return false;
//        break;
//    }
//    if (current == newVal)
//        return false;
//    prop.writeOnGadget(gadget, newVal);
//    return true;
//}


StyleNodeData::~StyleNodeData()
{
    qDebug() << "style shared data destroyed";
}
