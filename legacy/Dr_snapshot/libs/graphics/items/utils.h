#ifndef UTILS
#define UTILS

#define SHARED_DATA(Class) inline Class ## Data *m_f() const { return static_cast<Class ## Data*>(d.data()); }

#endif // UTILS

