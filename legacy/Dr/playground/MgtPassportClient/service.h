#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QVariantMap>
//#include "partmodel.h"
class PartModel;
class QThread;
class ServiceWorker;

class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(bool autoReconnect READ autoReconnect WRITE setAutoReconnect NOTIFY autoReconnectChanged)
    Q_PROPERTY(bool databaseOnline READ databaseOnline NOTIFY databaseOnlineChanged)
    Q_PROPERTY(bool authenticated READ authenticated NOTIFY authenticatedChanged)
    Q_PROPERTY(QVariantMap userInfo READ userInfo NOTIFY userInfoChanged)
    Q_PROPERTY(bool canRead READ canRead WRITE setCanRead NOTIFY canReadChanged)
    Q_PROPERTY(bool canEdit READ canEdit WRITE setCanEdit NOTIFY canEditChanged)
    Q_PROPERTY(bool canManageUsers READ canManageUsers WRITE setCanManageUsers NOTIFY canManageUsersChanged)

private:
    QThread* m_thread;
    ServiceWorker* m_serviceWorker;
    bool m_busy;
    PartModel *m_model;
    PartModel *m_modelNext;
    QHash< PartModel*, PartModel* > m_dirtyModelQueue;

public:
    explicit Service(QObject *parent = 0);
    ~Service();

signals:
    void sendMessage( QVariantMap message );
    /**/    void dirtyQueue();
//    void authenticated( QVariantMap message );

private slots:
    void serviceWorkerConnected();
    void serviceWorkerDisconnected();
    void onMessageRecieved( const QVariantMap& message );
    void processInfoMessage( const QVariantMap& message );
    void processLoginReplyMessage( const QVariantMap& message );
    void processErrorMessage( const QVariantMap& message );
    void processLogoutReplyMessage( const QVariantMap& message );
    void processDataReplyMessage( const QVariantMap& message );
    /**/    void onDirtyQueue();




public slots:
    void connectToHost();
    void disconnectFromHost();
    void login( QString user, QString pass );
    void logout();
    void stopServer();
    void refreshServerSettings();
    void onDataRequestByModel(PartModel* model );
    void queueRequest(PartModel* model);



// ------------- property stuff here --------------- //

private: // property members
    QString m_host;
    quint16 m_port;
    bool m_isConnected;
    bool m_autoReconnect;
    bool m_databaseOnline;
    bool m_authenticated;

    QVariantMap m_userInfo;

    bool m_canRead;

    bool m_canEdit;

bool m_canManageUsers;

public: // property getters
    QString host() const
    {
        return m_host;
    }

    quint16 port() const
    {
        return m_port;
    }

    bool isConnected() const
    {
        return m_isConnected;
    }

    bool autoReconnect() const
    {
        return m_autoReconnect;
    }

    bool databaseOnline() const
    {
        return m_databaseOnline;
    }
    bool authenticated() const
    {
        return m_authenticated;
    }

    QVariantMap userInfo() const
    {
        return m_userInfo;
    }

    bool canRead() const
    {
        return m_canRead;
    }

    bool canEdit() const
    {
        return m_canEdit;
    }

    bool canManageUsers() const
    {
        return m_canManageUsers;
    }

signals: // property signals
    void hostChanged(QString arg);
    void portChanged(quint16 arg);
    void isConnectedChanged(bool arg);
    void connected();
    void disconnected();
    void autoReconnectChanged(bool arg);
    void databaseOnlineChanged(bool arg);

    void authenticatedChanged(bool arg);

    void userInfoChanged(QVariantMap arg);

    void canReadChanged(bool arg);

    void canEditChanged(bool arg);

    void canManageUsersChanged(bool arg);

public slots: // property setters
    void setHost(QString arg)
    {
        if (m_host == arg)
            return;

        m_host = arg;
        emit hostChanged(arg);
    }
    void setPort(quint16 arg)
    {
        if (m_port == arg)
            return;

        m_port = arg;
        emit portChanged(arg);
    }
    void setAutoReconnect(bool arg)
    {
        if (m_autoReconnect == arg)
            return;

        m_autoReconnect = arg;
        emit autoReconnectChanged(arg);
    }
    void setCanRead(bool arg)
    {
        if (m_canRead == arg)
            return;

        m_canRead = arg;
        emit canReadChanged(arg);
    }
    void setCanEdit(bool arg)
    {
        if (m_canEdit == arg)
            return;

        m_canEdit = arg;
        emit canEditChanged(arg);
    }
    void setCanManageUsers(bool arg)
    {
        if (m_canManageUsers == arg)
            return;

        m_canManageUsers = arg;
        emit canManageUsersChanged(arg);
    }
};

#endif // SERVICE_H
