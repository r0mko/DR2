#include "logger.h"
#include <QTime>
#include <QDebug>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>

Logger::Logger(QObject *parent)
    : QObject(parent)
{
    m_archivePath = QDir::homePath() + "/drmessages.sqlite";
    perfTimer = startTimer(1000);

    perfData.recordsAdded = 0;
    perfData.recordsFetched = 0;
    connect(this, &Logger::log, this, &Logger::insert);
}

Logger::~Logger()
{
    if (db.isOpen())
        db.close();
}

QDateTime Logger::lastTimestamp() const
{
    return m_lastTimestamp;
}

QDateTime Logger::firstTimestamp() const
{
    return m_firstTimestamp;
}

int Logger::recordCount() const
{
    return m_recordCount;
}


void Logger::setArchivePath(QString arg)
{
    if (m_archivePath == arg)
        return;

    m_archivePath = arg;
    emit archivePathChanged(arg);
}

int Logger::beginSession(int precache)
{
    int ret = -0xffff;
    if (!m_sessions.isEmpty()) {
        int c = 0;
        while (c < m_sessions.size()) {
            if (m_sessions.at(c).active == false) {
                ret = c;
                break;
            }
            ++c;
        }
    }
    if (ret == -0xffff) {
        ret = m_sessions.size();
        LoggerSession s;
        m_sessions.append(s);
    }
    LoggerSession *ses = m_sessions.data() + ret;
    ses->cache = queryLog(QString("ORDER BY TIMESTAMP DESC LIMIT %1").arg(precache));
    ses->active = true;
    qDebug() << "Started session" << ret;
    return ret;
}

void Logger::endSession(int sessionId)
{
    Q_ASSERT(sessionId < m_sessions.size());
    m_sessions[sessionId].active = false;
    m_sessions[sessionId].cache.clear();
}

QVariantList Logger::fetch(int sessionId, int first, int last) const
{
    Q_ASSERT(sessionId < m_sessions.size());
    QVariantList ret;
    const LoggerSession &ses = m_sessions.at(sessionId);
    qDebug() << "fetch records for session" << sessionId << "from" << first << "to" << last << "from overall" << ses.cache.size();
    if (!ses.active) {
        printlog(Logger) << "invalid session id" << sessionId;
        return ret;
    }
    if (last < 0)
        last = 0;
    if (first < 0)
        first = 0;
    if (first > ses.cache.size())
        first = ses.cache.size() - 1;
    if (last > ses.cache.size())
        last = ses.cache.size() - 1;



    for (int i = first; i < last; ++i) {
        ret.append(ses.cache.at(i).packToVariant());
    }
    //    qDebug() << "fetched" << ret.size() << "records for session" << sessionId << "cache size" << ses.cache.size();
    return ret;
}

bool Logger::addRecord(const QDateTime &timestamp, const QString &nodeName, const QString &objectName, const QString &message, int type, const QByteArray &attachment)
{
    QSqlQuery q(db);
    q.prepare("INSERT INTO log(TIMESTAMP, NODE, ORIGIN, TYPE, MESSAGE, ATTACHMENT) VALUES(:timestamp, :node, :origin, :type, :message, :attach)");
    q.bindValue(":timestamp", timestamp.toMSecsSinceEpoch());
    q.bindValue(":node", nodeName);
    q.bindValue(":origin", objectName);
    q.bindValue(":type", type);
    q.bindValue(":message", message);
    q.bindValue(":attach", attachment);
    if (!q.exec()) {
        emit error(QString("Error adding record: %1").arg(q.lastError().text()));
        qDebug() << "Error insert" << q.lastError().text();
        return false;
    }
    ++perfData.recordsAdded;
    setLastTimestamp(timestamp);
    setFirstTimestamp(timestamp);
    return true;
}

QVariantList Logger::fetchLast(int count)
{
    QVariantList ret;
    auto l = queryLog(QString("ORDER BY TIMESTAMP DESC LIMIT %1").arg(count));
    for (const LogRecord &r:l) {
        ret.append(r.packToVariant());
        ++perfData.recordsFetched;
    }
    return ret;
}

QVariantList Logger::fetchBefore(QDateTime last, int count)
{
    QVariantList ret;
    auto l = queryLog(QString("WHERE TIMESTAMP < %1 ORDER BY TIMESTAMP DESC LIMIT %2").arg(last.toMSecsSinceEpoch()).arg(count));
    for (const LogRecord &r:l) {
        ret.append(r.packToVariant());
        ++perfData.recordsFetched;
    }
    return ret;
}

void Logger::timerEvent(QTimerEvent *e)
{
    static qint64 prevAdded;
    static qint64 prevFetched;
    static qint64 lastCheck;
    if (e->timerId() != perfTimer) {
        e->ignore();
        return;
    }
    if (perfData.recordsAdded != prevAdded) {
        printlog(Logger) << "performance data:";
        qDebug() << "added" << perfData.recordsAdded - prevAdded << "records" << perfData.recordsAdded << "total";
        prevAdded = perfData.recordsAdded;
    }
    if (perfData.recordsFetched != prevFetched) {
        qDebug() << "fetched" << perfData.recordsFetched - prevFetched << "records" << perfData.recordsFetched << "total";
        prevFetched = perfData.recordsFetched;
    }
    lastCheck = QDateTime::currentMSecsSinceEpoch();
}

