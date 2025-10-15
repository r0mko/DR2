#include "metanode.h"
#include "graphmodel.h"

#include <QDataStream>
#include <QDebug>

void MetaNodeHandle::addMember(QVariant member)
{
    //    node()->m_members.append(member.value<NodeHandle>().node());
    node()->m_members.append( QPersistentModelIndex( member.value<NodeHandle>().modelIndex() ) );
}

QVariantList MetaNodeHandle::members() const
{
    QVariantList ret;

    node()->m_members.erase( std::remove_if(node()->m_members.begin(),
                                            node()->m_members.end(),
                                            [this](const QPersistentModelIndex &i) { return !i.isValid(); } ),
                             node()->m_members.end() );
    for (auto n : node()->m_members ) {

        NodeHandle i(n);
        ret.append(QVariant::fromValue(i));
    }
    return ret;
}


MetaNodeFactory::MetaNodeFactory(QObject *parent) :
    NodeFactory(parent)
{}


void MetaNodeFactory::setMembers(QVariantList members)
{
    if (m_members == members)
        return;

    m_members = members;

    emit membersChanged();
}

void MetaNodeFactory::addMember(QVariant handle)
{
    //    qDebug() << "=====>> adding handle" << handle;
    m_members.append(handle);
    emit membersChanged();
}

int MetaNodeFactory::nodeMetatypeId() const
{
    return qMetaTypeId<MetaNode>();
}

void MetaNodeFactory::importNode(QDataStream &in, Node *node)
{
    int c;
    in >> c;
    MetaNode *mn = static_cast<MetaNode*>(node);
    while (c--) {
        bool ok;
        QList<int> addr;
        in >> ok >> addr;
        if (ok) {
            model()->requestDeferredMetaNodeMember(mn, addr, rootIndex());
        } else {
            qWarning() << "Unresolved member index" << addr;
        }
    }
    NodeFactory::importNode(in, node);
}

void MetaNodeFactory::exportNode(QDataStream &out, Node *node) const
{
    MetaNode *n = static_cast<MetaNode*>(node);
    out << n->m_members.size();
    for( auto m: n->m_members ){
        //        auto idx = model()->getNodeIndex(m);
        bool ok{true};
        QList<int> addr = model()->iPathRelative(m, rootIndex(), &ok);
        out << ok << addr;
    }
    NodeFactory::exportNode(out, node);
}


void MetaNodeFactory::setupNode(Node *node)
{
    MetaNode* n = static_cast<MetaNode*>(node);
    for (const QVariant& v : m_members)
        n->appendMember( QPersistentModelIndex( v.value<NodeHandle>().modelIndex() ) );
}
