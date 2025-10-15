#include "field.h"

Field::Field(QObject *parent) :
    QObject(parent),
    m_hidden( false ),
    m_control( ControlType::TEXT_CONTROL ),
    m_filter( FilterType::NO_FILTER )
{

}

Field::~Field()
{

}

