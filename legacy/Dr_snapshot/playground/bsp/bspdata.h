#ifndef BSPDATA_H
#define BSPDATA_H
#include <QObject>

#include <QPolygonF>
//#include <QVariant>

class BspData
{
    Q_GADGET
//    Q_PROPERTY(QVariant userData READ userData WRITE setUserData)
//    Q_PROPERTY(QPolygonF polygon READ polygon WRITE setPolygon)

//    QVariant m_userData;
//    QPolygonF m_polygon;

public:
    BspData();
//    QVariant userData() const;
//    QPolygonF polygon() const;
//    void setUserData(QVariant userData);
//    void setPolygon(QPolygonF polygon);
};

//class DummyCreatist: public QObject
//{
//    Q_OBJECT
//public:
//    DummyCreatist( QObject *parent = nullptr );
//    Q_INVOKABLE QVariant createBspDataGadget() const;
//};




Q_DECLARE_METATYPE(BspData)
#endif // BSPDATA_H
