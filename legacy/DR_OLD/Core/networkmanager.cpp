#include "networkmanager.h"

NetworkManager::NetworkManager(Core *core) :
    QObject(core), core(core)
{
}
