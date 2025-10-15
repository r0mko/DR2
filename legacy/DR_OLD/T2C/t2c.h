#ifndef T2C_H
#define T2C_H
#include <QtGlobal>
#include "T2C_global.h"
#include <QtCore/qglobal.h>
#include <QtGlobal>
#include <QHash>
#include <QDateTime>
#include <QTcpSocket>
#include <QTextCodec>
#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>
#include <QBitArray>
#include "../Core/plugininterface.h"
#include "../Core/core.h"

class PVListener;
/**
 * @brief Структура, хранящая информацию о процессной переменной
 */
struct PV
{
    /**
     * @brief Метка времени
     */
    QDateTime   time;
    /**
     * @brief Значение
     */
    QVariant    value;
    /**
     * @brief Статус
     */
    quint16     status;
    /**
     * @brief Текстовое обозначение типа данных в ControlStar
     */
    QByteArray  iType;
    /**
     * @brief Флаг подписки. Установлен, если значение этой ПП обновляется автоматически.
     */
    bool        subscribed;
    /**
     * @brief Флаг изменения. Установлен, если значение кем-то изменено, и необходима синхронизация со SCADA.
     */
    bool        changed;
    /**
     * @brief Указатель на объект listener
     */
    PVListener* listener;
    PV() :
        status(0),
        subscribed(false),
        changed(false),
        listener(0)
    {}
};

QDataStream & operator<< ( QDataStream& out, const PV & obj);

QDataStream & operator>> ( QDataStream& in, PV & obj);

Q_DECLARE_METATYPE(PV)
Q_DECLARE_METATYPE(QList<PV>)

class TokenList
{
public:
    TokenList ();
    TokenList (const QList<QByteArray>&other);
    QByteArray join (char separator) const;
    QByteArray toQuery();
    const QByteArray& tokenAt (int pos) const { return d.at(pos); }
    int size () const { return d.size(); }
    void clear () { d.clear(); }
    void append (const QByteArray& token) { d.append(token); }
    void append (int idf) { d.append(QByteArray::number(idf)); }
    void append (QList<int> idf) { foreach (int id, idf) { d.append(QByteArray::number(id)); }; }
    void append (qreal real) { d.append(QByteArray::number(real)); }
    void prepend (const QByteArray& token) { d.prepend(token); }
    bool contains (const QByteArray& token) const { return d.contains(token); }
    bool containsAll (const TokenList& tokens);
    bool containsSome (const TokenList& tokens);
    void remove (const QByteArray& token);
    void remove (const TokenList& tokens);
    void removeAt (int pos) {d.removeAt(pos); }
    bool isEmpty() const { return d.isEmpty(); }
    QByteArray first() const { return d.first(); }
    QByteArray last() const { return d.last(); }
    TokenList &operator=(const TokenList & other );
    TokenList &operator=(const QList<QByteArray>& other ) { d = other; return *this; }
    QSet<uint> queryIdfs();
    uint replyIdf();
    static TokenList tokenizeDatLine(const QByteArray &datLine);


protected:
    QList<QByteArray> d;
};


/**
 * @brief PVListener выдаёт сигнал valueUpdated при изменении значения связанной процессной переменной
 */
class PVListener : public QObject
{
    Q_OBJECT
    QAtomicInt refcnt;
public:
    PVListener(int pvcount) : refcnt(pvcount) {}

    virtual ~PVListener() {}
    bool deref() { return refcnt.deref(); }
public slots:
    void onUpdate(int idf) { emit valueUpdated(idf); }
signals:
    void valueUpdated(int idf);
};
Q_DECLARE_METATYPE(PVListener*)

typedef QHash<int, PV> PVCache;

Q_DECLARE_METATYPE(PVCache)



/**
 * @brief T2CManager - модуль для работы с интерфейсом T2C. Позволяет читать, записывать и подписываться на изменения процессных переменных в SCADA ControlStar
 *
 */
class T2CManager : public QObject
{
    Q_OBJECT
    PVCache cache;
    QHash<int, PVListener*> listeners;
    QSet<int> queryIdfs;
    QSet<int> subscribedIdfs;
    QTcpSocket socket;
    struct T2CParams
    {
        uint max_idf;
        uint max_inp;
        uint max_out;
    } params;
    bool t2cready;
    bool verboseMode;
    int sim;
    QTextCodec* codec;
    QSet<int> updatedVariables;
    Q_PROPERTY(uint simLevel READ simLevel WRITE setSimLevel)
    Q_PROPERTY(QString encoding READ encoding WRITE setEncoding)
    Q_PROPERTY(bool immediateUpdate READ immediate WRITE setImmediate)
    /**
     * @brief «Сырой режим»
     */
    Q_PROPERTY(bool rawMode READ rawMode WRITE setRawMode)
public:
    explicit T2CManager();
    /**
     * @brief bindToScriptEngine создаёт биндинг к JavaScript-движку
     * @param engine - указатель на QScriptEngine
     */
    static void bindToScriptEngine (QScriptEngine* engine);
    static QScriptValue JST2CCtor(QScriptContext* context, QScriptEngine* engine);


