#include "resourcemanager.h"
#include "core.h"
//#include <QMetaType>

ResourceManager::ResourceManager(Core *c)
{
    p.core = c;
    p.banks.resize(2048);
}

RUID ResourceManager::allocateResource(int typeId, const void *copy)
{
    RUID id;
    QVector<void*>& bank = p.banks[typeId];
    int index = bank.size();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    void* dp = QMetaType::create(typeId, copy);
#else
    void* dp = QMetaType::construct(typeId, copy);
#endif
    bank.append(dp);
    id.coreId = p.core->coreId();
    id.index = index;
    id.typeId = typeId;
    return id;
}

void *ResourceManager::getResource(const RUID &id)
{
    return p.banks[id.typeId][id.index];
}
