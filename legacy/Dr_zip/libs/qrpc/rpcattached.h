#ifndef RPCATTACHED_H
#define RPCATTACHED_H

#include <QObject>
#include <QtQml>

class RPCEngine;
class RPCAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool allowOfflineMode READ allowOfflineMode WRITE setAllowOfflineMode NOTIFY allowOfflineModeChanged)
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)
    Q_PROPERTY(RPCEngine* engine READ engine CONSTANT)
    Q_PROPERTY(bool shared READ shared WRITE setShared NOTIFY sharedChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    bool m_allowOfflineMode;
    bool m_online;
    bool m_shared;
    QString m_name;

public:
    explicit RPCAttached(QObject *parent = 0);

    bool allowOfflineMode() const;
    bool online() const;
    RPCEngine *engine() const;
    bool shared() const;
    QString name() const;

signals:
    void allowOfflineModeChanged(bool arg);
    void onlineChanged(bool arg);
    void sharedChanged(bool arg);
    void nameChanged(QString arg);

public slots:
    void setAllowOfflineMode(bool arg);
    void setOnline(bool arg);
    void setShared(bool arg);
    void setName(QString arg)
    {
        if (m_name == arg)
            return;

        m_name = arg;
        emit nameChanged(arg);
    }
};




#endif // RPCATTACHED_H
