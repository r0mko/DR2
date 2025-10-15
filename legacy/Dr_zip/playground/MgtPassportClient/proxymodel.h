#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QSortFilterProxyModel>
#include "filterobject.h"
#include <QQmlListProperty>
class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_CLASSINFO("DefaultProperty", "filters")

    Q_PROPERTY(QObject* source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QByteArray sortRole READ sortRole WRITE setSortRole NOTIFY sortRoleChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)


    Q_PROPERTY(QQmlListProperty<FilterObject> filters READ qmlFilters NOTIFY filtersChanged)
    QList<FilterObject*> m_filters;
    QQmlListProperty<FilterObject> qmlFilters();
    static void qmlFiltersAppend(QQmlListProperty<FilterObject> *list, FilterObject *object);
    static int qmlFiltersCount(QQmlListProperty<FilterObject> *list);
    static FilterObject *qmlFiltersAt(QQmlListProperty<FilterObject> *list, int index);

    QList< FilterObject* > m_staticFilters;

    int m_sortRoleId;
    int m_sortType = 0;

public:
    explicit ProxyModel(QObject *parent = 0);
    ~ProxyModel();

public:
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE int mapToSourceRow(int idx) const;
    Q_INVOKABLE QVariantMap getItemData(int idx) const;
    Q_INVOKABLE int getRowByData( QString role, QVariant value ) const;
    Q_INVOKABLE int getcount() const;

    Q_INVOKABLE QVariantMap item(int row) const;

public slots:
    bool hasFilter( QString role ) const;
    int filterCount( QString role ) const;
    void removeAllFilters( QString role = QString() );
    void removeFilter( FilterObject *o );

    void addStaticFilter( FilterObject *o );


protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QByteArray m_sortRole;
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;


signals:
    void sourceChanged(QObject *arg);
    void sortRoleChanged(QByteArray arg);
    void sortOrderChanged(Qt::SortOrder arg);
    void filtersChanged();


public :
    QObject* source() const;
    QByteArray sortRole() const;
    Qt::SortOrder sortOrder() const;


public slots:
    void setSource(QObject *arg);
    void addFilter(FilterObject *o );
    void setSortRole(QByteArray arg);
    void setSortOrder(Qt::SortOrder arg);

};

#endif // PROXYMODEL_H
