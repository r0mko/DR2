#ifndef SETTINGS_H
#define SETTINGS_H


#include <QJsonDocument>
#include <QVariantMap>

class Settings
{
    QJsonDocument m_settings;

public:
    bool initialize();
    void reload();
    QVariantMap data() const;
};

#endif // SETTINGS_H
