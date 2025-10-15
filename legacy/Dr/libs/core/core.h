#ifndef CORE_H
#define CORE_H

#include <QQuickItem>

class Core : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(Core)

public:
    Core(QQuickItem *parent = 0);
    ~Core();
};

#endif // CORE_H

