#ifndef RPCPEER_H
#define RPCPEER_H
#include "peer.h"
#include <QMetaObject>
#include <QQmlListProperty>
#include <QMutex>
#include "signalproxy.h"

typedef QHash<QString, QObject *> PuppetHash;
class RPCEngine;

typedef QPair<QObject*, QPair<int, quint64>> SignalMapHandle;

class RPCPeer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString nodeName READ nodeName NOTIFY nodeNameChanged)
    Q_PROPERTY(QString appName READ appName NOTIFY appNameChanged)
    Q_PROPERTY(QString appVersion READ appVersion NOTIFY appVersionChanged)
    Q_PROPERTY(Peer* netPeer READ netPeer NOTIFY netPeerChanged)
    Q_PROPERTY(QQmlListProperty<QObject> puppets READ qmlPuppets NOTIFY puppetsChanged)

public:
    friend class RPCEngine;
    explicit RPCPeer(QObject *parent = 0);
    ~RPCPeer();
    RPCPeer(Peer *netPeer, QObject *parent = 0);
    QString nodeName() const;
    QString appName() const;
    QString appVersion() const;

    void setNetPeer(Peer *netPeer);
    void setNodeName(QString arg);
    void setAppName(QString arg);
    void setAppVersion(QString arg);

    void addTypeMapping(int local, int remote);
    void addMetaObject(const QByteArray &name, QMetaObject *mo);
    QObject *addPuppet(const QString &name, QMetaObject *metaObject, quint64 ptr);
    void removePuppet(const QString &name);
    Peer* netPeer() const;

    QQmlListProperty<QObject> qmlPuppets();
    RPCEngine *engine() const;

    void setEngine(RPCEngine *engine);
    void forwardSignal(QObject *sender, int signalIdx, quint64 puppetPtr, int propertyIndex = -1);
    void removeSignalForwarding(QObject *sender, int signalIdx, quint64 puppetPtr);

signals:
    void nodeNameChanged(QString arg);
    void appNameChanged(QString arg);
    void appVersionChanged(QString arg);
    void puppetAdded(QObject *puppet);
    void netPeerChanged(Peer* arg);
    void puppetsChanged();

public slots:
    void reject(QString reason, int code);

private:
    static int qmlPuppetCount(QQmlListProperty<QObject> *list);
    static QObject *qmlPuppetAt(QQmlListProperty<QObject> *list, int index);
    Peer *m_peer;
    RPCEngine *m_engine;
    QHash<int, int> m_localTypeMap;
    QHash<int, int> m_remoteTypeMap;
    QHash<QByteArray, QMetaObject *> m_metaObjects;
    QHash<SignalMapHandle, int> m_forwardedSignals;
    PuppetHash m_puppets;
    QString m_nodeName;
    QString m_appName;
    QString m_appVersion;
    SignalProxy *m_signalProxy;

};

#endif // RPCPEER_H
