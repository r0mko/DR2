#ifndef TRIANGULATOR_H
#define TRIANGULATOR_H
#include "defs.h"
#include <QTransform>


VertexBuffer triangulate(const QPainterPath &path, const QTransform &matrix = QTransform(), qreal lod = 1);
#endif // TRIANGULATOR_H
