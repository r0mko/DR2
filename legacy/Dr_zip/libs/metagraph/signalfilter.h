#ifndef SIGNALFILTER_H
#define SIGNALFILTER_H

#include <QPersistentModelIndex>

class SignalFilter
{
public: // enum
    enum ModeFlag {
        None =      0x1,
        Node =      0x2,
        Children =  0x4,
        Tree =      0x8,
        Leave =     0x10, ///< items moved outside the subtree/list
        Enter =     0x20, ///< items moved into the subtree/list from outside
        All =       0xe
    };
    Q_DECLARE_FLAGS(ModeFlags, ModeFlag)

public:
    SignalFilter() {}
    ~SignalFilter() { disconnectFilter(); }

    bool initFilter(QPersistentModelIndex index, ModeFlags mode = Node);

protected:
    inline QPersistentModelIndex index() const { return m_index; }
    inline ModeFlags mode() const { return m_mode; }
    inline QAbstractItemModel *model() const { return m_model; }

    virtual void onColumnsAboutToBeInserted(ModeFlags mode, const QModelIndex & parent, int first, int last) { (void)mode; (void)parent; (void)first; (void)last; }
    virtual void onColumnsAboutToBeMoved(ModeFlags mode, const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn) { (void)mode; (void)sourceParent; (void)sourceStart; (void)sourceEnd; (void)destinationParent; (void)destinationColumn; }
    virtual void onColumnsAboutToBeRemoved(ModeFlags mode, const QModelIndex & parent, int first, int last) { (void)mode; (void)parent; (void)first; (void)last; }
    virtual void onColumnsInserted(ModeFlags mode, const QModelIndex & parent, int first, int last) { (void)mode; (void)parent; (void)first; (void)last; }
    virtual void onColumnsMoved(ModeFlags mode, const QModelIndex & parent, int start, int end, const QModelIndex & destination, int column) { (void)mode; (void)parent; (void)start; (void)end; (void)destination; (void)column; }
    virtual void onColumnsRemoved(ModeFlags mode, const QModelIndex & parent, int first, int last) { (void)mode; (void)parent; (void)first; (void)last; }
    virtual void onDataChanged(ModeFlags mode, const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles = QVector<int> ()) { (void)mode; (void)topLeft; (void)bottomRight; (void)roles; }
    virtual void onHeaderDataChanged(ModeFlags mode, Qt::Orientation orientation, int first, int last) { (void)mode; (void)orientation; (void)first; (void)last; }
    virtual void onLayoutAboutToBeChanged(ModeFlags mode, const QList<QPersistentModelIndex> & parents = QList<QPersistentModelIndex> (), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) { (void)mode; (void)parents; (void)hint; }
    virtual void onLayoutChanged(ModeFlags mode, const QList<QPersistentModelIndex> & parents = QList<QPersistentModelIndex> (), QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint) { (void)mode; (void)parents; (void)hint; }
    virtual void onModelAboutToBeReset() {}
    virtual void onModelReset() {}
    virtual void onRowsAboutToBeInserted(ModeFlags mode, const QModelIndex & parent, int start, int end) { (void)mode; (void)parent; (void)start; (void)end; }
    virtual void onRowsAboutToBeMoved(ModeFlags mode, const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow) { (void)mode; (void)sourceParent; (void)sourceStart; (void)sourceEnd; (void)destinationParent; (void)destinationRow; }
    virtual void onRowsAboutToBeRemoved(ModeFlags mode, const QModelIndex & parent, int first, int last) { (void)mode; (void)parent; (void)first; (void)last; }
    virtual void onRowsInserted(ModeFlags mode, const QModelIndex & parent, int first, int last) { (void)mode; (void)parent; (void)first; (void)last; }
    virtual void onRowsMoved(ModeFlags mode, const QModelIndex & parent, int start, int end, const QModelIndex & destination, int row) { (void)mode; (void)parent; (void)start; (void)end; (void)destination; (void)row; }
    virtual void onRowsRemoved(ModeFlags mode, const QModelIndex & parent, int first, int last) { (void)mode; (void)parent; (void)first; (void)last; }

private: // members
    QPersistentModelIndex               m_index;
    QAbstractItemModel*                 m_model { nullptr };
    ModeFlags                           m_mode{Node};
    QVector<QMetaObject::Connection>    m_connections;

    ModeFlags isRelevant(const QModelIndex &, int, int, const QModelIndex &, int) const;
    ModeFlags isRelevant(const QModelIndex &, int, int) const;
    ModeFlags isRelevant(const QModelIndex &, const QModelIndex &) const;
    void disconnectFilter();
    bool connectFilter();

};


#endif // SIGNALFILTER_H

