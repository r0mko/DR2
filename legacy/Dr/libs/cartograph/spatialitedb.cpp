#include "spatialitedb.h"
#include "spatialquery.h"
#include "geolayer.h"
#include "wkt.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QPointF>
#include <QtMath>
#include <QSqlRecord>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJSValue>
#include <QQmlEngine>

SpatiaLiteDB::SpatiaLiteDB(QObject *parent) :
    QObject(parent)
{
    worker = new DBWorker();
    auto workerThread = new QThread();
    worker->moveToThread(workerThread);
    workerThread->start();
    connect(this, &SpatiaLiteDB::destroyed, workerThread, &QThread::quit);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(worker, &DBWorker::openChanged, this, &SpatiaLiteDB::openChanged);
    connect(worker, &DBWorker::openChanged, this, &SpatiaLiteDB::readLayersInfo);
    connect(worker, SIGNAL(versionInfo(QVariantMap)), this, SLOT(setVersionInfo(QVariantMap)));
    connect(worker, &DBWorker::error, this, &SpatiaLiteDB::setError, Qt::QueuedConnection);
}


bool SpatiaLiteDB::hasLayer(const QString &name)
{
    return m_layers.contains(name);
}

bool SpatiaLiteDB::ready() const
{
    return m_ready;
}


QUrl SpatiaLiteDB::database() const
{
    return QUrl::fromLocalFile(worker->db.databaseName());
}

bool SpatiaLiteDB::isOpen() const
{
    return worker->db.isOpen();
}

QString SpatiaLiteDB::errorText() const
{
    return m_errorText;
}

DBWorker::SpatiaLiteError SpatiaLiteDB::error() const
{
    return m_error;
}

void SpatiaLiteDB::queryAsync(SpatialQuery *data, QString geometryColumn, int reportInterval) const
{
    QMetaObject::invokeMethod(worker, "queryAsync", Qt::QueuedConnection, Q_ARG(SpatialQuery *, data), Q_ARG(QString, geometryColumn), Q_ARG(int, reportInterval));
}

void SpatiaLiteDB::setDatabase(QUrl arg)
{
    if (arg.isEmpty())
        return;
    if(worker->db.databaseName() == arg.toLocalFile())
        return;
    worker->openDatabase(arg.toLocalFile());
}

SpatialLayer SpatiaLiteDB::layerInfo(const QString &name) const
{
    return m_layers.value(name);
}

void SpatiaLiteDB::setVersionInfo(QVariantMap arg)
{
    m_versionInfo = arg;
}

DBWorker::DBWorker(QObject *parent) : QObject(parent)
{
    //    connect (this, &DBWorker::fetch, this, &DBWorker::queryGeometry, Qt::QueuedConnection);
    connect (this, &DBWorker::openDatabase, this, &DBWorker::openDB, Qt::QueuedConnection);
}

void DBWorker::openDB(QString arg)
{
    if (db.databaseName() == arg)
        return;
    if (db.isOpen())
        db.close();
    db = QSqlDatabase::addDatabase("SPATIALITE", arg);
    db.setDatabaseName(arg);
    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return;
    }
    if (!db.tables(QSql::Views).contains("vector_layers")) {
        emit openChanged(false);
        db.close();
        return;
    }
    requestVersionInfo();
    emit openChanged(true);
}

QVariantList DBWorker::querySync(QString query)
{
    QVariantList ret;
    QSqlQuery q(db);
    q.exec(query);

    QSqlRecord rec = q.record();
    int c = rec.count();
    QStringList fields;
    for (int i = 0; i < c; ++i)
        fields << rec.fieldName(i);
    while(q.next()) {
        QVariantMap row;
        for (int i = 0; i < c; ++i)
            row[fields[i]] = q.value(i);
        ret << row;
    }
    return ret;
}

