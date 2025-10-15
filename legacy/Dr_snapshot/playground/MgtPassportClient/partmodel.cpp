#include "partmodel.h"
#include <QDataStream>
#include <QDebug>
#include <QTimer>
//#include "service.h"

PartModel::PartModel(QObject *parent) :
    QAbstractListModel(parent),
    m_viewPosition( 0 ),
    m_rowCount( 0 ),
    m_currentDataRowStart( 0 ),
    m_currentDataRowEnd( 0 ),
    m_fieldsDirty( true ),
    m_rowCountDirty( true ),
    m_dataDirty( true ),
    m_windowSize( 5500 ),
    m_modelService( nullptr ),
    m_editable( false ),
    m_currentSelectedSiteId( -1 )
{
    updateData();
    connect( this, &PartModel::sortFieldChanged, this, &PartModel::updateData );
    connect( this, &PartModel::sortAscendingChanged, this, &PartModel::updateData );
    connect( this, &PartModel::currentSelectedSiteIdChanged, this, &PartModel::updateData );
    connect( this, &PartModel::parentIdChanged, this, &PartModel::updateData );
}

PartModel::~PartModel()
{
}

QVariant PartModel::data(const QModelIndex &index, const int role) const
{
    const int row = index.row();
    m_viewPosition = row;
    if( row < 0 || row >= rowCount() ) return QVariant();

    if( role == Qt::UserRole + 1 )
        if( !checkSafeZone() )
            QTimer::singleShot( 0, this, &PartModel::getMoreData );

    if( !m_data.contains( row ) ) return QVariant( m_loadingText );
    return m_data.value( row ).value( role );
}

int PartModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED( parent )
    return m_rowCount;
}

QHash<int, QByteArray> PartModel::roleNames() const
{
    return m_roleHash;
}

QVariantMap PartModel::item( int row ) const
{
    QVariantMap m;
    for( int k: m_roleHash.keys() ) m.insert( QString( m_roleHash.value( k ) ), m_data.value( row ).value( k ) );
    return m;
}

int PartModel::listIndex(int index, const QString &role) const
{
    if( m_data.size() == 0 ) return 0;
    if( !item( 0 ).contains( role ) ) return 0;
    int i = index;
    if( m_data.size() < i ) i = m_data.size() - 1;
    while( i >= 0 ) {
        if( item( i ).value( role ) == index )
            return i - 1;
        i--;
    }
    return 0;
}

int PartModel::realIndex(int listIndex, const QString &role) const
{
    if( m_data.size() == 0 ) return 0;
    if( !item( 0 ).contains( role ) ) return -1;
    for( int i = 0; i < m_roleHash.keys().size(); ++i )
        if( m_roleHash.value( m_roleHash.keys()[ i ] ) == role )
            return m_data.value( listIndex ).value( m_roleHash.keys()[ i ] ).toInt();
    return -1;
}

bool PartModel::hasFilter(const QString &role) const
{
    return m_filters.contains( role );
}

void PartModel::removeAllFilters()
{
    m_filters.clear();
}

int PartModel::fieldDataIndexByRole(const QString &role) const
{
    if( m_data.size() == 0 ) return 0;
    if( !item( 0 ).contains( role ) ) return -1;
    for( int i = 0; i < m_fieldData.size(); ++i )
        if( m_fieldData.value( i ).toMap().value( "name" ) == role )
            return i;
    return -1;
}

QVariantMap PartModel::readBlob(QVariant a) const
{
    QVariantMap xx;
    QByteArray ba = a.toByteArray();
    QDataStream in( &ba, QIODevice::ReadOnly );
    in >> xx;
    qDebug() << "reading blob" << xx;
    return xx;
}

QVariantMap PartModel::dataRequestMessage() const
{
    if( m_dataDirty || m_rowCountDirty || m_fieldsDirty ) {
        QVariantMap message;

        QVariantMap vmo;
        vmo.insert( "field", m_sortField );
        vmo.insert( "ascending", m_sortAscending );
        message.insert( "orderBy", vmo );
        message.insert( "method", "GET" );
        message.insert( "dataSource", m_dataSource );
        message.insert( "messageType", "dataRequest" );
//        if( m_currentSelectedSiteId > 0 )
            message.insert( "currentSelectedSiteId", m_currentSelectedSiteId );
            message.insert( "parentId", m_parentId );

        int pos = m_viewPosition - m_windowSize / 2;
        if( pos < 0 ) pos = 0;
        message.insert( "filters", m_filters );

        message.insert( "windowPosition", pos ); // TODO: get actual value

        message.insert( "windowSize", m_windowSize );

        if( m_rowCountDirty ) message.insert( "getRowCount", true );
        if( m_fieldsDirty ) message.insert( "getFieldData", true );
        m_dataDirty = false;
        m_rowCountDirty = false;
        m_fieldsDirty = false;
        return message;
    }
    return QVariantMap();
}

void PartModel::getMoreData()
{
    m_dataDirty = true;
    if( m_modelService != nullptr )
        m_modelService->queueRequest( this );
//    emit needDataSignal( this );
}

void PartModel::onModelServiceConnected()
{

}

void PartModel::onModelServiceDisconnected()
{

}

