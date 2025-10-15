#ifndef T2C_GLOBAL_H
#define T2C_GLOBAL_H


#if defined(T2C_LIBRARY)
#  define T2CSHARED_EXPORT Q_DECL_EXPORT
#else
#  define T2CSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // T2C_GLOBAL_H
