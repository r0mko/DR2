#ifndef GRAPHINDEX_H
#define GRAPHINDEX_H

#include <QAbstractItemModel>
#include <qjsonvalue.h>



class GraphModel;
class Node;
class GraphIndex;
/**
 * @brief Предполагаемая замена NodeHandle
 */
class GraphIndex : public QPersistentModelIndex
{
    Q_GADGET

    Q_PROPERTY(QString nodeId READ nodeId WRITE setNodeId)
    Q_PROPERTY(QVariant data READ data WRITE setData)
    Q_PROPERTY(GraphIndex parent READ parent WRITE setParent)

    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(int childCount READ childCount)
    Q_PROPERTY(int rowIndex READ rowIndex)
    Q_PROPERTY(QVariantList childNodes READ childNodes)
    Q_PROPERTY(QList<int> address READ address)
    Q_PROPERTY(QString mrl READ mrl)
    Q_PROPERTY(QString typeName READ typeName CONSTANT)

    GraphModel *gmodel() const;
    Node *node() const;


public:
//    GraphIndex() : QPersistentModelIndex() {}
//    GraphIndex(const QModelIndex &index) : QPersistentModelIndex(index) {}
//    GraphIndex(const QPersistentModelIndex &other) : QPersistentModelIndex(other) {}

    using QPersistentModelIndex::QPersistentModelIndex;
    //using QPersistentModelIndex::operator =;

    QString nodeId() const;
    QVariant data() const;
    GraphIndex parent() const;
    int childCount() const;
    int rowIndex() const;
    QVariantList childNodes() const;
    QList<int> address() const;
    QString mrl() const;
    QString typeName() const;

    void setNodeId(QString nodeId);
    void setData(QVariant data);
    void setParent(GraphIndex &parent);

    Q_INVOKABLE QVariant getData() const;
    Q_INVOKABLE QVariantList outEdges(QString tagFilter = QString()) const;
    Q_INVOKABLE QVariantList inEdges(QString tagFilter = QString()) const;
    Q_INVOKABLE QVariantList outNodes(QString tagFilter = QString()) const;
    Q_INVOKABLE QVariantList inNodes(QString tagFilter = QString()) const;
    //    GraphIndex &operator=(const QModelIndex &other);
};

Q_DECLARE_METATYPE(GraphIndex)

#endif // GRAPHINDEX_H
