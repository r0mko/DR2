#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractTableModel>

class DataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DataModel(QObject *parent = 0);

signals:

public slots:

};

#endif // DATAMODEL_H
