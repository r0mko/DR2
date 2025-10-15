#ifndef IDSHAREDMEMORY_H
#define IDSHAREDMEMORY_H

#include <QObject>

class IdSharedMemory : public QObject
{
    Q_OBJECT
public:
    enum AccessMode {
        ReadOnly,
        ReadWrite
    };
    enum IdSharedMemoryError {
        NoError,
        PermissionDenied,
        InvalidSize,
        KeyError,
        AlreadyExists,
        NotFound,
        LockError,
        OutOfResources,
        UnknownError
    };

    explicit IdSharedMemory(QObject *parent = 0);
    IdSharedMemory(const int shmid, QObject *parent = 0);
    ~IdSharedMemory();
    bool attach(AccessMode mode = ReadWrite);
    bool detach();
    void *data();
    IdSharedMemoryError error() const;
    QString errorString() const;
    bool isAttached() const;
    int key() const;
    void setKey(const int key);
    int size() const;

signals:

public slots:
private:
    int                 m_shmid = 0;
    int                 m_size = 0;
    void               *m_ptr = 0;
    IdSharedMemoryError m_lastErrorNo = NoError;
    QString             m_lastErrorString;
};

#endif // IDSHAREDMEMORY_H
