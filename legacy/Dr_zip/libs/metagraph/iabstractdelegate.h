#ifndef IABSTRACTDELEGATE
#define IABSTRACTDELEGATE

class IAbstractDelelegate
{
protected:
    virtual void insertChild(int index, QObject *child) = 0;
    virtual void removeChild(int index, QObject *child) = 0;
    virtual void moveChild(int from, int to) = 0;
};
#define IAbstractDelelegate_iid "com.r0mko.GraphModel.IAbstractDelelegate"
Q_DECLARE_INTERFACE(IAbstractDelelegate, IAbstractDelelegate_iid)

#endif // IABSTRACTDELEGATE