void DBWorker::queryAsync(SpatialQuery *job, QString geometryColumn, int reportInterval)
{
    QSqlQuery q(db);
    bool qresult = q.prepare(job->query());
    if (!qresult) {
        qWarning() << q.lastError().text() << "\n" << job->query();
        SpatiaLiteError err;
        switch (q.lastError().type()) {
        case QSqlError::StatementError:
            err = StatementError;
            break;
        case QSqlError::ConnectionError:
            err = ConnectionError;
            break;
        case QSqlError::TransactionError:
            err = TransactionError;
            break;
        case QSqlError::UnknownError:
            err = UnknownError;
            break;
        default:
            break;
        }
        emit error(err, q.lastError().text());
        job->m_error = q.lastError().text();
        emit job->errorChanged();
        QMetaObject::invokeMethod(job, "setComplete", Qt::QueuedConnection);
        return;
    }
//    job->clear();
    q.exec();
    QSqlRecord rec = q.record();
    QStringList columns;
    int colCount = rec.count();
    int gcol = -1;
    for (int i = 0; i < colCount; ++i) {
        if (rec.fieldName(i) == geometryColumn)
            gcol = i;
        else
            columns << rec.fieldName(i);
    }
    QMetaObject::invokeMethod(job, "setColumns", Qt::QueuedConnection, Q_ARG(QStringList, columns));
    QMetaObject::invokeMethod(job, "reportCount", Qt::QueuedConnection);
    emit job->countChanged(0);
    int count = 0;
    while(q.next()) {
        SpatialEntity e;
        if (gcol >= 0) {
            e.wkblob = q.value(gcol).toByteArray();
            e.attributes.resize(colCount-1);
        } else {
            e.attributes.resize(colCount);
        }
        int col = 0;
        for (int i = 0; i < colCount; ++i) {
            if (i == gcol)
                continue;
            e.attributes[col++] = q.value(i);
        }
        job->appendRow(e);
        count++;
        if (!(count % reportInterval)) {
            QMetaObject::invokeMethod(job, "reportCount", Qt::QueuedConnection);
//            emit job->countChanged(job->m_rows.size());
        }
    }
    QMetaObject::invokeMethod(job, "reportCount", Qt::QueuedConnection);
    QMetaObject::invokeMethod(job, "setComplete", Qt::QueuedConnection);
}

