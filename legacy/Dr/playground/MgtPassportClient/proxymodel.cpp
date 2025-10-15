#include "proxymodel.h"
#include <QDate>
#include <QHashIterator>
#include "partmodel.h"
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlExpression>
#include "qqml.h"
#include <QVariantList>
#include "filterdefinition.h"

QQmlListProperty<FilterObject> ProxyModel::qmlFilters() {
    return QQmlListProperty<FilterObject>(this, 0,
                                          &ProxyModel::qmlFiltersAppend,
                                          &ProxyModel::qmlFiltersCount,
                                          &ProxyModel::qmlFiltersAt,
                                          QQmlListProperty<FilterObject>::ClearFunction());
}

void ProxyModel::qmlFiltersAppend(QQmlListProperty<FilterObject> *list, FilterObject *object)
{
    ProxyModel *o = qobject_cast<ProxyModel*>(list->object);
    o->m_filters.append(object);
    emit o->filtersChanged();
}

int ProxyModel::qmlFiltersCount(QQmlListProperty<FilterObject> *list)
{
    ProxyModel *o = qobject_cast<ProxyModel*>(list->object);
    return o->m_filters.size();
}

FilterObject *ProxyModel::qmlFiltersAt(QQmlListProperty<FilterObject> *list, int index)
{
    ProxyModel *o = qobject_cast<ProxyModel*>(list->object);
    return o->m_filters.at(index);
}

ProxyModel::ProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

ProxyModel::~ProxyModel() {}

QHash<int, QByteArray> ProxyModel::roleNames() const
{
    if( sourceModel() != nullptr )
        return sourceModel()->roleNames();
    return QHash<int, QByteArray>();
}

bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if( !m_staticFilters.isEmpty() ) {
        for( int i = 0; i < m_staticFilters.size(); ++i ) {
            const QString role = m_staticFilters.at( i )->role();
            const int roleKey = roleNames().key( role.toLatin1(), -1 );
            if( roleKey == -1 ) continue;
            FilterObject* fo = m_staticFilters.at( i );
            fo->setProperty( "value", sourceModel()->data( sourceModel()->index(source_row, 0, source_parent), roleKey ) );
            QObject* qo = fo->property( "filter" ).value<QObject*>();
            if( qo != nullptr ) {
                FilterDefinition* fd = qobject_cast<FilterDefinition*>( qo );
                if( fd != nullptr ) {
                    bool x = fd->evalCompare( fo );
                    if( !x ) return false;
                }
            }
        }
    }
    if( !m_filters.isEmpty() ) {
        for( int i = 0; i < m_filters.size(); ++i ) {
            const QString role = m_filters.at( i )->role();
            const int roleKey = roleNames().key( role.toLatin1(), -1 );
            if( roleKey == -1 ) continue;
            FilterObject* fo = m_filters.at( i );
            fo->setProperty( "value", sourceModel()->data( sourceModel()->index(source_row, 0, source_parent), roleKey ) );
            QObject* qo = fo->property( "filter" ).value<QObject*>();
            if( qo != nullptr ) {
                FilterDefinition* fd = qobject_cast<FilterDefinition*>( qo );
                if( fd != nullptr ) {
                    bool x = fd->evalCompare( fo );
                    if( !x ) return false;
                }
            }
        }
    }
    return true;
}

bool ProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{

    if( m_sortType == 1 ) return left.data( m_sortRoleId ).toInt() < right.data( m_sortRoleId ).toInt();
    else if( m_sortType == 2 ) return left.data( m_sortRoleId ).toFloat() < right.data( m_sortRoleId ).toFloat();
    else if( m_sortType == 3 ) return left.data( m_sortRoleId ).toDate() < right.data( m_sortRoleId ).toDate();

    return QSortFilterProxyModel::lessThan( left, right );
}

QObject *ProxyModel::source() const
{
    return sourceModel();
}

QByteArray ProxyModel::sortRole() const
{
    return m_sortRole;
}

Qt::SortOrder ProxyModel::sortOrder() const
{
    return m_sortOrder;
}

void ProxyModel::setSource(QObject *arg)
{
    setSourceModel( qobject_cast< QAbstractItemModel* >( arg ) );
    emit sourceChanged(arg);
}

