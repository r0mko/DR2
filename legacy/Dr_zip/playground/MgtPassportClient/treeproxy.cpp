#include "treeproxy.h"



TreeProxy::TreeProxy(QObject *parent):
    QAbstractItemModel( parent )
{
}

QModelIndex TreeProxy::index(int row, int column, const QModelIndex &parent) const
{
    if( m_sourceModel == nullptr ) return QModelIndex();


    return QModelIndex();
}

QModelIndex TreeProxy::parent(const QModelIndex &child) const
{
    if( m_sourceModel == nullptr ) return QModelIndex();
    return QModelIndex();
}

int TreeProxy::rowCount(const QModelIndex &parent) const
{
    if( m_sourceModel == nullptr ) return 0;
    int result = 0;
    if( parent == QModelIndex() ) {
//        for( int i = 0; i < m_sourceModel->rowCount(); ++i ) {
//            if( m_sourceModel->data())
//            result ++
//        }
    }
    return 0;
}

int TreeProxy::columnCount(const QModelIndex &parent) const
{
    if( m_sourceModel == nullptr ) return 0;
    return 1;
}

QVariant TreeProxy::data(const QModelIndex &index, int role) const
{
    if( m_sourceModel == nullptr ) return QVariant();

    return QVariant();
}
