#ifndef T2CMANAGER_H
#define T2CMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "pv.h"
#include "tokenlist.h"

class PVListener;
class QTextCodec;
class T2CManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(uint simLevel READ simLevel WRITE setSimLevel)
    Q_PROPERTY(QString encoding READ encoding WRITE setEncoding)
    Q_PROPERTY(bool immediateUpdate READ immediate WRITE setImmediate)
    Q_PROPERTY(bool rawMode READ rawMode WRITE setRawMode)

    int  m_sim{0};
    bool m_immediateUpdate{true};
    bool m_rawMode{false};

    QHash<int, PV> m_cache;
    QHash<int, PVListener*> m_listeners;
    QSet<int> m_queryIdfs;
    QSet<int> m_subscribedIdfs;
    QTcpSocket m_socket;

    struct T2CParams
    {
        uint max_idf;
        uint max_inp;
        uint max_out;
    } m_params;

    bool m_t2cready;
    bool m_verboseMode;
    QTextCodec* m_codec;
    QSet<int> m_updatedVariables;

public:
    explicit T2CManager();

//    static void bindToScriptEngine (QScriptEngine* engine);
//    static QScriptValue JST2CCtor(QScriptContext* context, QScriptEngine* engine);

    Q_INVOKABLE bool connectToT2C(QHostAddress addr, quint16 port, bool verbose = false);
    Q_INVOKABLE bool connectToT2C(QString addr, quint16 port, bool verbose = false);
    Q_INVOKABLE PV getPV(int idf, bool forceCached = true);
    Q_INVOKABLE QVariant getValue(int idf, bool forceCached = true);
    Q_INVOKABLE QList<PV> getPVs(const QList<int> &idfList, bool subscribe = true);
    Q_INVOKABLE QList<QVariant> getValues(const QList<int> &idfList, bool subscribe = true);
    Q_INVOKABLE void subscribeValues(const QList<int> &idfList);
    Q_INVOKABLE void unsubscribeValues(const QList<int> &idfList);
    Q_INVOKABLE void updateValue(int idf, QVariant newValue, const QString &status = QString(""));
    Q_INVOKABLE PVListener* listenPVs(const QList<int> &idfList);
    Q_INVOKABLE void commit();
    QString encoding() const;
    bool immediate() const { return m_immediateUpdate; }
    Q_INVOKABLE void sendRawQuery(const QString& query);
    Q_INVOKABLE QString readRawReply(int timeout);
    Q_INVOKABLE bool hasRawData();
    Q_INVOKABLE bool canReadLine() { return m_socket.canReadLine(); }
    Q_INVOKABLE QHash<int, PV> getCache() const { return m_cache; }
    Q_INVOKABLE bool saveCache(QString filename) const;
    Q_INVOKABLE bool loadCache(QString filename);
    bool rawMode() const { return m_rawMode; }

signals:
    void PVupdated(int idf);

public slots:
    void setEncoding(QString arg);
    void readReply();
    void setImmediate(bool arg) { m_immediateUpdate = arg; }
    void setRawMode(bool arg) { m_rawMode = arg; }

private:
    void initParams();
    void setSimLevel(uint arg) { m_sim = arg; }
    uint simLevel() const { return m_sim; }
    bool validateLine(const QByteArray &query, const QByteArray &replyLine);
    PV parseDatTokens(const TokenList &tokens);
    TokenList generateANMQuery(bool subscribe, bool returnValues);
    TokenList generateDATCommand(uint idf);

};

#endif // T2CMANAGER_H
