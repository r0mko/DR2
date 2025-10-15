#ifndef HANDLETEMPLATE
#define HANDLETEMPLATE

#define DECLARE_NODE(Class)                                                         \
Q_DECLARE_METATYPE(Class)                                                           \
Q_DECLARE_METATYPE(Class ## Handle)                                                 \
Q_DECLARE_METATYPE(Class ## Factory*)                                               \
template<>                                                                          \
struct HandleType<Class>                                                            \
{                                                                                   \
    static inline int handlerMetaType() { return qMetaTypeId<Class ## Handle>(); }  \
    typedef Class ## Handle type;                                                   \
    typedef Class ## Factory *factoryType;                                          \
    static factoryType factory;                                                     \
};                                                                                  \
int Class :: typeId() const { return SELF_TYPEID; }                                 \

template<class T>
struct HandleType
{
    typedef void type;
    typedef void factoryType;
    static inline int handlerMetaType() { return 0; }
};

#endif // HANDLETEMPLATE

