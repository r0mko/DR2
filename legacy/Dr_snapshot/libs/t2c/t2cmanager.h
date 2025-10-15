#ifndef T2CMANAGER_H
#define T2CMANAGER_H

#include <QObject>
#include <QQmlParserStatus>
#include <QSet>
#include "processvariable.h"
class T2CWorker;
class T2CManager: public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString encoding READ encoding WRITE setEncoding NOTIFY encodingChanged)
    Q_PROPERTY(int symLevel READ symLevel WRITE setSymLevel NOTIFY symLevelChanged)

    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)


    // members
    T2CWorker                      *m_workerAsync{nullptr};
    T2CWorker                      *m_workerSync{nullptr};
    QTextCodec                     *m_codec{nullptr};
    QSet<int>                       m_subscribeQueue;
    QHash<int, ProcessVariable>     m_subscribedHash;
    QHash<int, ProcessVariable>     m_cache;

    //member funcions
    void start();
    void stop();

    // property members
    QString     m_host;
    int         m_port{0};
    int         m_symLevel{0};

private slots:
    void onAsyncWorkerReady(bool ready);



public:
    T2CManager(QObject *parent = nullptr);


    // QQmlParserStatus interface
    void classBegin();
    void componentComplete();

    // property getters
    QString host() const;
    int port() const;
    QString encoding() const;
    int symLevel() const;
    bool connected() const;

public slots:

    void subscribe(int idf);
    void unsubscribe(int idf);
    void onGotProcessVariableMessage(ProcessVariable pv);
    QString getName(int idf);
//    QString getName(int idf);
//    QString sendRawCommand(const QString &command);

    // property setters
    void setHost(QString host);
    void setPort(int port);
    void setEncoding(QString encoding);
    void setSymLevel(int symLevel);

signals:
    void subscribeSignal(int idf);
    void unsubscribeSignal(int idf);


    // property signals
    void hostChanged(QString host);
    void portChanged(int port);
    void encodingChanged(QString encoding);
    void connectedChanged(bool ready);



    void symLevelChanged(int symLevel);
};

#endif // T2CMANAGER_H
