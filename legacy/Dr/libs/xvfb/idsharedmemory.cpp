#include "idsharedmemory.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <QDebug>

IdSharedMemory::IdSharedMemory(QObject *parent) : QObject(parent)
{

}

IdSharedMemory::IdSharedMemory(const int shmid, QObject *parent) : QObject(parent)
{
    setKey(shmid);
}

IdSharedMemory::~IdSharedMemory()
{
    setKey(0);
}

bool IdSharedMemory::attach(IdSharedMemory::AccessMode mode)
{
    if (isAttached() || !m_shmid)
        return false;

    m_ptr = shmat(m_shmid, 0, (mode == IdSharedMemory::ReadOnly ? SHM_RDONLY : 0));
    if ((void*) - 1 == m_ptr) {
        m_ptr = 0;
        m_lastErrorNo = NotFound;
        m_lastErrorString = "Attach failed. Shared Memory by id not found";
        return false;
    }

    // grab the size
    shmid_ds shmid_ds;
    if (!shmctl(m_shmid, IPC_STAT, &shmid_ds)) {
        m_size = (int)shmid_ds.shm_segsz;
    } else {
        m_lastErrorNo = InvalidSize;
        m_lastErrorString = "Failed to get Size of Shared Memory";
        return false;
    }

    return true;

}

bool IdSharedMemory::detach()
{
    if (!isAttached())
        return false;

    if (-1 == shmdt(m_ptr)) {
        switch (errno) {
        case EINVAL:
            m_lastErrorString = "Detach. Shared Memory not attached";
            m_lastErrorNo = NotFound;
            break;
        default:
            m_lastErrorString = "Detach. Unknown Error";
            m_lastErrorNo = UnknownError;
        }
        return false;
    }
    m_ptr = 0;
    m_size = 0;

    return true;
}

void *IdSharedMemory::data()
{
    return m_ptr;
}

IdSharedMemory::IdSharedMemoryError IdSharedMemory::error() const
{
    return m_lastErrorNo;
}

QString IdSharedMemory::errorString() const
{
    return m_lastErrorString;
}

bool IdSharedMemory::isAttached() const
{
    return (0 != m_ptr);
}

int IdSharedMemory::key() const
{
    return m_shmid;
}

void IdSharedMemory::setKey(const int key)
{
    if (key == m_shmid)
        return;
    if (isAttached())
        detach();
    m_shmid = key;
}

int IdSharedMemory::size() const
{
    return m_size;
}

