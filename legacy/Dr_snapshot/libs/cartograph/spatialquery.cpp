#include "spatialquery.h"
#include <QDebug>
#include <QThread>
#include "spatialitedb.h"

ReplyModel::ReplyModel(SpatialQuery *parent) : QAbstractListModel(parent)
{
    connect(parent, &SpatialQuery::statusChanged, this, &ReplyModel::onStatusChanged);
}


void SpatialQuery::appendRow(SpatialEntity row)
{
    setStatus(Fetching);
    m_rows.append(row);
//    qDebug() << "Appending row" << row.attributes << m_rows.size() << m_status;
}

void SpatialQuery::clear()
{
    m_rows.clear();
//    qDebug() << "Clear" << m_rows.size() << this << this->thread() << QThread::currentThread();
}


ReplyModel *SpatialQuery::reply() const
{
    if (m_attributes.isEmpty())
        return nullptr;
    return m_reply;
}

void SpatialQuery::classBegin()
{
    m_reply = new ReplyModel(this);
//    connect(this, &SpatialQuery::complete, this, &SpatialQuery::onComplete);
}

void SpatialQuery::componentComplete()
{
}

bool SpatialQuery::isComplete() const
{
    return m_status == Complete;
}

SpatialQuery::SpatialQuery(QObject *parent) : QObject(parent)
{}

void SpatialQuery::setQuery(QString query)
{
    if (m_query == query)
        return;
    m_query = query;
    emit queryChanged(query);
    if (m_active && canQuery())
        fetch();
}

QVariantMap SpatialQuery::row(int index) const
{
    QVariantMap ret;
    if (index >= m_rows.size())
        return ret;
    SpatialEntity e = m_rows.at(index);
    if (e.attributes.size() < m_attributes.size())
        return ret;
    for (int i = 0; i < m_attributes.size(); ++i) {
        ret.insert(m_attributes.at(i), e.attributes.at(i));
    }
    if (e.entity) {
        ret.insert("polygon", e.entity->getPolygon(QTransform()));
        ret.insert("path", QVariant::fromValue(e.entity->getPath(QTransform())));
    }
    return ret;
}

void SpatialQuery::setDatabase(SpatiaLiteDB *database)
{
    if (m_database == database)
        return;
    m_database = database;
    emit databaseChanged(database);
    if (!m_database) {
        setError("Null database");
    } else {
        setStatus(Ready);
        if (!m_query.isEmpty() && m_active)
            fetch();
    }
}

void SpatialQuery::fetch()
{
//    qDebug() << "Fetch data" << m_query << canQuery() << m_status << canQuery();
    if (!canQuery())
        return;
    setStatus(Queued);
    if (executedQueryHash == qHash(m_query)) {
        setStatus(Complete);
        qDebug() << "query not changed since last result";
    } else {
        m_database->queryAsync(this);
        executedQueryHash = qHash(m_query);
    }
    emit replyChanged();
}

void SpatialQuery::fetchGeometry(QString geomColumn)
{
//    qDebug() << "Fetch geom" << m_query << canQuery() << m_status << geomColumn << canQuery() << m_status;
    if (!canQuery())
        return;
    m_database->queryAsync(this, geomColumn);
    setStatus(Queued);
    emit replyChanged();
}

void SpatialQuery::cancel()
{
    qDebug() << "Implement cancel!";
}

void SpatialQuery::setActive(bool active)
{
    if (m_active == active)
        return;

//    qDebug() << "Set active" << active << "current" << m_active << "status" << m_status << "query" << m_query;
    m_active = active;
    if (!m_active && busy())
        cancel();

//    m_rows.clear();
    emit activeChanged(active);
    setStatus(Ready);
    if (m_active && canQuery())
        fetch();
}

void SpatialQuery::setColumns(QStringList columns)
{
    m_attributes = columns;
//    qDebug() << "Columns arrived" << columns;
    if (m_reply)
        emit replyChanged();
    emit attributesChanged();
}


void SpatialQuery::reportCount()
{
    emit countChanged(m_rows.size());
}

void SpatialQuery::setComplete()
{
    if (m_status == Complete)
        return;
    setStatus(Complete);
    emit completeChanged();
    emit complete();
}

void SpatialQuery::setStatus(SpatialQuery::QueryStatus status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

void SpatialQuery::setError(const QString &message)
{
    m_error = message;
    emit errorChanged();
    setStatus(Error);
}


int ReplyModel::count() const
{
    return d()->rows().size();
}

SpatialQuery *ReplyModel::d() const
{
    return qobject_cast<SpatialQuery*>(parent());
}

int ReplyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return count();
}

QVariant ReplyModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    int r = index.row();
    if (r >= d()->rows().size())
        return QVariant();
    if (role == PolygonRole)
        return QVariant(d()->rows().at(r).entity->getPolygon(QTransform()));
    if (role == PathRole)
        return QVariant::fromValue(d()->rows().at(r).entity->getPolygon(QTransform()));

    return QVariant::fromValue(d()->rows().at(r).attributes.at(role - PolygonRole - 1));
}

QHash<int, QByteArray> ReplyModel::roleNames() const
{
    QHash<int, QByteArray> ret;
    for (int i = 0; i < d()->attributes().size(); ++i) {
        ret.insert(PolygonRole + i + 1, d()->attributes().at(i).toLocal8Bit());
    }
    ret[PathRole] = "path";
    ret[PolygonRole] = "polygon";
    return ret;
}


void ReplyModel::onStatusChanged()
{
    switch (d()->status()) {
    case SpatialQuery::Fetching:
        beginResetModel();
        break;
    case SpatialQuery::Complete:
        endResetModel();
        break;
    default:
        break;
    }
}
