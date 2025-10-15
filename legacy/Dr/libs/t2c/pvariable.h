#ifndef PVARIABLE_H
#define PVARIABLE_H
#include <QVariant>
#include <QDateTime>
#include <QBitArray>
//#include <QString>

struct PVariable
{
    PVariable()
    {
        status = OK;
        value = QVariant();
        time = QDateTime();
    }
    enum PVStatus
    {
        OK =          0x00,
        NotOK =       0x01,
        Test =        0x02,
        Blocked =     0x04,
        UserEntry =   0x08,
        Substituted = 0x10,
        Queried =     0x20,
        Reserved =    0x40,
        InvalidTime = 0x80
    };

    //———————————————————————————————
    QVariant    value;
    QDateTime   time;
    uint        status;
    QByteArray  vtype;
    QString     fullName;
    //———————————————————————————————
};

Q_DECLARE_METATYPE(PVariable)
#endif // PVARIABLE_H
