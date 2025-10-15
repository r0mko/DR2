#ifndef BASEMODULE_H
#define BASEMODULE_H

#include <QObject>
#include <node.h>
#include "abstractmodule.h"

class BaseModule : public AbstractModule
{
    Q_OBJECT
    Q_PROPERTY(QString nodeId READ nodeId WRITE setNodeId NOTIFY nodeIdChanged)
    Q_PROPERTY(NodeHandle parent READ parentNode WRITE setParentNode NOTIFY parentChanged)
    QString m_nodeId;

public:
    explicit BaseModule(QObject *parent = 0);

    void setParentNode(NodeHandle parent);
    void setNodeId(QString nodeId);

signals:
    void parentChanged(NodeHandle parent);
    void nodeIdChanged(QString nodeId);

public:
    QVector<int> nodeMetatypes() const;
    QVector<int> edgeMetatypes() const;

    NodeHandle parentNode() const;
    QString nodeId() const;

protected:

};

#endif // BASEMODULE_H
