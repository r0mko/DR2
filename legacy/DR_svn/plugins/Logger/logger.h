#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMap>
#include <QHash>
#include <QDateTime>
#include <QVariant>
#include <QMutex>
#include <QTimer>
#include <QtQml>
#include <QSqlDatabase>
#include <QCache>
#define printlog(x) qDebug() << #x << QDateTime::currentDateTime().toString("d-MM-yyyy H:mm:ss:zzz").toLocal8Bit().data()

class Logger : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_ENUMS(MessageType)
    Q_PROPERTY(QDateTime lastTimestamp READ lastTimestamp NOTIFY lastTimestampChanged)
    Q_PROPERTY(QDateTime firstTimestamp READ firstTimestamp NOTIFY firstTimestampChanged)
    Q_PROPERTY(int recordCount READ recordCount NOTIFY recordCountChanged)
    Q_PROPERTY(QString archivePath READ archivePath WRITE setArchivePath NOTIFY archivePathChanged)

public:
    enum MessageType {
        Info = 0,
        Debug = 1,
        Warning = 2,
        Error = 3,
        Critical = 4,
        Fatal = 5,
        Internal = 6,
        User = 100
    };

    Logger(QObject *parent = 0);
    virtual ~Logger();
    QDateTime lastTimestamp() const;
    QDateTime firstTimestamp() const;
    int recordCount() const;

    QString archivePath() const;
    void componentComplete();
    void classBegin() {}

    bool addRecord(const QDateTime &timestamp, const QString &nodeName, const QString &objectName, const QString &message, int type, const QByteArray &attachment = QByteArray());

    void setArchivePath(QString arg);

    Q_INVOKABLE int beginSession(int precache);
    Q_INVOKABLE void endSession(int sessionId);
    Q_INVOKABLE QVariantList fetch(int sessionId, int first, int last) const;

    Q_INVOKABLE QVariantList fetchLast(int count);
    Q_INVOKABLE QVariantList fetchBefore(QDateTime last, int count);
protected:
    void timerEvent(QTimerEvent *e);

public slots:
    void insert(QDateTime timestamp, QString origin, QString name, QString message, int type, QVariant attachment = QVariant());

signals:
    void lastTimestampChanged(QDateTime arg);
    void firstTimestampChanged(QDateTime arg);
    void recordCountChanged(int arg);
    void archivePathChanged(QString arg);
    void error(QString message);
//    void recordAdded(QDateTime timestamp, QString nodeName, QString objectName, MessageType type, QString message, QVariant attachment);
    void recordAdded(QVariantMap record);
    void log(QDateTime timestamp, QString origin, QString name, QString message, int type, QVariant attachment = QVariant());

private:
    struct LogRecord {
        QDateTime eventTimeStamp;
        QDateTime dbTimeStamp;
        QString nodeName;
        QString objectName;
        int type;
        int priority;
        QString message;
        QVariant attach;
        QVariantMap packToVariant() const;
    };

    struct LoggerSession
    {
        QList<LogRecord> cache;
        bool active;
    };

    struct {
        qint64 recordsAdded;
        qint64 recordsFetched;
    } perfData;

    bool init(const QString &path);
    void setLastTimestamp(const QDateTime &ts);
    void setFirstTimestamp(const QDateTime &ts);

    QList<LogRecord> queryLog(const QString &clause);


    QSqlDatabase db;
    QDateTime m_lastTimestamp;

    QVector<LoggerSession> m_sessions;

    QDateTime m_firstTimestamp;
    int m_recordCount;
    QString m_archivePath;
    QMutex m_mutex;
    int perfTimer;
};


#endif // LOGGER_H