void Logger::insert(QDateTime timestamp, QString nodeName, QString objectName, QString message, int type, QVariant attachment)
{
    //    qDebug().nospace() << "inserting " << message << "from" << nodeName << "." << objectName << " " << timestamp;
    QByteArray adata;
    if (!attachment.isNull()) {
        QDataStream out(&adata, QIODevice::WriteOnly);
        attachment.save(out);
    }
    if(addRecord(timestamp, nodeName, objectName, message, type, adata)) {
        LogRecord rec;
        rec.eventTimeStamp = timestamp;
        rec.dbTimeStamp = QDateTime::currentDateTime();
        rec.nodeName = nodeName;
        rec.objectName = objectName;
        rec.message = message;
        rec.type = type;
        rec.attach = attachment;
        emit recordAdded(rec.packToVariant());
        //        emit recordAdded(timestamp, nodeName, objectName, (MessageType)type, message, attachment);
    }

}

bool Logger::init(const QString &path)
{
    qDebug() << "Initializing DB" << path;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    if (!db.isValid()) {
        qDebug() << "error adding database:" << db.lastError().text();
        return false;
    }
    if (!db.open()) {
        emit error(tr("Error opening database"));
        return false;
    }
    if (!db.tables().contains("log")) {
        QSqlQuery q(db);
        if (q.exec("CREATE TABLE `log` (\
                   `ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
                   `TIMESTAMP`	INTEGER NOT NULL,\
                   `DB_TIMESTAMP`	TIMESTAMP DEFAULT CURRENT_TIMESTAMP,\
                   `NODE`	TEXT DEFAULT \"this\",\
                   `ORIGIN`	TEXT DEFAULT 'NULL',\
                   `MESSAGE`	TEXT NOT NULL,\
                   `TYPE`	INTEGER DEFAULT '0',\
                   `ATTACHMENT`	BLOB DEFAULT 'NULL');")
                )
        {
            q.prepare("INSERT INTO log(TIMESTAMP, ORIGIN, MESSAGE) VALUES (:now, :origin, :message)");
            q.bindValue(":now", QDateTime::currentMSecsSinceEpoch());
            q.bindValue(":origin", this->metaObject()->className());
            q.bindValue(":message", "Журнал создан");
            if (!q.exec()) {
                QString err = QString("DB error:(%1)").arg(q.lastError().text());
                emit error(err);
                qDebug() << "Create DB error:" << err;
            }
        } else {
            qDebug() << "Could not create database";
            return false;
        }
    }
    QSqlQuery q(db);
    bool ret = q.exec("SELECT COUNT(ID) FROM log");
    if (!ret) {
        emit error(QString("DB error:(%1)").arg(q.lastError().text()));
        return ret;
    }
    if (q.next()) {
        m_recordCount = q.value(0).toInt();
        emit recordCountChanged(m_recordCount);
    } else {
        emit error(QString("DB error:(%1)").arg(q.lastError().text()));
        return false;
    }

    ret = q.exec("SELECT TIMESTAMP FROM log ORDER BY ID DESC LIMIT 1");
    if (!ret) {
        emit error(QString("DB error:(%1)").arg(q.lastError().text()));
        return ret;
    }
    if (q.next()) {
        m_lastTimestamp = QDateTime::fromMSecsSinceEpoch(q.value(0).toLongLong());
        emit lastTimestampChanged(m_lastTimestamp);
    } else {
        emit error(QString("DB error:(%1)").arg(q.lastError().text()));
        return false;
    }

    ret = q.exec("SELECT TIMESTAMP FROM log ORDER BY ID LIMIT 1");
    if (!ret) {
        emit error(QString("DB error:(%1)").arg(q.lastError().text()));
        return ret;
    }
    if (q.next()) {
        m_firstTimestamp = QDateTime::fromMSecsSinceEpoch(q.value(0).toLongLong());
        emit firstTimestampChanged(m_firstTimestamp);
    } else {
        emit error(QString("DB error:(%1)").arg(q.lastError().text()));
        return false;
    }

    return ret;

}

void Logger::setLastTimestamp(const QDateTime &ts)
{
    if (m_lastTimestamp < ts) {
        m_lastTimestamp = ts;
        emit lastTimestampChanged(ts);
    }
}

void Logger::setFirstTimestamp(const QDateTime &ts)
{
    if (m_firstTimestamp > ts) {
        m_firstTimestamp = ts;
        emit firstTimestampChanged(ts);
    }
}

QList<Logger::LogRecord> Logger::queryLog(const QString &clause)
{
    QList<Logger::LogRecord> ret;
    QSqlQuery q(db);
    if (!q.exec(QString("SELECT * FROM log %1").arg(clause))) {
        emit error(QString("DB error:(%1)").arg(q.lastError().text()));
        return ret;
    }
    while (q.next()) {
        Logger::LogRecord rec;
        rec.eventTimeStamp = QDateTime::fromMSecsSinceEpoch(q.value("TIMESTAMP").toULongLong());
        rec.dbTimeStamp = QDateTime::fromMSecsSinceEpoch(q.value("DB_TIMESTAMP").toULongLong());
        rec.nodeName = q.value("NODE").toString();
        rec.objectName = q.value("ORIGIN").toString();
        rec.message = q.value("MESSAGE").toString();
        rec.type = q.value("TYPE").toInt();
        QByteArray adata = q.value("ATTACHMENT").toByteArray();
        if (!adata.isEmpty()) {
            QDataStream in(adata);
            rec.attach.load(in);
        }
        ret.prepend(rec);
    }
    return ret;
}



QString Logger::archivePath() const
{
    return m_archivePath;
}

void Logger::componentComplete()
{
    if (init(m_archivePath))
        qDebug() << "DB initialized successfully";
    else
        qDebug() << "DB init error";
}


QVariantMap Logger::LogRecord::packToVariant() const
{
    QVariantMap rec;
    rec["eventDateTime"] = eventTimeStamp;
    rec["dbDateTime"] = dbTimeStamp;
    rec["nodeName"] = nodeName;
    rec["originName"] = objectName;
    rec["message"] = message;
    rec["attachment"] = attach;
    rec["type"] = type;
    return rec;
}
