#ifndef NODEMODELFILTER_H
#define NODEMODELFILTER_H

#include <QObject>
#include <QVariant>

class Node;
class NodeFilter : public QObject
{
    Q_OBJECT
public:
    NodeFilter(QObject *parent = 0);
    virtual bool applyFilter(const Node *node) = 0;
};


class NodeTypeFilter : public NodeFilter
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

    QString m_type;

public:
    NodeTypeFilter(QObject *parent = 0);
    virtual ~NodeTypeFilter();
    QString type() const;
    void setType(QString type);

    bool applyFilter(const Node *node);
signals:
    void typeChanged(QString type);

    // NodeFilter interface
};

class NodeAttributeFilter : public NodeFilter
{
    Q_OBJECT
    Q_PROPERTY(QString attributeName READ attributeName WRITE setAttributeName NOTIFY attributeNameChanged)
    Q_PROPERTY(QVariant attributeValue READ attributeValue WRITE setAttributeValue NOTIFY attributeValueChanged)

    QString m_attributeName;
    QVariant m_attributeValue;

public:
    NodeAttributeFilter(QObject *parent = 0);
    virtual ~NodeAttributeFilter();
    QString attributeName() const;
    void setAttributeName(QString attributeName);

    QVariant attributeValue() const;
    void setAttributeValue(QVariant attributeValue);
    bool applyFilter(const Node *node);

public slots:

signals:
    void attributeNameChanged(QString attributeName);
    void attributeValueChanged(QVariant attributeValue);

    // NodeFilter interface
};

class NodeModelFilter : public QObject
{
    Q_OBJECT
public:
    explicit NodeModelFilter(QObject *parent = 0);

signals:

public slots:
};

#endif // NODEMODELFILTER_H
