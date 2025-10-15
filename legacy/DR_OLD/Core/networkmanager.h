#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include "core.h"
#include <QTcpServer>
#include <QTcpSocket>

class NetworkManager : public QObject
{
    Q_OBJECT
    Core*   core;
    QTcpServer  server;

public:
    explicit NetworkManager(Core* core);
    
signals:
    
public slots:
    
};

#endif // NETWORKMANAGER_H
