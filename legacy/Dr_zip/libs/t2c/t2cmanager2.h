#ifndef T2CMANAGER2_H
#define T2CMANAGER2_H

#include <QHostAddress>
#include <QObject>
#include <QWaitCondition>


class T2CWorker2;
class T2CManager2: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost)
    Q_PROPERTY(int port READ port WRITE setPort)
    Q_PROPERTY(QString encoding READ encoding WRITE setEncoding NOTIFY encodingChanged)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

    bool            m_ready{false};


    T2CWorker2     *m_worker{nullptr};
    QTextCodec     *m_codec;


public:
    T2CManager2(QObject *parent = nullptr);

    QString host() const;
    int port() const;
    QString encoding() const;
    bool ready() const;

public slots:
    void setHost(QString host);
    void setPort(int port);
    void setEncoding(QString arg);


    void start();
    void stop();

    void subscribe(int idf);



signals:
    void sendMessage(QString msg);
    void onSubscribe(int idf);
    void gotMessage(QString message);
    void setValue(int idf, QString type, QVariant value );
    void encodingChanged(QString encoding);
    void readyChanged(bool ready);
};

#endif // T2CMANAGER2_H