    /**
     * @brief Возвращает кодировку, используемую для обмена с T2C.
     */
    QString encoding() const { return QString(codec->name()); }
    /**
     * @brief Возвращает true, если включен режим немедленной записи при изменениях.
     */
    bool immediate() const { return p_immediateUpdate; }

    /**
     * @brief Возвращает true, если включен «сырой» режим.
     */
    bool rawMode() const { return m_rawMode; }


signals:
    /**
     * @brief Сигнализирует об изменении ПП указанного IDF
     */
    void PVupdated(int idf);

public slots:
    /**
     * @brief Устанавливает кодировку для обмена с T2C.
     * @param arg — обозначение кодировки, обычно "ISO8859-5".
     * @see encoding()
     */
    void setEncoding(QString arg) { codec = QTextCodec::codecForName(arg.toLatin1()); }

    /**
     * Управляет режимом немедленной записи изменений.
     * @detailed Если флаг установлен в true, то при изменении процессной переменной функциями updateValue() команда на запись в T2C выдаётся немедленно. Если false, то изменения значений процессных переменных записываются в кэш, а фактическая запись происходит только при вызове commit().
     * @see commit()
     * @see updateValue()
     */
    void setImmediate(bool arg) { p_immediateUpdate = arg; }

    /**
     * @brief Управляет «сырым» режимом.
     * @param arg
     */
    void setRawMode(bool arg) { m_rawMode = arg; }
    /**
     * @brief Запрашивает единичную ПП из T2C. Подключение должно быть установлено.
     * @param idf - IDF процессной переменной.
     * @param forceCached - флаг запроса из кэша. По умолчанию установлен: если значение с таким IDF содержится в кэше, оно будет возвращено, иначе будет выполнен запрос. Если флаг установлен в false, будет выполнен запрос значения независимо от наличия элемента в кэше.
     * @return Процессная переменная.
     */
    PV getPV(int idf, bool forceCached = true);

    /**
     * @brief Запрашивает значение единичной ПП из T2C. Подключение должно быть установлено. Удобно использовать, когда не важен статус или метка времени ПП.
     * @param idf - IDF процессной переменной.
     * @param forceCached - флаг запроса из кэша. По умолчанию установлен: если значение с таким IDF содержится в кэше, оно будет возвращено, иначе будет выполнен запрос. Если флаг установлен в false, будет выполнен запрос значения независимо от наличия элемента в кэше.
     * @return Значение ПП.
     */
    QVariant getValue(int idf, bool forceCached = true);

    /**
     * @brief Создаёт объект-уведомитель об изменении значения процессных переменных из списка.
     * @param idfList - список IDF для прослушки.
     * @return Указатель на созданный объект-уведомитель.
     */
    PVListener* listenPVs(const QList<int> &idfList);

    /**
     * @brief Посылка сырого запрос в T2C. Свойство "rawMode" должно быть установлено в true.
     * @param query - текст запроса.
     */
    void sendRawQuery(const QString& query);

    /**
     * @brief Пытается подключиьтся к T2C с указанными параметрами.
     * @param addr - IP-адрес SCADA-системы.
     * @param port - порт (обычно 47001).
     * @param verbose - флаг "молчаливого" режима. Если установлен в true, в stdout не выводятся сообщения о ходе подключения.
     * @return
     */
    bool connectToT2C(QHostAddress addr, quint16 port, bool verbose = false);

    /**
     * @brief Функция для удобства использования с JavaScript. Пытается подключиьтся к T2C с указанными параметрами.
     * @param addr - IP-адрес SCADA-системы, переданный строкой вида "10.0.0.1"
     * @param port - порт (обычно 47001).
     * @param verbose - флаг "молчаливого" режима. Если установлен в true, в stdout не выводятся сообщения о ходе подключения.
     * @return Результат попытки. Если подключение удалось, возвращает true.
     */
    bool connectToT2C(QString addr, quint16 port, bool verbose = false);

