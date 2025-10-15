#ifndef ABSTRACTMODULE_H
#define ABSTRACTMODULE_H
#include <iostream>
#include <QAbstractListModel>
#include <QObject>
#include <qqmlparserstatus.h>
#include <QMetaType>
#include <node.h>

class GraphModel;
class Node;
class AbstractEdge;

class AbstractModule : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(GraphModel* model READ model CONSTANT)
    Q_PROPERTY(QModelIndex root READ root WRITE setRoot)

    GraphModel *m_model = nullptr;
    QPersistentModelIndex m_root;

public:
    explicit AbstractModule(QObject *parent = 0);

    void componentComplete();
    void classBegin();
    GraphModel *model() const;

    virtual QVector<int> nodeMetatypes() const = 0;
    virtual QVector<int> edgeMetatypes() const = 0;

public slots:
protected:
    template <class N, class H = typename HandleType<N>::type>
    H createNodeHelper(NodeHandle handle = NodeHandle());

    const QModelIndex &root() const;
    void setRoot(const QModelIndex &root);
};

#endif // ABSTRACTMODULE_H
