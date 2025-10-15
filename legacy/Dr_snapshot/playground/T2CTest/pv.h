#ifndef PV_H
#define PV_H

#include <QDataStream>
#include <QDateTime>
#include <QVariant>

class PVListener;

struct PV
{
    QDateTime   time                    ;
    QVariant    value                   ;
    quint8      status      {0}         ;
    QByteArray  iType                   ;
    bool        subscribed  {false}     ;
    bool        changed     {false}     ;
    PVListener *listener    {nullptr}   ;
};


QDataStream &operator<< (QDataStream& out, const PV & obj);

QDataStream &operator>> (QDataStream& in, PV & obj);


#endif // PV_H
