#include "abstractedge.h"
#include "graphmodel.h"

/*!
    \class AbstractEdge
    \inmodule MetaGraph
    \brief The AbstractEdge предоставляет абстрактную основу для классов типа \l {Edge} и производных
    \ingroup Edges

    Длинное описание класса

*/

/*!
    \fn AbstractEdge::AbstractEdge()
   Конструктор класса

*/


AbstractEdge::AbstractEdge()
{

}

AbstractEdge::~AbstractEdge()
{
    m_startNode = nullptr;
    m_endNode = nullptr;
}
