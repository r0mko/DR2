#ifndef GEOMETRYDATASOURCE_H
#define GEOMETRYDATASOURCE_H

#include <QObject>

class GeometryDataSource : public QObject
{
    Q_OBJECT
public:
    explicit GeometryDataSource(QObject *parent = 0);
    ~GeometryDataSource();

signals:

public slots:
};

#endif // GEOMETRYDATASOURCE_H
