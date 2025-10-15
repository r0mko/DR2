#include "signalfilter.h"
#include "graphmodel.h"
#include <QDebug>



SignalFilter::ModeFlags SignalFilter::isRelevant(const QModelIndex &parent, int sfirst, int slast, const QModelIndex &destParent, int dfirst) const // move items
{
    ModeFlags ret{None};
    if ( parent == m_index.parent() && m_index.row() >= sfirst && m_index.row() <= slast ) // parent about to be changed
        ret |= Node;
    if( m_mode == Node ) return ret;

    if ( destParent == m_index ) { // insert to list
        ret |= Enter;
        ret |= Children;
    }
    if ( parent == m_index ) {
        if ( ret & Enter)
            ret &= ~Enter;
        else
            ret |= Leave;
        ret |= Children;
    }
    return ret;
    if (m_mode == Children ) return ret;

    if ( isParentOf(destParent, m_index ) ) { // insert to subtree
        ret |= Enter;
        ret |= Tree;
    }
    if ( isParentOf(parent, m_index) ) {
        if ( ret & Enter)
            ret &= ~Enter;
        else
            ret |= Leave;
        ret |= Children;
    }
    return ret;
}


SignalFilter::ModeFlags SignalFilter::isRelevant(const QModelIndex &parent, int first, int last) const // insert, delete
{
    ModeFlags ret{None};
    if ( parent == m_index.parent() && m_index.row() >= first && m_index.row() <= last )
        ret |= Node;
    if ( m_mode == Node ) return ret;

    if ( parent == m_index ) ret |= Children; // i think there will be new children to read
    if ( m_mode == Children ) return ret;

    if ( isParentOf(parent, m_index) ) ret |= Tree;

    return ret;
}

SignalFilter::ModeFlags SignalFilter::isRelevant(const QModelIndex &topLeft, const QModelIndex &bottomRight) const // data changes
{
    ModeFlags ret{None};
    if (!topLeft.isValid() || !bottomRight.isValid()) return ret;

    if ( m_index.row() >= topLeft.row() && m_index.row() <= bottomRight.row() ) ret |= Node;
    if ( m_mode == Node ) return ret;

    if ( topLeft.parent() == m_index ) ret |= Children;
    if ( m_mode == Children ) return ret;

    if ( isParentOf(topLeft, m_index) ) ret |= Tree;

    return ret;
}

bool SignalFilter::initFilter(QPersistentModelIndex index, SignalFilter::ModeFlags mode)
{
    if( !index.isValid() ) return false;
    if( m_index == index && m_mode == mode ) return true;
    if( m_index != index ) {
        m_index = index;
        disconnectFilter();
        m_model = const_cast< QAbstractItemModel* >( index.model() );
    }
    if( m_model == nullptr ) {
        qDebug() << "got some troubles initializing filter: could not get model from index provided:" << index;
        m_index = QPersistentModelIndex();
        m_mode = Node;
        return false;
    }
    if( m_mode != mode ) m_mode = mode;
    return connectFilter();
}

void SignalFilter::disconnectFilter()
{
    while( !m_connections.isEmpty() )
        QObject::disconnect( m_connections.takeLast() );
}

bool SignalFilter::connectFilter()
{
    m_connections << QObject::connect(m_model, &QAbstractItemModel::dataChanged, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){

        ModeFlags m = isRelevant(topLeft, bottomRight);
        if( m != None )
            onDataChanged(m, topLeft, bottomRight, roles);
    });

    m_connections << QObject::connect(m_model, &QAbstractItemModel::modelAboutToBeReset, [this](){
        onModelAboutToBeReset();
    });

    m_connections << QObject::connect(m_model, &QAbstractItemModel::modelReset, [this](){
        onModelReset();
    });

    m_connections << QObject::connect(m_model, &QAbstractItemModel::rowsAboutToBeInserted, [this](const QModelIndex &parent, int start, int end){
        ModeFlags m = isRelevant(parent, start, end);
        if( m != None )
            onRowsAboutToBeInserted(m, parent, start, end);
    });

    m_connections << QObject::connect(m_model, &QAbstractItemModel::rowsAboutToBeMoved, [this](const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow){
        ModeFlags m = isRelevant(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow );
        if (m & (Children | Tree)) {
            if (m & Enter)
                onRowsAboutToBeInserted(m, destinationParent, destinationRow, destinationRow + (sourceEnd - sourceStart));
            else if (m & Leave)
                onRowsAboutToBeRemoved(m, sourceParent, sourceStart, sourceEnd);
            else
                onRowsAboutToBeMoved(m, sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow);
        }
    });

    m_connections << QObject::connect(m_model, &QAbstractItemModel::rowsAboutToBeRemoved, [this](const QModelIndex &parent, int first, int last){
        ModeFlags m = isRelevant(parent, first, last);
        if( m != None )
            onRowsAboutToBeRemoved(m, parent, first, last);
    });

    m_connections << QObject::connect(m_model, &QAbstractItemModel::rowsInserted, [this](const QModelIndex &parent, int first, int last){
        ModeFlags m = isRelevant(parent, first, last);
        if( m != None )
            onRowsInserted(m, parent, first, last);
    });
    m_connections << QObject::connect(m_model, &QAbstractItemModel::rowsMoved, [this](const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow){
        ModeFlags m = isRelevant(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow );
        if (m & (Children | Tree)) {
            if (m & Enter)
                onRowsInserted(m, destinationParent, destinationRow, destinationRow + (sourceEnd - sourceStart));
            else if (m & Leave)
                onRowsRemoved(m, sourceParent, sourceStart, sourceEnd);
            else
                onRowsMoved(m, sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow);
        }
    });

    m_connections << QObject::connect(m_model, &QAbstractItemModel::rowsRemoved, [this](const QModelIndex &parent, int first, int last){
        ModeFlags m = isRelevant(parent, first, last);
        if( m != None )
            onRowsRemoved(m, parent, first, last);
    });

    for( auto c: m_connections )
        if( !c ) return false;
    return true;
}



