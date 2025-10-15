#ifndef XMLHANDLER_H
#define XMLHANDLER_H

#include <QObject>
#include <QDebug>
#include <QXmlDefaultHandler>
#include "transportsimulator.h"

class XmlHandler : public QXmlDefaultHandler
{    
public:
    XmlHandler(TransportSimulator* _ts);

public:
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    QString errorString() const;
    QString errorMessage;
    TransportSimulator* ts = nullptr;

    unsigned long long relationId = 0;
    QList<TransportSimulator::Member> relationMembers;
    QList<TransportSimulator::Tag> relationTags;

    unsigned long long wayId = 0;
    QList<TransportSimulator::Tag> wayTags;
    QList<unsigned long long> wayNodes;
};

#endif // XMLHANDLER_H
