#ifndef PVLISTENER_H
#define PVLISTENER_H

#include <QObject>

class PVListener : public QObject
{
    Q_OBJECT
    QAtomicInt refcnt;

public:
    PVListener(int pvcount) : refcnt(pvcount) {}

    virtual ~PVListener() {}
    bool deref() { return refcnt.deref(); }

public slots:
    void onUpdate(int idf) { emit valueUpdated(idf); }

signals:
    void valueUpdated(int idf);

};

#endif // PVLISTENER_H