void PartModel::placeData(const QVariantMap &data)
{
//    if( m_rowCount != data.value( "rowCount" ).toInt() ) {
    if( data.contains( "rowCount" ) ) {
        beginResetModel();

        m_rowCount = data.value( "rowCount", 0 ).toInt();
        if( data.contains( "fieldData" ) ) {


            m_fieldData = data.value( "fieldData" ).toList();
            emit fieldDataChanged( m_fieldData );



            m_editable = data.value( "editable", false ).toBool();
            emit editableChanged( m_editable );
            m_indexField = data.value( "indexField", "" ).toString();
            emit indexFieldChanged(m_indexField);
            m_readableField = data.value( "readableField", "" ).toString();
            emit readableFieldChanged(m_readableField);
        }

        const QVariantMap& fieldMap = data.value( "fieldMap" ).toMap();
        for( int i = 0; i < fieldMap.keys().size(); ++i )
            m_roleHash.insert( Qt::UserRole + 1 + i, fieldMap.keys().value( i ).toLocal8Bit() );


        const QVariantList& recordList = data.value( "recordList" ).toList();
        for( int i = 0; i < recordList.size(); ++i )
        {
            QHash< int, QVariant > roleItem;
            for( int k: m_roleHash.keys() )
                roleItem.insert( k, recordList.at( i ).toMap().value( m_roleHash.value( k ) ) );
            m_data.insert( 0 + i, roleItem );
        }
        endResetModel();
        emit rowCountChanged( m_rowCount );
    }else{
        m_data.clear();
        const QVariantList& recordList = data.value( "recordList" ).toList();
        for( int i = 0; i < recordList.size(); ++i )
        {
            QHash< int, QVariant > roleItem;
            for( int k: m_roleHash.keys() )
                roleItem.insert( k, recordList.at( i ).toMap().value( m_roleHash.value( k ) ) );
            m_data.insert( data.value( "pos" ).toInt() + i, roleItem );
        }
        dataChanged( index( 0 ), index( m_rowCount - 1 ) );
    }
    m_currentDataRowStart = data.value( "pos" ).toInt();
    m_currentDataRowEnd = m_currentDataRowStart + m_windowSize - 1;
    emit dataUpdated();
}

void PartModel::updateData()
{
//    emit needDataSignal( this );

    m_dataDirty = true;
    m_rowCountDirty = true;
    m_fieldsDirty = true;
    emit queueMe( this );

//    m_modelService->queueRequest( this );
}

bool PartModel::checkSafeZone() const
{
    if( m_rowCount == 0 ) return false;
    int backward = m_viewPosition - m_windowSize / 4;
    if( backward < 0 ) backward = 0;
    int forward = m_viewPosition + m_windowSize / 4;
    if( forward >= m_rowCount ) forward = m_rowCount - 1;
    return ( backward >= m_currentDataRowStart ) && ( forward <= m_currentDataRowEnd );

    //    return m_data.contains( forward ) && m_data.contains( backward );
}

void PartModel::addItem(const QVariantMap &item)
{
    if( m_modelService == nullptr ) return;
    QVariantMap m;
    m.insert( "method", "ADD" );
    m.insert( "dataSource", m_dataSource );
    m.insert( "messageType", "dataAdd" );
    m.insert( "data", item );
//    m.insert( "values", item.value( "values" ).toList() );
    m_modelService->sendMessage( m );
}

void PartModel::editItem(const QVariantMap &item)
{
    if( m_modelService == nullptr ) return;
    QVariantMap m;
    m.insert( "method", "UPD" );
    m.insert( "dataSource", m_dataSource );
    m.insert( "messageType", "dataEdit" );
//    m.insert( "values", item.value( "values" ).toList() );
    m.insert( "data", item );
    m_modelService->sendMessage( m );

}

void PartModel::delItem(const QVariantMap &item)
{
    if( m_modelService == nullptr ) return;
    QVariantMap m;
    m.insert( "method", "DEL" );
    m.insert( "dataSource", m_dataSource );
    m.insert( "messageType", "dataDel" );
//    m.insert( "id", item );
    m.insert( "data", item );
    m_modelService->sendMessage( m );
}

void PartModel::testf(const QVariantMap &item)
{
    qDebug() << item;
    if( m_modelService == nullptr ) return;
    QVariantMap m;
    m.insert( "method", "TEST" );
    m.insert( "dataSource", m_dataSource );
    m.insert( "messageType", "testf" );
    m.insert( "data", item );
    m_modelService->sendMessage( m );
}

void PartModel::addFilter(const QString &filterName, const QVariantMap &filter)
{
    m_filters.insert( filterName, filter );
    qDebug() << m_filters.size() << filter;
    m_dataDirty = true;
    m_fieldsDirty = true;
    m_rowCountDirty = true;
    if( m_modelService != nullptr )
        m_modelService->queueRequest( this );
//    emit
}

void PartModel::removeFilter(const QString &filterName)
{
    m_filters.remove( filterName );
    qDebug() << m_filters;
    m_dataDirty = true;
    m_fieldsDirty = true;
    m_rowCountDirty = true;
    if( m_modelService != nullptr )
        m_modelService->queueRequest( this );
}

QVariantMap PartModel::getFilter(const QString &filterName) const
{
    qDebug() << "c++ partmodel debug" << filterName << m_filters.value( filterName ).toMap();
    return m_filters.value( filterName ).toMap();
}



