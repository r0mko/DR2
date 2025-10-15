#ifndef DUMMY_H
#define DUMMY_H
#include "dynamicobjectextension.h"

class Dummy : public DynamicObjectExtension<QObject>
{
public:
    Dummy(QObject *parent = 0) :
        DynamicObjectExtension(parent)
      //    m_metaObject(metaObject)
    {}
    ~Dummy() {}
};

#endif // DUMMY_H
