#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include "resourcedefs.h"
#include <QObject>
#include <QVector>
//#include "core.h"
class Core;

class ResourceManager : public QObject
{
    Q_OBJECT
    struct ResourceManagerPrivate
    {
        QVarLengthArray<QVector<void*>, 2048> banks;
        Core* core;
    } p;

public:
    ResourceManager(Core* c);
    RUID allocateResource(int typeId, const void *copy = 0);
    void* getResource(const RUID& id);
signals:
    
public slots:
    
};

#endif // RESOURCEMANAGER_H
