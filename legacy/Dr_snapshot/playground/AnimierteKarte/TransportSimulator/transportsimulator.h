#ifndef TRANSPORTSIMULATOR_H
#define TRANSPORTSIMULATOR_H

#define _USE_MATH_DEFINES

#include <QObject>
#include <QXmlSimpleReader>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QList>
#include <QThread>
#include <QMetaObject>
#include <QMetaMethod>
#include <QDebug>
#include <QTimer>
#include <math.h>
#include <QTime>
#include <QDateTime>
#include <QByteArray>
#include <QDataStream>

#define ER 6378137.0

const double SCF = 0.02; //Speed correction factor
const double stopRadius = 150.0;
const double_t RBF = 1.0; //rounds breke factor

template <class T> inline void SafeDelete(T **pT)
{
    if (pT && *pT)
    {
        delete *pT;
            *pT = nullptr;
    }
}

inline qreal rad2deg(qreal rad)
{
    return (rad * 180) / M_PI;
}

inline qreal deg2rad(qreal deg)
{
    return (deg * M_PI) / 180;
}

inline QPointF sphereMercator(const QPointF &coords, qreal radius)
{
    return QPointF(deg2rad(coords.y()) * radius, radius * log(tan(M_PI_4 + deg2rad(coords.x())/2)));
}

inline QPointF invSphereMercator(const QPointF &coords, qreal radius)
{
    return QPointF(rad2deg(2 * atan(exp( (coords.y() / radius ) )) - M_PI/2), rad2deg(coords.x() / radius));
}


class XmlHandler;

class TransportSimulator : public QObject
{
    Q_OBJECT
public:
    friend class XmlHandler;

    struct Node
    {
        unsigned long long id = 0;
        double lat = 0.0;
        double lon = 0.0;
    };

    struct Tag
    {
        QString k;
        QString v;
    };

    struct Way
    {
        unsigned long long id = 0;
        QList<Tag> tag;
        QList<unsigned long long> nd;
    };

    struct Member
    {
        QString type;
        unsigned long long ref = 0;
        QString role;
    };    

    struct Relation
    {
        unsigned long long id = 0;
        QList<Member> member;
        QList<Tag> tag;
    };

    enum TransportType
    {
        Unknown = 0,
        Tram = 1,
        Trolleybus = 2
    };

    struct TransportWay
    {
        int id = -1;
        TransportType transportType = Unknown;
        unsigned long long wayLength = 0;
        QString ref;
        QString name;
        QString from;
        QString to;
        QList<QPointF> points;
        QList<QPointF> stopPoints;
    };

    struct Vehicle
    {
        int id = -1;
        TransportType type = Unknown;
        double distFromStart = 0.0;
        double speed = 1.0;        
        QPointF coords;
        QPointF coordsInOriginal;
        int wayId = -1;
        QString ref;
        QString name;
        QString from;
        QString to;

        qreal regenEfficiency     = 0.6;
        qreal minRegenSpeed       = 15.0;
        qreal mass                = 20.0;
        qreal maxSpeed            = 70.0;
        qreal maxPower            = 220.0;
        qreal maxRegenPower       = 135.0;
        qreal currentSpeed        = 0.0;
        qreal lastSpeed = -1.0;
        qreal currentAcceleration = 0.0;
        qreal currentPower        = 0.0;

        qreal speedBeforeStop = 0.0;
        qreal speedFactorBeforeStop = 0.0;
        qreal speedUsageFactor = 1.0;

        qint64 baseAccelerationTime = 20000;
        qint64 accelerationTime = 0;
        qint64 accelerationTimeLeft = -1;

        qint64 baseBrakingTime = 15000;
        qint64 brakingTime = 0;
        qint64 brakingTimeLeft = -1;

        qint64 baseParkingTime = 10000;
        qint64 parkingTime = 0;
        qint64 parkingTimeLeft = -1;
        int lastStopPointId = -1;

        ////////////////////////////////

        ////////////////////////////////////

