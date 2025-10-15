#ifndef QMLPEER_H
#define QMLPEER_H

#include <QObject>
#include "rpcpeer.h"

class QmlPeer : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(Mode)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(RPCPeer* peer READ peer NOTIFY peerArrived)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged STORED false)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged STORED false)
    Q_PROPERTY(bool isActive READ isActive WRITE setActive NOTIFY isActiveChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)

public:
    enum Mode
    {
        ReadWrite,
        ReadOnly
    };
    explicit QmlPeer(QObject *parent = 0);
    RPCPeer* peer() const;
    QUrl url() const;
    Mode mode() const;
    void classBegin() {}
    void componentComplete();
    bool isActive() const;
    void setPeer(RPCPeer* arg);

    QString host() const;

    int port() const;

signals:
    void peerArrived(RPCPeer* arg);
    void urlChanged(QUrl arg);
    void modeChanged(Mode arg);
    void isActiveChanged(bool arg);
    void hostChanged(QString arg);
    void portChanged(int arg);

public slots:
    void setUrl(QUrl arg);
    void setMode(Mode arg);
    void setActive(bool arg);
    void setHost(QString arg);
    void setPort(int arg);

private:

    RPCPeer* m_peer;
    QUrl m_url;
    Mode m_mode;
    bool m_isActive;
};

#endif // QMLPEER_H
