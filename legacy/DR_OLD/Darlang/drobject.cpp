#include "drobject.h"

DRObject::DRObject(DRObject *parent) :
    QObject(parent)
{
}

void DRObject::sumf(int &callOffset, int &argOffset, void **args)
{

    qreal sum = 0;
    int count = *(int*)*(args+1);
    while(count--)
        sum += *(qreal*)*(args+count);
    *(qreal*)*(args) = sum;
    callOffset += sizeof(FunctionPtr);
    argOffset += *(int*)*(args+1);
}


void DAlgorithm::exec()
{

}

void DAlgorithm::setup(DRObject *obj)
{

}
