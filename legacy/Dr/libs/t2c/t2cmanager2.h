#ifndef T2CMANAGER2_H
#define T2CMANAGER2_H

#include <QHostAddress>
#include <QObject>


class T2CWorker2;
class T2CManager2: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost)
    Q_PROPERTY(int port READ port WRITE setPort)

    T2CWorker2 *m_worker{nullptr};


public:
    T2CManager2(QObject *parent = nullptr);
    QString host() const;
    int port() const;

public slots:
    void setHost(QString host);
    void setPort(int port);

    void start();
    void stop();
    void subscribe(int idf);
signals:
    void onSubscribe(int idf);
    void gotMessage(QString message);
};

#endif // T2CMANAGER2_H
