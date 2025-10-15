#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QAbstractSocket>
#include <QJSEngine>
#include <QJSValue>

class QTcpSocket;
class QSqlDatabase;
class Settings;
class Server;
class Connection : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap msg READ msg WRITE setMsg NOTIFY msgChanged)

    static const quint32 magic = 0xfcb0ad53;

private:
    qintptr m_handle;
    QSqlDatabase* m_database;
    Settings* m_settings;
    Server* m_server;

    QTcpSocket* m_socket;
    quint32 m_blockSize;

    quint32 m_rowCount;

    bool m_authorized;
    int m_userId;
    QString m_userName;
    QString m_roleName;
//    QString m_login;
    bool m_canRead;
    bool m_canWrite;
    bool m_canManageUsers;

    QJSEngine m_engine;
    QJSValue m_this;

public:
    explicit Connection( quintptr handle, QSqlDatabase* database, Settings* settings, Server* server );
    ~Connection();

    Q_INVOKABLE QVariant exec( QString query, QVariantMap bindings );
    Q_INVOKABLE QVariant dbOperation( QString query, QVariantMap bindings );
    Q_INVOKABLE QVariant dbValue( QString query, QVariantMap bindings );


    QVariantMap msg() const
    {
        return m_msg;
    }

signals:
    void finished();
    void loginRetry( const QVariantMap &msg );



    void msgChanged(QVariantMap arg);

private:
    void processDataRequestMessage( const QVariantMap &msg );
    void processLoginMessage( const QVariantMap &msg );
    void processHelloMessage( const QVariantMap &msg );
    void processLogoutMessage( const QVariantMap &msg );
    void processServerStatusMessage( const QVariantMap &msg );
    void processStopServerMessage( const QVariantMap &msg );
    void processDataAddMessage( const QVariantMap &msg );
    void processDataDelMessage( const QVariantMap &msg );
    void processDataEditMessage( const QVariantMap &msg );
    void processTestfMessage( const QVariantMap &msg );

    void processServerReloadSettingsMessage( const QVariantMap &msg );









    QString sqlSelect(const QVariantMap& msg, bool count = false);

    void sendReadyMessage();
    bool checkDatabase();



    QVariantMap m_msg;

private slots:
    void processMessage( const QVariantMap &msg );

protected slots:
    void onConnected();
    void onDisconnected();
    void onError( QAbstractSocket::SocketError error );
    void onStateChanged( QAbstractSocket::SocketState state );
    void onReadyRead();

public slots:
    void start();
    void sendMessage( QVariantMap msg );
    void onDatabaseOpenedChanged( bool arg );


    void setMsg(QVariantMap arg)
    {
        if (m_msg == arg)
            return;

        m_msg = arg;
        emit msgChanged(arg);
    }
};



#endif // CONNECTION_H

