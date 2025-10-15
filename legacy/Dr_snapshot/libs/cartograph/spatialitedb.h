#ifndef SPATIALITEDB_H
#define SPATIALITEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QHash>
#include <QQueue>
#include <QStringList>
#include "wkt.h"
#include <QMutex>
#include <QDataStream>
#include <QTransform>
#include <QJSValue>
#include <QVariantMap>
#include <QUrl>

class SpatialQuery;
class SpatialLayer
{
    Q_GADGET
    Q_PROPERTY(int srid READ srid CONSTANT)
    Q_PROPERTY(QString geometryColumn READ geometryColumn CONSTANT)
    Q_PROPERTY(QString layerType READ layerType CONSTANT)
    Q_PROPERTY(int geometryType READ geometryType CONSTANT)
    Q_PROPERTY(int geometryKind READ geometryKind CONSTANT)
    Q_PROPERTY(bool isValid READ isValid CONSTANT)
    Q_PROPERTY(QString tableName READ tableName CONSTANT)
    Q_PROPERTY(int entityCount READ entityCount CONSTANT)
    Q_PROPERTY(QRectF extent READ extent CONSTANT)

    int             m_srid;
    QString         m_geometryColumn;
    QString         m_layerType;
    GeomType        m_geometryType;
    QString         m_tableName;
    int             m_entityCount;
    QRectF          m_extent;

public:
    SpatialLayer() : m_geometryType(Invalid) {}
    SpatialLayer(QVariantMap row);

    int srid() const;
    void setSrid(int);

    QString geometryColumn() const;
    void setGeometryColumn(const QString &);

    QString layerType() const;
    void setLayerType(const QString &);

    GeomType wktGeomType() const;
    void setGeomType(const GeomType &);

    int geometryType() const;
    void setGeometryType(int);

    int geometryKind() const;

    QString tableName() const;
    void setTableName(const QString &);

    int entityCount() const;
    void setEntityCount(int);

    QRectF extent() const;
    void setExtent(const QRectF &);

    bool isValid() const;
};



class DBWorker : public QObject
{
    Q_OBJECT
public:

    DBWorker(QObject *parent = 0);
    virtual ~DBWorker() {}
    enum SpatiaLiteError {
        NoError,
        DatabaseTypeError,
        ConnectionError,
        StatementError,
        TransactionError,
        UnknownError
    };
    Q_ENUM(SpatiaLiteError)
signals:
    void openDatabase(const QString &filename);
//    void fetch(Geometry *geo, QString queryGeometry, QString geometryColumn);
    void openChanged(bool arg);
    void versionInfo( QVariantMap );
    void error(DBWorker::SpatiaLiteError error, QString errorText);
    void queryProgressChanged(qreal);

    void currentJobCountChanged(int count);
    void currentJobColumnsChanged(QStringList columns);
    void currentJobComplete();

public slots:
//    void queryGeometry(Geometry *geo, QString queryGeometry, QString geometryColumn);
    void openDB(QString arg);
    QStringList tables(QSql::TableType type) const;
    QVariantList querySync(QString query);
    void queryAsync(SpatialQuery *job, QString geometryColumn, int reportInterval);
protected:
//    void connectNotify(const QMetaMethod &signal);
//    void disconnectNotify(const QMetaMethod &signal);
private:
    friend class SpatiaLiteDB;
    QSqlDatabase    db;
    mutable QMutex  dbMutex;
    QVariantMap     m_versionInfo;
    void requestVersionInfo();
};

class GeoLayer;
class QQuickItem;
class SpatiaLiteDB : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl database READ database WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(bool isOpen READ isOpen NOTIFY openChanged)
    Q_PROPERTY(bool ready READ ready WRITE setReady NOTIFY readyChanged)
    Q_PROPERTY(QVariantMap versionInfo READ versionInfo CONSTANT)
    Q_PROPERTY(DBWorker::SpatiaLiteError error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorText READ errorText NOTIFY errorTextChanged)

public:
    explicit SpatiaLiteDB(QObject *parent = 0);
    QUrl database() const;
    bool isOpen() const;
    QString errorText() const;
    DBWorker::SpatiaLiteError error() const;

    void queryAsync(SpatialQuery *data, QString geometryColumn = QString(), int reportInterval = 1000) const;

    QVariantMap versionInfo() const;
    Q_INVOKABLE QRectF extent(const QString &layerName, uint srid = 0);
    Q_INVOKABLE QRectF extent(uint srid = 0);
    Q_INVOKABLE QVariantList query(QString query) const;
    Q_INVOKABLE SpatialQuery *postQuery(QString query);
    Q_INVOKABLE QVariantList geometryLayers() const;
    Q_INVOKABLE QStringList allTables() const;
    Q_INVOKABLE SpatialLayer layerInfo(const QString &name) const;
    Q_INVOKABLE bool hasLayer(const QString &name);

    void setDatabase(QUrl arg);
    bool ready() const;

signals:
    void databaseChanged(QString arg);
    void openChanged(bool arg);
    void readyChanged(bool ready);
    void errorChanged(DBWorker::SpatiaLiteError error);
    void errorTextChanged(QString errorText);

private slots:
    void setVersionInfo( QVariantMap arg );
    void setError(DBWorker::SpatiaLiteError error, QString errorText)
    {
        if (m_error == error && m_errorText == errorText)
            return;
        m_error = error;
        m_errorText = errorText;
        emit errorChanged(error);
        emit errorTextChanged(errorText);
    }

private:
    void readLayersInfo();
    void setReady(bool ready);


    DBWorker*                               worker = nullptr;
    mutable QHash<QString, SpatialLayer>    m_layers;
    QVariantMap                             m_versionInfo;
    bool                                    m_ready = false;
    DBWorker::SpatiaLiteError               m_error;
    QString                                 m_errorText;
};

Q_DECLARE_METATYPE(SpatialLayer)

#endif // SPATIALITEDB_H
