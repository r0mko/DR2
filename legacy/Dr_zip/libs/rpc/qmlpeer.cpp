#include "qmlpeer.h"
#include "engine.h"

QmlPeer::QmlPeer(QObject *parent) :
    QObject(parent),
    m_peer(0),
    m_isActive(true)
{

}

RPCPeer *QmlPeer::peer() const
{
    return m_peer;
}

void QmlPeer::setPeer(RPCPeer *arg)
{
    if (m_peer == arg)
        return;

    m_peer = arg;
    emit peerArrived(arg);
}

QString QmlPeer::host() const
{
    return m_url.host();
}

int QmlPeer::port() const
{
    return m_url.port();
}

QUrl QmlPeer::url() const
{
    return m_url;
}

QmlPeer::Mode QmlPeer::mode() const
{
    return m_mode;
}

void QmlPeer::componentComplete()
{
    if (m_url.scheme().isEmpty())
        m_url.setScheme("tcp");
    RPCEngine::engine()->addConnection(m_url, m_isActive, this);
}

bool QmlPeer::isActive() const
{
    return m_isActive;
}

void QmlPeer::setUrl(QUrl arg)
{
    if (m_url == arg)
        return;

    m_url = arg;
    emit urlChanged(arg);
}

void QmlPeer::setMode(QmlPeer::Mode arg)
{
    if (m_mode == arg)
        return;

    m_mode = arg;
    emit modeChanged(arg);
}

void QmlPeer::setActive(bool arg)
{
    if (m_isActive == arg)
        return;

    m_isActive = arg;
    emit isActiveChanged(arg);
}

void QmlPeer::setHost(QString arg)
{
    if (m_url.host() == arg)
        return;

    m_url.setHost(arg);
    emit hostChanged(arg);
    emit urlChanged(m_url);
}

void QmlPeer::setPort(int arg)
{
    if (m_url.port() == arg)
        return;
    m_url.setPort(arg);
    emit portChanged(arg);
    emit urlChanged(m_url);
}
