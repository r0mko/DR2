#ifndef MODELWATCHER_H
#define MODELWATCHER_H

#include <QJSValue>
#include <QObject>
#include <QQmlPropertyMap>
#include "dynamicobjectextension.h"
#include "node.h"
struct NId;

class ModelWatcher : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(GraphModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString mrl READ mrl WRITE setMrl NOTIFY mrlChanged)
    Q_PROPERTY(QJSValue handle READ handle NOTIFY handleChanged)

//    Q_PROPERTY(ModelAgent* modelData READ modelData NOTIFY modelDataChanged)
    Q_PROPERTY(bool linked READ linked NOTIFY linkedChanged)
    Q_PROPERTY(QModelIndex modelIndex READ modelIndex WRITE setModelIndex NOTIFY modelIndexChanged)
    Q_PROPERTY(QVariantList children READ children NOTIFY childrenChanged)
    Q_PROPERTY(QQmlPropertyMap* namedChildren READ namedChildren NOTIFY namedChildrenChanged)
    Q_PROPERTY(NodeChildrenModel* childrenModel READ childrenModel NOTIFY childrenModelChanged)
    Q_PROPERTY(QStringList childIds READ childIds NOTIFY childIdsChanged)

    GraphModel* m_model = nullptr;
    QString m_mrl;
    bool mrlWatcher = false;
    QQmlPropertyMap* m_namedChildren { new QQmlPropertyMap(this) };
    NodeChildrenModel* m_childrenModel;

protected:
    QPersistentModelIndex i;
public:
    explicit ModelWatcher(QObject *parent = 0);
    GraphModel *model() const;
    QString mrl() const;
//    ModelAgent* modelData() const;
    bool linked() const;
    QModelIndex modelIndex() const;
    QVariantList children() const;
    void classBegin() {}
    void componentComplete();
    QJSValue handle() const;
    QQmlPropertyMap* namedChildren() const;

    NodeChildrenModel* childrenModel();

    QStringList childIds() const;

signals:
    void modelChanged(GraphModel* model);
    void mrlChanged();
//    void modelDataChanged();
    void linkedChanged(bool linked);
    void modelIndexChanged(QModelIndex modelIndex);
    void childrenChanged();
    void handleChanged();
    void namedChildrenChanged();
    void childrenModelChanged();
    void childIdsChanged();

public slots:
    void setModel(GraphModel* model);
    void setMrl(QString mrl);
    void setModelIndex(QModelIndex index);

protected:
    void childrenDataChange(int first, int last, const QVector<int> &roles);
    virtual void dataChange(const QVector<int> &);
    virtual void invalidate() {}
    virtual void modelChange(GraphModel *) {}

private slots:
    void onDataChanged          (const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    //    void onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onRowsInserted         (const QModelIndex &parent, int, int);
    void onRowsAboutToBeRemoved (const QModelIndex &parent, int first, int last);
    void onRowsRemoved          (const QModelIndex &parent, int, int);
    //    void onRowsAboutToBeMoved   (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);
    //    void onRowsMoved            (const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow);
    void onModelAboutToBeReset  ();
    void onModelReset           ();
    void resolveMrl();
    void resetLink();
private:
    bool checkMrl();
    void initNamedChildren();
    QStringList m_childIds;
};

#endif // MODELWATCHER_H