    /**
     * Запрашивает список процессных переменных.
     * Функция позволяет получить список значений ПП намного быстрее, чем множественные запросы к getPV().
     * @param idfList - список IDF.
     * @param subscribe - флаг подписки. Если установлен, дальнейшие обновления процессных переменных будут автоматически помещаться в кэш, и следующие запросы к ним будут происходить гораздо быстрее.
     * @return Список процессных переменных.
     */
    QList<PV> getPVs(const QList<int> &idfList, bool subscribe = true);

    /**
     * Запрашивает процессные переменные из T2C по списку IDF.
     * Функция позволяет получить список значений ПП намного быстрее, чем множественные запросы к getPV().
     * @param idfList - список IDF.
     * @param subscribe - флаг подписки. Если установлен, дальнейшие обновления процессных переменных будут автоматически помещаться в кэш, и следующие запросы к ним будут происходить гораздо быстрее.
     * @return Список значений процессных переменных.
     */
    QList<QVariant> getValues(const QList<int> &idfList, bool subscribe = true);

    /**
     * @brief Подписывается на список ПП. Удобна тем, что не дожидается возвращения всех значений. Данные накапливаются в кэше по мере поступления.
     * @param idfList - список IDF.
     */
    void subscribeValues(const QList<int> &idfList);

    /**
     * @brief Отменяет подписку на список ПП.
     * @param idfList - список IDF.
     */
    void unsubscribeValues(const QList<int> &idfList);

    /**
     * @brief Изменяет значение ПП.
     * Записывает новое значение процессной переменной. Если включен режим immediateUpdate (свойство установлено в true), то запись происходит немедленно, в противном случае новое значение переменной заносится в кэш, а обновление значения фактически происходит при вызове commit(). Если процессная переменная ранее не запрашивалась, то есть неизвестен её внутренний тип в SCADA-системе, то происходит одиночный запрос. В случае, если необходимо изменить значение очень большого количества ПП, большое количество одиночных запросов может занять значительное время. Во избежание этого необходимо запросить типы этих переменных посредством вызова getValues() или getPVs(), пренебрегая возвращаемым значениям. Типы процессных переменных запишутся в кэш, и при записи будут запрашиваться из него, что значительно ускорит запись. Использование отложенной записи так же значительно ускоряет выполнение этой функции.
     * @param idf — IDF изменяемой процессной переменной
     * @param newValue — новое значение ПП
     * @param status — строка статуса в нотации ControlStar, например, "ABZN". <b>Внимание:</b> в режиме отложенной записи значение данного параметра игнорируется!
     */
    void updateValue(int idf, QVariant newValue, const QString &status = QString(""));

    /**
     * Читает одну строку ответа на запрос в «сыром» режиме.
     * Если полная строка не получена за указанное время, функция возвращает все данные, доступные в буфере сокета.
     * @param timeout — таймаут на чтение строки.
     */
    QString readRawReply(int timeout);

    /**
     * @brief Возвращает true, если в буфере сокета доступны данные для чтения, в противном случае возвращает false. Используется в «сыром» режиме T2C.
     */
    bool hasRawData();

    /**
     * @brief Возвращает true, если в буфере сокета доступна 1 или более строка текста, в противном случае возвращает false. Используется в «сыром» режиме T2C.
     */
    bool canReadLine() { return socket.canReadLine(); }


    /**
     * @brief Возвращает кэш процессных переменных.
     */


    PVCache getCache() const { return cache; }

    /**
     * @detailed Сохраняет кэш процессных переменных в текущей директории.
     * @param filename — имя файла.
     */
    bool saveCache(QString filename) const;
    bool loadCache(QString filename);

    /**
     * \brief Запись изменений.
     * @detailed Записывает в T2C изменения, сделанные с помощью updateValue() в режиме отложенной записи (при установленном в false свойстве immediateUpdate).
     * @see setImmediate()
     */
    void commit();

private slots:
    void readReply();

private:
    //    Q_DISABLE_COPY(T2CManager)
    void initParams();
    void setSimLevel(uint arg) { sim = arg; }
    uint simLevel() const { return sim; }
    bool validateLine(const QByteArray &query, const QByteArray &replyLine);
    PV parseDatTokens(const TokenList &tokens);
    TokenList generateANMQuery(bool subscribe, bool returnValues);
    TokenList generateDATCommand(uint idf);
    bool p_immediateUpdate;
    bool m_rawMode;
};
Q_DECLARE_METATYPE(T2CManager*)

class T2CFactory : public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID "org.PGE.DataReactor.T2C")
#endif
    Core* core;
public:
    T2CFactory() {}
    virtual ~T2CFactory() {}
    QObject* newInstance() { return new T2CManager(); }
    int init(QObject *core);
    QVariantHash getInfo();
};

#endif // T2C
