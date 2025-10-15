#include "settings.h"
#include <QFile>
#include <QJsonObject>
#include <QDebug>
#include <QDir>
bool Settings::initialize()
{
    QDir::setCurrent(INSTALLPATH);
    QFile file( "settings.json" );
    file.open( QIODevice::ReadOnly | QIODevice::Text );
    QJsonParseError p;
    m_settings = QJsonDocument::fromJson( file.readAll(), &p);
    file.close();
    return !m_settings.isEmpty() && m_settings.isObject();
}

void Settings::reload()
{
    QDir::setCurrent(INSTALLPATH);
    QFile file( "settings.json" );
    file.open( QIODevice::ReadOnly | QIODevice::Text );
    QJsonParseError p;
    QJsonDocument s = QJsonDocument::fromJson( file.readAll(), &p);
    file.close();
    if( !m_settings.isEmpty() && m_settings.isObject() ) {
        m_settings = s;
        qDebug() << "[ Settings ] reloaded";
    }else{
        qDebug() << "[ Settings ] reload failed";
    }
}

QVariantMap Settings::data() const
{
    return m_settings.object().toVariantMap();
}

