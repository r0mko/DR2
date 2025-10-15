#include "xmlhandler.h"

XmlHandler::XmlHandler(TransportSimulator *_ts) : ts(_ts)
{
}

bool XmlHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if(!ts)
    {
        errorMessage = "ts is null!";
        return false;
    }

    if(qName == "node")
    {
        QString id = atts.value("id");
        QString lat = atts.value("lat");
        QString lon = atts.value("lon");
        if(id.isEmpty() || lat.isEmpty() || lon.isEmpty())
        {

            errorMessage = "startElement - 'node' : id or lat or lon is empty!";
            return false;
        }
        TransportSimulator::Node node;
        bool ok = false;
        node.id = id.toULongLong(&ok);
        node.lat = lat.toDouble(&ok);
        node.lon = lon.toDouble(&ok);
        if(!ok)
        {
            errorMessage = "startElement - 'node' : data conversion error!";
            return false;
        }
        ts->nodes.append(node);
    }
    else if(qName == "way")
    {
        if(wayId > 0)
        {
            errorMessage = "startElement - 'way' : start before it ended!";
            return false;
        }
        QString id = atts.value("id");
        if(id.isEmpty())
        {
            errorMessage = "startElement - 'way' : id is empty!";
            return false;
        }
        bool ok = false;
        wayId = id.toULongLong(&ok);
        if(!ok)
        {
            errorMessage = "startElement - 'way' : data conversion error!";
            return false;
        }
    }
    else if(qName == "nd" && wayId > 0)
    {
        QString id = atts.value("ref");
        if(id.isEmpty())
        {
            errorMessage = "startElement - 'nd' : ref is empty!";
            return false;
        }
        bool ok = false;
        unsigned long long ullId = id.toULongLong(&ok);
        if(!ok)
        {
            errorMessage = "startElement - 'nd' : data conversion error!";
            return false;
        }
        wayNodes.append(ullId);
    }
    else if(qName == "relation")
    {
        if(relationId > 0)
        {
            errorMessage = "startElement - 'relation' : start before it ended!";
            return false;
        }
        QString id = atts.value("id");
        if(id.isEmpty())
        {
            errorMessage = "startElement - 'relation' : id is empty!";
            return false;
        }
        bool ok = false;
        relationId = id.toULongLong(&ok);
        if(!ok)
        {
            errorMessage = "startElement - 'relation' : data conversion error!";
            return false;
        }
    }
    else if(qName == "member" && relationId > 0)
    {
        QString type = atts.value("type");
        QString ref = atts.value("ref");
        QString role = atts.value("role");
        if(type.isEmpty() || ref.isEmpty())
        {
            errorMessage = "startElement - 'member' : type or ref is empty!";
            return false;
        }
        TransportSimulator::Member member;
        bool ok = false;
        member.type = type;
        member.ref = ref.toULongLong(&ok);
        member.role = role;
        if(!ok)
        {
            errorMessage = "startElement - 'member' : data conversion error!";
            return false;
        }
        relationMembers.append(member);
    }
    else if(qName == "tag" && relationId > 0)
    {
        QString k = atts.value("k");
        QString v = atts.value("v");
        if(k.isEmpty() || v.isEmpty())
        {
            errorMessage = "startElement - 'tag - relationId' : k or v is empty!";
            return false;
        }
        TransportSimulator::Tag tag;
        tag.k = k;
        tag.v = v;
        relationTags.append(tag);
    }
    else if(qName == "tag" && wayId > 0)
    {
        QString k = atts.value("k");
        QString v = atts.value("v");
        if(k.isEmpty() || v.isEmpty())
        {
            errorMessage = "startElement - 'tag - wayId' : k or v is empty!";
            return false;
        }
        TransportSimulator::Tag tag;
        tag.k = k;
        tag.v = v;
        wayTags.append(tag);
    }

    return true;
}

bool XmlHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if(qName == "way")
    {
        if(wayId == 0)
        {
            errorMessage = "endElement - 'way' : end before it started";
            return false;
        }
        if(wayNodes.empty())
        {
            errorMessage = "endElement - 'way' : way nodes is empty";
            return false;
        }
        TransportSimulator::Way way;
        way.id = wayId;
        way.nd.swap(wayNodes);
        way.tag.swap(wayTags);
        ts->ways.append(way);

        wayId = 0;
        wayNodes.clear();
        wayTags.clear();
    }
    else if(qName == "relation")
    {
        if(relationId == 0)
        {
            errorMessage = "endElement - 'relation' : end before it started";
            return false;
        }
        if(relationMembers.empty())
        {
            errorMessage = "endElement - 'relation' : relationMembers is empty";
            return false;
        }
        if(relationTags.empty())
        {
            qDebug() << "endElement - 'relation' : relationTags is empty";
        }
        TransportSimulator::Relation relation;
        relation.id = relationId;
        relation.member.swap(relationMembers);
        relation.tag.swap(relationTags);

        //BLACK LIST
//        if(relation.id != 3241077)
//            ts->relations.append(relation);
        //END OF BLACK LIST

        ts->relations.append(relation);

        relationId = 0;
        relationMembers.clear();
        relationTags.clear();
    }

    return true;
}

QString XmlHandler::errorString() const
{
    return errorMessage;
}


