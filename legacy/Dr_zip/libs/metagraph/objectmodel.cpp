#include "objectmodel.h"
#include "instance.h"
#include "graphmodel.h"
#include <qjsvalueiterator.h>

ObjectModel::ObjectModel(const QModelIndex &rootIndex, const QMetaObject *type, QObject *parent) :
    SubModel(rootIndex, parent),
    m_dtype(type)
{}

ObjectModel::~ObjectModel()
{}