void ProxyModel::addFilter(FilterObject* o)
{
    FilterObject* fo = qobject_cast<FilterObject*>( o );
    if( fo != nullptr ) {
        if( !fo->criteria().isValid() ) return;
        if( fo->role().isEmpty() ) return;
        m_filters.append( fo );
        emit filtersChanged();
        invalidate();
    }
}

void ProxyModel::setSortRole(QByteArray arg)
{
    if (m_sortRole == arg)
        return;
    m_sortRole = arg;
    m_sortRoleId = roleNames().key( arg );
    PartModel* pm = qobject_cast<PartModel*>(sourceModel());
    if( pm != nullptr ){
        for( int i = 0; i < pm->fieldData().size(); ++i ) {
            if( pm->fieldData().at( i ).toMap().value( "name" ) == sortRole() ) {
                if( pm->fieldData().at( i ).toMap().value( "tableDelegate" ).toString() == "int" ) m_sortType = 1;
                else if( pm->fieldData().at( i ).toMap().value( "tableDelegate" ).toString() == "real" ) m_sortType = 2;
                else if( pm->fieldData().at( i ).toMap().value( "tableDelegate" ).toString() == "date" ) m_sortType = 3;
                else m_sortType = 0;
            }
        }
    }

    QSortFilterProxyModel::setSortRole( roleNames().key( arg ) );
    QSortFilterProxyModel::sort( 0, m_sortOrder );
    emit sortRoleChanged(arg);
}

void ProxyModel::setSortOrder(Qt::SortOrder arg)
{
    if (m_sortOrder == arg)
        return;
    m_sortOrder = arg;
    QSortFilterProxyModel::sort( 0, m_sortOrder );
    emit sortOrderChanged(arg);
}

int ProxyModel::mapToSourceRow(int idx) const
{
    return mapToSource( index( idx, 0 ) ).row();
}

QVariantMap ProxyModel::getItemData(int idx) const
{
    QVariantMap d;
    const auto &roles = roleNames();
    for( auto it = roles.constBegin(); it != roles.constEnd(); ++it )
        d.insert( it.value(), data( index( idx, 0 ), it.key() ) );
    return d;
}

int ProxyModel::getRowByData(QString role, QVariant value) const
{
    if( value.toInt() == -1 ) return -1;
    int rk = roleNames().key( role.toLatin1() );
//    qDebug() << "roles" << role << rk << roleNames().count();
    for( int i = 0; i < rowCount(); ++i ) {
//        qDebug() << data( index( i, 0 ), rk ) << value;
        if( data( index( i, 0 ), rk ).toInt() == value.toInt() )
            return i;
    }
    return -1;
}

int ProxyModel::getcount() const
{
    return this->rowCount();
}

QVariantMap ProxyModel::item(int row) const
{
    QVariantMap m;
    for( int k: roleNames().keys() ) m.insert( QString( roleNames().value( k ) ), data( index( row, 0 ), k ) );
    return m;
}

bool ProxyModel::hasFilter(QString role) const
{
    for( int i = 0; i < m_filters.size(); ++i )
        if( m_filters.at( i )->role() == role ) return true;
    return false;
}

int ProxyModel::filterCount(QString role) const
{
    int x = 0;
    for( int i = 0; i < m_filters.size(); ++i )
        if( m_filters.at( i )->role() == role ) x++;
    return x;
}

void ProxyModel::removeAllFilters(QString role)
{
    auto it = m_filters.begin();
    if( role.isEmpty() ) {
        while( !m_filters.isEmpty() ) m_filters.takeFirst()->deleteLater();
    }else{
        while( it != m_filters.end() )
            if( (*it)->role() == role ) {
                (*it)->deleteLater();
                it = m_filters.erase( it );
            }
            else ++it;
    }
    invalidate();
    emit filtersChanged();
}

void ProxyModel::removeFilter(FilterObject *o)
{
    auto it = m_filters.begin();
    while( it != m_filters.end() )
        if( (*it) == o ) {
            (*it)->deleteLater();
            it = m_filters.erase( it );
            break;
        }
        else ++it;
    invalidate();
    emit filtersChanged();
}

void ProxyModel::addStaticFilter(FilterObject *o)
{
//    qDebug() << "static filter added";
    m_staticFilters.append( o );
    connect( o, &FilterObject::changed, this, &ProxyModel::invalidate );
    invalidate();
}


