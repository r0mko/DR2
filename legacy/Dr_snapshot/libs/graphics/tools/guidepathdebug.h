#ifndef GUIDEPATHDEBUG_H
#define GUIDEPATHDEBUG_H

#include <guide.h>
#include <qcolor.h>

class QPainter;

class GuideDebug
{
public:
    GuideDebug();
    static void paintNodes(Guide p, QPainter *painter, QPen pen, QColor textColor = QColor(Qt::black));
};

#endif // GUIDEPATHDEBUG_H
