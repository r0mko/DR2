#ifndef SPATIALQUERY_H
#define SPATIALQUERY_H

#include <QObject>
#include <QQmlParserStatus>
#include <QMutex>
#include <qabstractitemmodel.h>
#include "wkt.h"

class SpatiaLiteDB;
class DBWorker;
class SpatialQuery;

class ReplyModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    friend class SpatialQuery;
public:
    enum GeometryRoles {
        PathRole = Qt::UserRole + 1,
        PolygonRole
    };

    ReplyModel(SpatialQuery *parent = 0);
    int count() const;

signals:
    void countChanged(int count);

private slots:
    void onStatusChanged();
private:
    SpatialQuery *d() const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    int m_count;
};
Q_DECLARE_METATYPE(QPainterPath)



class SpatialQuery : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(QueryStatus)
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(SpatiaLiteDB* database READ database WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool complete READ isComplete NOTIFY completeChanged)
    Q_PROPERTY(QueryStatus status READ status NOTIFY statusChanged)

    Q_PROPERTY(QStringList attributes READ attributes NOTIFY attributesChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(ReplyModel* reply READ reply NOTIFY replyChanged)

    friend class GeomBuffer;

    friend class DBWorker;
    Q_INTERFACES(QQmlParserStatus)
public:
    SpatialQuery(QObject *parent = 0);
    virtual ~SpatialQuery() {}

    enum QueryStatus {
        Ready       = 0x0,
        Queued      = 0x1,
        Fetching    = 0x2,
        Complete    = 0x4,
        Error       = 0x8
    };
    Q_ENUM(QueryStatus)

    inline int count() const { return m_rows.size(); }
    inline QStringList attributes() const { return m_attributes; }
    inline const QList<SpatialEntity> &rows() const { return m_rows; }

    inline QString query() const { return m_query; }
    void setQuery(QString query);

    Q_INVOKABLE QVariantMap row(int index) const;
    inline QString error() const { return m_error; }
    SpatiaLiteDB* database() const { return m_database; }
    bool active() const { return m_active; }
    QueryStatus status() const { return m_status; }
    ReplyModel* reply() const;
    // QQmlParserStatus interface
    void classBegin();
    void componentComplete();

    bool isComplete() const;

public slots:
    void setDatabase(SpatiaLiteDB* database);
    void clear();
    void fetch();
    void fetchGeometry(QString geomColumn);
    void cancel();
    void setActive(bool active);

signals:
    void countChanged(int count);
    void attributesChanged();
    void complete();
    void errorChanged();
    void queryChanged(QString query);
    void databaseChanged(SpatiaLiteDB* database);
    void activeChanged(bool active);
    void completeChanged();
    void statusChanged();
    void replyChanged();

private slots:
    void setColumns(QStringList columns);
    void appendRow(SpatialEntity row);

    // threaded
    void reportCount();
    void setComplete();

private:
    QMutex                  mtx;
    QString                 m_query;
    uint                    executedQueryHash;
    QList<SpatialEntity>    m_rows;
    QStringList             m_attributes;
    QString                 m_error;
    SpatiaLiteDB*           m_database = nullptr;
    bool                    m_active;
    QueryStatus             m_status = Error;
    ReplyModel*             m_reply = nullptr;



    inline bool busy() const { return m_status & (Fetching | Queued); }
    inline bool canQuery() const { return m_database && !(m_status & (Queued | Fetching | Error)); }
    void setStatus(QueryStatus status);
    void setError(const QString &message);


};

#endif // SPATIALQUERY_H
