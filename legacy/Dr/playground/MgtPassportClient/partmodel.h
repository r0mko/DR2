#ifndef PARTMODEL_H
#define PARTMODEL_H

#include <QAbstractListModel>
//class Service;
#include "service.h"
#include "field.h"
#include <QDebug>
#include <QJSValue>
class PartModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString loadingText READ loadingText WRITE setLoadingText NOTIFY loadingTextChanged)
    Q_PROPERTY(int windowSize READ windowSize WRITE setWindowSize NOTIFY windowSizeChanged)
    Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)
    Q_PROPERTY(QString sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(QString sortAscending READ sortAscending WRITE setSortAscending NOTIFY sortAscendingChanged)
    Q_PROPERTY(Service* modelService READ modelService WRITE setModelService NOTIFY modelServiceChanged)
    Q_PROPERTY(QVariantList fieldData READ fieldData NOTIFY fieldDataChanged)
    Q_PROPERTY(bool editable READ editable NOTIFY editableChanged)
    Q_PROPERTY(QString indexField READ indexField NOTIFY indexFieldChanged)
    Q_PROPERTY(QString readableField READ readableField NOTIFY readableFieldChanged)
    Q_PROPERTY(int currentSelectedSiteId READ currentSelectedSiteId WRITE setCurrentSelectedSiteId NOTIFY currentSelectedSiteIdChanged)
    Q_PROPERTY(int parentId READ parentId WRITE setParentId NOTIFY parentIdChanged)




public:
    explicit PartModel(QObject *parent = 0);
    ~PartModel();

public: // model related methods
    Q_INVOKABLE QVariant data(const QModelIndex &index, const int role) const;
    Q_INVOKABLE int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE QVariantMap item(int row) const;
    Q_INVOKABLE int listIndex( int index, const QString& role ) const;
    Q_INVOKABLE int realIndex( int listIndex, const QString& role ) const;


signals:
    void rowCountChanged( int rowCount );
    void needDataSignal( PartModel* me );
    void queueMe( PartModel* me );

private:
    mutable int m_viewPosition;
    QHash< int, QByteArray > m_roleHash;
    int m_rowCount = 0; // total number of rows in request
    QHash< int, QHash< int, QVariant > > m_data; // actual recieved data
    int m_currentDataRowStart;
    int m_currentDataRowEnd;
    QVariantMap m_filters;

    mutable bool m_fieldsDirty;
    mutable bool m_rowCountDirty;
    mutable bool m_dataDirty;

public:
    Q_INVOKABLE QVariantMap dataRequestMessage() const;

private slots:
    void getMoreData(); // get small part
    void onModelServiceConnected();
    void onModelServiceDisconnected();

public slots:
    void placeData( const QVariantMap& data );
    void updateData(); // refresh all data
    bool checkSafeZone() const;


    void addItem(const QVariantMap &item );
    void editItem( const QVariantMap& item );
    void delItem(const QVariantMap& item);
    void testf( const QVariantMap& item );

    void addFilter(const QString &filterName, const QVariantMap& filter );
    void removeFilter(const QString& filterName );
    QVariantMap getFilter(const QString& filterName ) const;
    bool hasFilter(const QString& filterName ) const;
    void removeAllFilters();

    int fieldDataIndexByRole(const QString &role )const;

    QVariantMap readBlob( QVariant a ) const;

signals:
    void dataUpdated();
private:



// ------------- property stuff here --------------- //

private: // property members
    QString m_loadingText;
    int m_windowSize;
    QString m_dataSource;
    QString m_sortField;
    QString m_sortAscending;
    Service* m_modelService;
    QVariantList m_fieldData;
    bool m_editable;
    QString m_indexField;
    QString m_readableField;

    int m_currentSelectedSiteId;

    int m_parentId;

signals: // property signals
    void loadingTextChanged(QString arg);
    void windowSizeChanged(int arg);
    void dataSourceChanged(QString arg);
    void sortFieldChanged(QString arg);
    void sortAscendingChanged(QString arg);
    void modelServiceChanged(Service* arg);
    void fieldDataChanged(QVariantList arg);
    void dirtyChanged(bool arg);
    void fieldsDirtyChanged(bool arg);
    void rowCountDirtyChanged(bool arg);
    void dataDirtyChanged(bool arg);
    void editableChanged(bool arg);
    void indexFieldChanged(QString arg);
    void readableFieldChanged(QString arg);

    void currentSelectedSiteIdChanged(int arg);

    void parentIdChanged(int arg);

public: // property getters
    QString loadingText() const
    {
        return m_loadingText;
    }
    int windowSize() const
    {
        return m_windowSize;
    }
    QString dataSource() const
    {
        return m_dataSource;
    }
    QString sortField() const
    {
        return m_sortField;
    }
    QString sortAscending() const
    {
        return m_sortAscending;
    }

    Service* modelService() const
    {
        return m_modelService;
    }
    QVariantList fieldData() const
    {
        return m_fieldData;
    }
    bool editable() const
    {
        return m_editable;
    }
    QString indexField() const
    {
        return m_indexField;
    }
    QString readableField() const
    {
        return m_readableField;
    }

    int currentSelectedSiteId() const
    {
        return m_currentSelectedSiteId;
    }

    int parentId() const
    {
        return m_parentId;
    }

public slots: // property setters
    void setLoadingText(QString arg)
    {
        if (m_loadingText == arg)
            return;

        m_loadingText = arg;
        emit loadingTextChanged(arg);
    }
    void setWindowSize(int arg)
    {
        if (m_windowSize == arg || m_windowSize < 300 )
            return;

        m_windowSize = arg;
        emit windowSizeChanged(arg);
    }
    void setDataSource(QString arg)
    {
        if (m_dataSource == arg)
            return;

        m_dataSource = arg;
        m_dataDirty = true;
        m_fieldsDirty = true;
        m_rowCountDirty = true;
        if( m_modelService != nullptr )
            m_modelService->queueRequest( this );
        emit dataSourceChanged(arg);
    }
    void setSortField(QString arg)
    {
        if (m_sortField == arg)
            return;
        m_sortField = arg;
        m_dataDirty = true;
        if( m_modelService != nullptr )
            m_modelService->queueRequest( this );
        emit sortFieldChanged(arg);
    }
    void setSortAscending(QString arg)
    {
        if (m_sortAscending == arg)
            return;
        m_sortAscending = arg;
        m_dataDirty = true;
        if( m_modelService != nullptr )
            m_modelService->queueRequest( this );
        emit sortAscendingChanged(arg);
    }
    void setModelService(Service* arg)
    {
        if( arg == nullptr ) return;
        if (m_modelService == arg) return;
        if( m_modelService != nullptr ) m_modelService->disconnect();

        m_modelService = arg;

        connect( m_modelService, &Service::connected,    this, &PartModel::onModelServiceConnected    );
        connect( m_modelService, &Service::disconnected, this, &PartModel::onModelServiceDisconnected );
        connect( this, &PartModel::queueMe, m_modelService, &Service::queueRequest );

        m_dataDirty = true;
        m_fieldsDirty = true;
        m_rowCountDirty = true;
        m_modelService->queueRequest( this );

        emit modelServiceChanged(arg);
    }

    void setCurrentSelectedSiteId(int arg)
    {
        if (m_currentSelectedSiteId == arg)
            return;

        m_dataDirty = true;
        m_fieldsDirty = true;
        m_rowCountDirty = true;
        m_currentSelectedSiteId = arg;
        emit currentSelectedSiteIdChanged(arg);


    }
    void setParentId(int arg)
    {
        if (m_parentId == arg)
            return;
        m_dataDirty = true;
        m_fieldsDirty = true;
        m_rowCountDirty = true;
        m_parentId = arg;
        emit parentIdChanged(arg);
    }
};

#endif // PARTMODEL_H
