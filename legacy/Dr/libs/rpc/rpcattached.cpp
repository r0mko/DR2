#include "rpcattached.h"
#include "engine.h"
#include "qdebug.h"
RPCAttached::RPCAttached(QObject *parent) :
    QObject(parent)
{
    qDebug() << "Consturcted" << this << "as parent of" << parent;
}



bool RPCAttached::allowOfflineMode() const
{
    return m_allowOfflineMode;
}

bool RPCAttached::online() const
{
    return m_online;
}

RPCEngine *RPCAttached::engine() const
{
    return RPCEngine::engine();
}

bool RPCAttached::shared() const
{
    return m_shared;
}

QString RPCAttached::name() const
{
    return m_name;
}

void RPCAttached::setAllowOfflineMode(bool arg)
{
    if (m_allowOfflineMode == arg)
        return;

    m_allowOfflineMode = arg;
    emit allowOfflineModeChanged(arg);
}

void RPCAttached::setOnline(bool arg)
{
    if (m_online == arg)
        return;

    m_online = arg;
    emit onlineChanged(arg);
}

void RPCAttached::setShared(bool arg)
{
    if (m_shared == arg)
        return;

    m_shared = arg;
    if (m_shared)
        RPCEngine::engine()->registerObject(parent());
    else
        emit sharedChanged(arg);
}