        friend QDataStream& operator <<(QDataStream& stream, const Vehicle& item)
        {
            stream << item.id;
            stream << (short)item.type;
            stream << item.distFromStart;
            stream << item.speed;
            stream << item.maxSpeed;
            stream << item.coords;
            stream << item.coordsInOriginal;
            stream << item.wayId;
            stream << item.ref;
            stream << item.name;
            stream << item.from;
            stream << item.to;

            stream << item.regenEfficiency;
            stream << item.minRegenSpeed;
            stream << item.mass;
            stream << item.maxSpeed;
            stream << item.maxPower;
            stream << item.maxRegenPower;
            stream << item.currentSpeed;
            stream << item.lastSpeed;
            stream << item.currentAcceleration;
            stream << item.currentPower;

            return stream;
        }

        friend QDataStream& operator >>(QDataStream& stream, Vehicle& item)
        {
            stream >> item.id;
            short type;
            stream >> type;
            item.type = (TransportType)type;
            stream >> item.distFromStart;
            stream >> item.speed;
            stream >> item.maxSpeed;
            stream >> item.coords;
            stream >> item.coordsInOriginal;
            stream >> item.wayId;
            stream >> item.ref;
            stream >> item.name;
            stream >> item.from;
            stream >> item.to;

            stream >> item.regenEfficiency;
            stream >> item.minRegenSpeed;
            stream >> item.mass;
            stream >> item.maxSpeed;
            stream >> item.maxPower;
            stream >> item.maxRegenPower;
            stream >> item.currentSpeed;
            stream >> item.lastSpeed;
            stream >> item.currentAcceleration;
            stream >> item.currentPower;
            return stream;
        }
    };

    struct VehicleCollection
    {
        void Serialize(QDataStream& ds)
        {
            ds << vehicles.size();
            for(auto& it : vehicles)
                ds << it;
        }

        void Deserialize(QDataStream& ds)
        {
            vehicles.clear();
            int size = 0;
            ds >> size;
            vehicles.reserve(size);
            for(int i = 0; i < size; i++)
            {
                Vehicle v;
                ds >> v;
                vehicles.append(v);
            }
        }

        QList<Vehicle> vehicles;
    };

    explicit TransportSimulator(bool separateThread = false, QObject *parent = 0);
    ~TransportSimulator();

    Node& getNodeById(unsigned long long id);
    Way& getWayById(unsigned long long id);
    QList<Relation>& getRelation();
    VehicleCollection& getVehicles();
    QList<TransportWay>& getTransportWays();
    Vehicle& getVehicleById(int id);


    static QPointF applyGlobalTransform(QPointF p);
    static QPointF applyInverseGlobalTransform(QPointF p);
    static double calcDist(QPointF a, QPointF b);

    int getTramsPerKm() const;
    void setTramsPerKm(int value);

    int getTramsSpeed() const;
    void setTramsSpeed(int value);

    int getTrolleybusSpeed() const;
    void setTrolleybusSpeed(int value);

    int getTrolleybusPerKm() const;
    void setTrolleybusPerKm(int value);

signals:
    void initialized(QByteArray data);
    void vehiclesCountChanged(int count);
    void initializationError();
    void started();
    void stopped();

    void tickMulti(QByteArray data);
    void tickSingle(QByteArray data);
    void vehicleUpdated(QPointF pos, int id, int type, qreal power, qreal speed, qreal accel, QString routeId);

public slots:
    void initializeFromOverpassXML(QString fileName);
    void start(int timeout = 100);
    void stop();
    void clearContent();
    void destroy();

private slots:
    void initialize(QString fileName);
    void runSimulation(int timeout);
    void stopSimulation();
    void updateScene();
    void clearData();
    void destroyObject();

private:
    QThread* simulatorThread = nullptr;
    Node dummyNode;
    Way dummyWay;
    TransportWay dummyTW;
    Vehicle dummyVehicle;

    QList<Node> nodes;
    QList<Way> ways;
    QList<Relation> relations;    

    QList<TransportWay> transportWays;
    TransportWay& getTransportWayById(int id);
    void madeTransportWays();
    void madeVehicles();

    QTimer* timer = nullptr;
    VehicleCollection vehicleCollection;

    QPointF calcCoordsByWay(double wayOffset, TransportWay& way);

    int getFreeId();
    int globalId = -1;

    int getFreeVehicleId();
    int vehicleId = -1;

    int tramsPerKm = 3;
    int tramsSpeed = 1;

    int trolleybusPerKm = 4;
    int trolleybusSpeed = 2;

    qint64 lastTickTime = 0;
};

#endif // TRANSPORTSIMULATOR_H
