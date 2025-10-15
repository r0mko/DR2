#ifndef T2CMANAGER_H
#define T2CMANAGER_H
#include <QObject>
#include <QHostAddress>
//#include "t2cworker.h"
//#include <QDebug>
#include <QMutex>
#include <QWaitCondition>
#include "pvariable.h"
//#include "common.h"

//using namespace std;
class T2CWorker;
class T2CManager : public QObject
{
    Q_OBJECT
    T2CWorker* worker;
    QMutex replyMtx;
    QMutex startMtx;
    QWaitCondition replyWaiter;
    QWaitCondition readyWaiter;
    QHash<quint64, PVariable> PVCache;
    bool T2Cready;
    QTextCodec* codec;
    Q_PROPERTY(QString encoding READ encoding WRITE setEncoding)

public:
    explicit T2CManager();
    ~T2CManager();

    bool connectToT2C(QHostAddress addr, quint16 port, bool verbose = false);
    void disconnectFromT2C();
    void setTextEncoding(const char* name);


    // синхронный (блокирующий) запрос переменной. Если subscribe = true, то результат заносится в хэш, а обновления сопровождаются сигналом valueUpdated(quint64 idf);
    PVariable probePVariable(quint64 idf, bool subscribe = false);

    // асинхронный (неблокирующий) запрос списка переменных. Значения возв
    void subcribePVariables(QList<quint64> idf);

    // Асинхронная команда изменения переменной. Если переменная не была ранее инициализирована.
    void updatePVariable(quint64 idf, const QVariant& newValue);
    // Блокирующая команда изменения переменной. Переменная должна быть инициализирована в хэше (подпиской или листингом).
    void updatePVariable_blocking(quint64 idf, const QVariant& newValue);
    // создаёт срез всех ПП
    void listPVs();

    // полное имя ПП
    QString resolveName(quint64 idf);

    QString encoding() const;

signals:
    void t2cReady();
    void valueUpdated(quint64 idf);

public slots:
    void onValueChange();
    void setEncoding(QString arg);


private:
    PVariable parseDATReply(const QList<QByteArray> &replyTokens, quint64 &idf);
    bool checkDATReply(const QList<QByteArray> &tokens);
    QString makeUpdateCmd(quint64 idf, QVariant newValue, bool setStatus = false, bool setTime = false);
    QString m_encoding;
};

#endif // T2CMANAGER_H
