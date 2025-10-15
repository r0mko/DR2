#include "pv.h"


QDataStream &operator<<(QDataStream &out, const PV &obj)
{
    out << obj.time;
    out << obj.value;
    out << obj.status;
    out << obj.iType;
    return out;
}


QDataStream &operator>>(QDataStream &in, PV &obj)
{
    in >> obj.time;
    in >> obj.value;
    in >> obj.status;
    in >> obj.iType;
    return in;
}