void DBWorker::requestVersionInfo()
{
    auto q = db.exec("SELECT sqlite_version()");
    while(q.next())
        m_versionInfo.insert( "sqlite_version", q.value( 0 ).toString() );
    q = db.exec( "SELECT spatialite_version();" );
    while( q.next() )
        m_versionInfo.insert( "spatialite_version", q.value( 0 ).toString() );
    q = db.exec( "SELECT spatialite_target_cpu();" );
    while( q.next() )
        m_versionInfo.insert( "spatialite_target_cpu", q.value( 0 ).toString() );

    q = db.exec( "SELECT proj4_version();" );
    if( q.size() != 0 )
        while( q.next() )
            m_versionInfo.insert( "proj4_version", q.value( 0 ).toString() );
    else
        m_versionInfo.insert( "proj4_version", "unsupported" );

    q = db.exec( "SELECT geos_version();" );
    if( q.size() != 0 )
        while( q.next() )
            m_versionInfo.insert( "geos_version", q.value( 0 ).toString() );
    else
        m_versionInfo.insert( "geos_version", "unsupported" );

    q = db.exec( "SELECT lwgeom_version();" );
    if( q.size() != 0 )
        while( q.next() )
            m_versionInfo.insert( "lwgeom_version", q.value( 0 ).toString() );
    else
        m_versionInfo.insert( "lwgeom_version", "unsupported" );

    q = db.exec( "SELECT libxml2_version();" );
    if( q.size() != 0 )
        while( q.next() )
            m_versionInfo.insert( "libxml2_version", q.value( 0 ).toString() );
    else
        m_versionInfo.insert( "libxml2_version", "unsupported" );

    q = db.exec( "SELECT HasIconv();" );
    while( q.next() )
        m_versionInfo.insert( "HasIconv", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasMathSQL();" );
    while( q.next() )
        m_versionInfo.insert( "HasMathSQL", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasGeoCallbacks();" );
    while( q.next() )
        m_versionInfo.insert( "HasGeoCallbacks", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasProj();" );
    while( q.next() )
        m_versionInfo.insert( "HasProj", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasGeos();" );
    while( q.next() )
        m_versionInfo.insert( "HasGeos", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasGeosAdvanced();" );
    while( q.next() )
        m_versionInfo.insert( "HasGeosAdvanced", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasGeosTrunk();" );
    while( q.next() )
        m_versionInfo.insert( "HasGeosTrunk", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasLwGeom();" );
    while( q.next() )
        m_versionInfo.insert( "HasLwGeom", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasLibXML2();" );
    while( q.next() )
        m_versionInfo.insert( "HasLibXML2", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasEpsg();" );
    while( q.next() )
        m_versionInfo.insert( "HasEpsg", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasFreeXL();" );
    while( q.next() )
        m_versionInfo.insert( "HasFreeXL", q.value( 0 ).toBool() );
    q = db.exec( "SELECT HasGeoPackage();" );
    while( q.next() )
        m_versionInfo.insert( "HasGeoPackage", q.value( 0 ).toBool() );



    emit versionInfo( m_versionInfo );

}


QVariantMap SpatiaLiteDB::versionInfo() const
{
    return m_versionInfo;
}


QRectF SpatiaLiteDB::extent(const QString &layerName, uint srid)
{
    QString q1 = QString("SELECT GetLayerExtent('%1') AS geo").arg(layerName);
    QString what = srid ? QString("Transform(Extent(geo), %1)").arg(srid) : QString("Extent(geo)");
    QString q = QString("SELECT AsGeoJSON(%1) as rect FROM (%2)").arg(what).arg(q1);
    QVariantList retRect = query(q);
    if (retRect.isEmpty())
        return QRectF();
    QJsonDocument doc = QJsonDocument::fromJson(retRect.first().toMap()["rect"].toString().toLocal8Bit());
    auto arr = doc.object().value("coordinates").toArray().first().toArray();
    auto i = arr.begin();

    QPolygonF extent;
    while (i != arr.end()) {
        qreal x =(*i).toArray().first().toDouble();
        qreal y =(*i).toArray().last().toDouble();
        extent << QPointF(x, y);
        ++i;
    }
    return extent.boundingRect();

}

QRectF SpatiaLiteDB::extent(uint srid)
{
    QStringList llist;
    if (m_layers.isEmpty())
        geometryLayers();
    auto it = m_layers.cbegin();
    while (it != m_layers.cend()) {
        QString q = QString("SELECT GetLayerExtent('%1') AS geo").arg(it.key());
        llist << q;
        ++it;
    }
    QString what = srid ? QString("Transform(Extent(geo), %1)").arg(srid) : QString("Extent(geo)");
    QString q = QString("SELECT AsGeoJSON(%1) as rect FROM (%2)").arg(what).arg(llist.join(" UNION "));
    QVariantList retRect = query(q);

    if (retRect.isEmpty()) {
        qWarning() << "null reply for query" << q;
        return QRectF();
    }
    QJsonDocument doc = QJsonDocument::fromJson(retRect.first().toMap()["rect"].toString().toLocal8Bit());
    auto arr = doc.object().value("coordinates").toArray().first().toArray();
    auto i = arr.begin();

    QPolygonF extent;
    while (i != arr.end()) {
        qreal x =(*i).toArray().first().toDouble();
        qreal y =(*i).toArray().last().toDouble();
        extent << QPointF(x, y);
        ++i;
    }
    //    qDebug() << "Bounding rect:" << extent.boundingRect();
    return extent.boundingRect();

}
QVariantList SpatiaLiteDB::query(QString query) const
{
    QVariantList ret;
    QMetaObject::invokeMethod(worker, "querySync", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QVariantList, ret), Q_ARG(QString, query));
    return ret;
}

SpatialQuery *SpatiaLiteDB::postQuery(QString query)
{
    SpatialQuery *ret = new SpatialQuery();
    queryAsync(ret, query);

    return ret;
}

void SpatiaLiteDB::readLayersInfo()
{
    m_layers.clear();
    QVariantList lst = query("SELECT * FROM vector_layers LEFT JOIN vector_layers_statistics ON vector_layers.table_name=vector_layers_statistics.table_name");
    for (QVariant v : lst) {
        QVariantMap vl = v.toMap();
        SpatialLayer l(vl);
        m_layers.insert(l.tableName(), l);
    }
    setReady(true);
}

void SpatiaLiteDB::setReady(bool ready)
{
    if (m_ready == ready)
        return;

    m_ready = ready;
    emit readyChanged(ready);
}

QVariantList SpatiaLiteDB::geometryLayers() const
{
    QVariantList ldata;
    for (SpatialLayer l : m_layers.values()) {
        ldata.append(QVariant::fromValue(l));
    }
    return ldata;
    //    return m_layers.values();
}

QStringList SpatiaLiteDB::allTables() const
{
    QStringList ret;
    QMetaObject::invokeMethod(worker, "tables", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QStringList, ret), Q_ARG(QSql::TableType, QSql::AllTables));
    return ret;
}

QStringList DBWorker::tables(QSql::TableType type) const
{
    return db.tables(type);
}

void SpatialLayer::setLayerType(const QString &value)
{
    m_layerType = value;
}

void SpatialLayer::setSrid(int srid)
{
    if (m_srid == srid)
        return;
    m_srid = srid;
}

void SpatialLayer::setGeomType(const GeomType &value)
{
    m_geometryType = value;
}

void SpatialLayer::setGeometryType(int type)
{
    m_geometryType = (GeomType)(type);
}

int SpatialLayer::geometryKind() const
{
    switch (m_geometryType) {
    case Point:
    case MultiPoint:
        return 1;
    case LineString:
    case MultiLineString:
    case CircularString:
    case CompoundCurve:
    case MultiCurve:
    case Curve:
        return 2;
    case Polygon:
    case MultiPolygon:
    case CurvePolygon:
    case MultiSurface:
    case Surface:
    case PolyhedralSurface:
        return 3;
    case GeometryCollection:
        return 4;
    default:
        break;
    }
    return 0;
}

void SpatialLayer::setTableName(const QString &name)
{
    m_tableName = name;
}


void SpatialLayer::setExtent(const QRectF &extent)
{
    m_extent = extent;
}

void SpatialLayer::setEntityCount(int entityCount)
{
    m_entityCount = entityCount;
}
SpatialLayer::SpatialLayer(QVariantMap row)
{
    m_geometryColumn = row["geometry_column"].toString();
    m_geometryType = (GeomType)row["geometry_type"].toInt();
    m_srid = row["srid"].toInt();
    m_tableName = row["table_name"].toString();
    qreal minX = row["extent_min_x"].toDouble();
    qreal minY = row["extent_min_y"].toDouble();
    qreal maxX = row["extent_max_x"].toDouble();
    qreal maxY = row["extent_max_y"].toDouble();
    m_extent = QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
    m_entityCount = row["row_count"].toInt();

}

int SpatialLayer::srid() const
{
    return m_srid;
}

QString SpatialLayer::geometryColumn() const
{
    return m_geometryColumn;
}

QString SpatialLayer::layerType() const
{
    return m_layerType;
}

int SpatialLayer::geometryType() const
{
    return m_geometryType;
}

bool SpatialLayer::isValid() const
{
    return m_geometryType != Invalid && !m_geometryColumn.isEmpty();
}

QString SpatialLayer::tableName() const
{
    return m_tableName;
}

int SpatialLayer::entityCount() const
{
    return m_entityCount;
}

QRectF SpatialLayer::extent() const
{
    return m_extent;
}

void SpatialLayer::setGeometryColumn(const QString &value)
{
    m_geometryColumn = value;
}
