#ifndef INSTANTIATORCOLLECTION_H
#define INSTANTIATORCOLLECTION_H

#include <QObject>
#include <QQmlListProperty>
#include <private/qqmlinstantiator_p.h>
#include <private/qtqmlglobal_p.h>
class InstantiatorCollection : public QObject
{
    Q_OBJECT
    friend class AutoList;
public:
    explicit InstantiatorCollection(QObject *parent = 0);
    int size() const;
    bool scan(QObject *object = 0);
signals:
    void instantiatorBound();
    void instantiatorReset();
    void objectAdded(int index, QObject* object);
    void objectRemoved(int index, QObject* object);
    void activeChanged(bool isActive);
    void sizeChanged();

protected slots:
    void onObjectAdded(int index, QObject* object);
    void onObjectRemoved(int index, QObject* object);
    void onActiveChanged();
protected:
    bool introspect(QObject *instantiator, const char *classname);
    virtual void handleObjectAdded(int index, QObject* object);
    virtual void handleObjectRemoved(int index, QObject* object);
    virtual void handleActiveChanged() {}
    virtual bool tryBind(QObject *instantiator) = 0;
    QObject *objectAt(int index) const;

private:
    void bind(QObject *itor);
    struct ItorMeta {
        int p_count = -1;
        int p_active = -1;
        int s_objectAt = -1;
        const QMetaObject *mo = nullptr;
        inline bool isValid() const { return p_count >= 0 && p_active >= 0 && s_objectAt >=0; }
    };

    QVariant readProperty(int idx) const;
    QVariant callMethod(int idx, QVariant val0 = QVariant()) const;
    QObject *i = 0;
    ItorMeta iMeta;



};

template <class T>
class IList : public InstantiatorCollection
{
public:
    explicit IList(QObject *parent = 0) : InstantiatorCollection(parent) { }
    class const_iterator;
    class iterator {
    public:
        IList<T> *d = nullptr;
        int i = 0;
        typedef std::random_access_iterator_tag iterator_category;
        // ### Qt6: use int
        typedef qptrdiff difference_type;
        typedef T *value_type;
        typedef T **pointer;
        typedef T *reference;

        inline iterator() : d(0) {}
        inline iterator(IList<T> *coll, int index) : d(coll), i(index) {}
        inline iterator(const iterator &o): d(o.d), i(o.i) {}
        inline T *operator*() const { return d->at(i); }
        inline T **operator->() const { return &d->at(i); }
        inline T *operator[](difference_type j) const { return d->at(j + i); }
        inline bool operator==(const iterator &o) const { return i == o.i; }
        inline bool operator!=(const iterator &o) const { return i != o.i; }
        inline bool operator<(const iterator& other) const { return i < other.i; }
        inline bool operator<=(const iterator& other) const { return i <= other.i; }
        inline bool operator>(const iterator& other) const { return i > other.i; }
        inline bool operator>=(const iterator& other) const { return i >= other.i; }
        inline bool operator==(const const_iterator &o) const { return i == o.i; }
        inline bool operator!=(const const_iterator &o) const { return i != o.i; }
        inline bool operator<(const const_iterator& other) const { return i < other.i; }
        inline bool operator<=(const const_iterator& other) const { return i <= other.i; }
        inline bool operator>(const const_iterator& other) const { return i > other.i; }
        inline bool operator>=(const const_iterator& other) const { return i >= other.i; }
        inline iterator &operator++() { ++i; return *this; }
        inline iterator operator++(int) { int n = i; ++i; return iterator(d, n); }
        inline iterator &operator--() { i--; return *this; }
        inline iterator operator--(int) { int n = i; i--; return iterator(d, n); }
        inline iterator &operator+=(difference_type j) { i += j; return *this; }
        inline iterator &operator-=(difference_type j) { i -= j; return *this; }
        inline iterator operator+(difference_type j) const { return iterator(d, i+j); }
        inline iterator operator-(difference_type j) const { return iterator(d, i-j); }
        inline int operator-(iterator j) const { return int(i - j.i); }
    };
    friend class iterator;

    class const_iterator {
    public:
        IList<T> *d = nullptr;
        int i = 0;
        typedef std::random_access_iterator_tag  iterator_category;
        // ### Qt6: use int
        typedef qptrdiff difference_type;
        typedef T *value_type;
        typedef const T **pointer;
        typedef const T *reference;

        inline const_iterator() {}
        inline const_iterator(IList<T> *coll, int index) : d(coll), i(index) {}
        inline const_iterator(const const_iterator &o): d(o.d), i(o.i) {}
        inline const_iterator(const iterator &o): d(o.d), i(o.i) {}
        inline const T *operator*() const { return d->at(i); }
        inline const T **operator->() const { return &d->at(i); }
        inline const T *operator[](difference_type j) const { return d->at(j + i); }
        inline bool operator==(const const_iterator &o) const { return i == o.i; }
        inline bool operator!=(const const_iterator &o) const { return i != o.i; }
        inline bool operator<(const const_iterator& other) const { return i < other.i; }
        inline bool operator<=(const const_iterator& other) const { return i <= other.i; }
        inline bool operator>(const const_iterator& other) const { return i > other.i; }
        inline bool operator>=(const const_iterator& other) const { return i >= other.i; }
        inline const_iterator &operator++() { ++i; return *this; }
        inline const_iterator operator++(int) { int n = i; ++i; return const_iterator(d, n); }
        inline const_iterator &operator--() { i--; return *this; }
        inline const_iterator operator--(int) { int n = i; i--; return const_iterator(d, n); }
        inline const_iterator &operator+=(difference_type j) { i+=j; return *this; }
        inline const_iterator &operator-=(difference_type j) { i-=j; return *this; }
        inline const_iterator operator+(difference_type j) const { return const_iterator(d, i+j); }
        inline const_iterator operator-(difference_type j) const { return const_iterator(d, i-j); }
        inline int operator-(const_iterator j) const { return int(i - j.i); }
    };
    friend class const_iterator;
    inline iterator begin() { return iterator(this, 0); }
    inline const_iterator begin() const { return const_iterator(this, 0); }
    inline const_iterator cbegin() const { return const_iterator(this, 0); }
    inline const_iterator constBegin() const { return const_iterator(this, 0); }
    inline iterator end() { return iterator(this, size()); }
    inline const_iterator end() const { return const_iterator(this, size()); }
    inline const_iterator cend() const { return const_iterator(this, size()); }
    inline const_iterator constEnd() const { return iterator(this, size()); }

    inline T *at(int index) const;
    QQmlListProperty<T> qmlList();
    bool tryBind(QObject *instantiator);
private:
    static int qmlListCount(QQmlListProperty<T> *list);
    static T *qmlListAt(QQmlListProperty<T> *list, int index);
};

template <class T>
T *IList<T>::at(int index) const
{
    return const_cast<T*>(reinterpret_cast<const T*>(objectAt(index)));
}

template <class T>
QQmlListProperty<T> IList<T>::qmlList()
{
    return QQmlListProperty<T>(this, 0, &IList::qmlListCount, &IList::qmlListAt);
}

template <class T>
bool IList<T>::tryBind(QObject *instantiator)
{
    return introspect(instantiator, T::staticMetaObject.className());
}

template <class T>
int IList<T>::qmlListCount(QQmlListProperty<T> *list)
{
    IList *o = static_cast<IList*>(list->object);
    return o->size();
}

template <class T>
T *IList<T>::qmlListAt(QQmlListProperty<T> *list, int index)
{
    IList *o = static_cast<IList*>(list->object);
    return o->at(index);
}


#endif // INSTANTIATORCOLLECTION_H
