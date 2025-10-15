#include "userdata.h"

Userdata::Userdata(QObject *parent) : QObject(parent)
{

}

Userdata::~Userdata()
{

}

void Userdata::store(const QString &username, QVariantMap data)
{
    Q_UNUSED(username)
    Q_UNUSED(data)
}

