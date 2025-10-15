#ifndef USERDATA_H
#define USERDATA_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QVariantMap>
class Userdata : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString file READ file WRITE setFile NOTIFY fileChanged)
    QFile m_dufile;
    QVariantMap m_allData;

public:
    explicit Userdata(QObject *parent = 0);
    ~Userdata();

public slots:
    void store( const QString& username, QVariantMap data );

private:
    QString m_file;

public:
    QString file() const
    {
        return m_file;
    }

signals:
    void fileChanged(QString arg);

public slots:
    void setFile(QString arg)
    {
        if( m_file.isEmpty() )
            return;
        if (m_file == arg )
            return;
        // TODO: check wether old file exists?
        // TODO: copy old file to new location
        // TODO: check wether new file exists? ask for overwrite
        m_file = arg;
        emit fileChanged(arg);
    }
};

#endif // USERDATA_H
