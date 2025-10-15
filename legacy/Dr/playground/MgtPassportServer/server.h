#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QSqlDatabase>
#include <QHash>
#include "settings.h"

class Server : public QTcpServer
{
    Q_OBJECT

    Q_PROPERTY(bool databaseOpened READ databaseOpened NOTIFY databaseOpenedChanged)

private:
    QSqlDatabase m_database;
    Settings m_settings;
    quint16 m_reopenDatabaseDelay;

public:
    explicit Server();
    ~Server();

    bool initialize();

protected:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

public slots:
    void openDatabase();
    void closeDatabase();

signals: // onChanged signals
    void databaseOpenedChanged(bool arg);

public: // getters
    bool databaseOpened() const;

};

#endif // SERVER_H
