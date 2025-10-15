#include "transportsimulator.h"
#include "xmlhandler.h"

TransportSimulator::TransportSimulator(bool separateThread, QObject *parent) : QObject(parent)
{    
    if(separateThread)
    {
        simulatorThread = new QThread;
        moveToThread(simulatorThread);
        connect(this, &TransportSimulator::destroyed, simulatorThread, &QThread::quit);
        connect(simulatorThread, &QThread::finished, simulatorThread, &QThread::deleteLater);
        simulatorThread->start();
    }
}

TransportSimulator::~TransportSimulator()
{
    clearData();
}

TransportSimulator::Node &TransportSimulator::getNodeById(unsigned long long id)
{
    for(auto& it : nodes)
    {
        if(it.id == id)
            return it;
    }
    return dummyNode;
}

TransportSimulator::Way &TransportSimulator::getWayById(unsigned long long id)
{
    for(auto& it : ways)
    {
        if(it.id == id)
            return it;
    }
    return dummyWay;
}

QList<TransportSimulator::Relation> &TransportSimulator::getRelation()
{
    return relations;
}

TransportSimulator::VehicleCollection& TransportSimulator::getVehicles()
{
    return vehicleCollection;
}

QList<TransportSimulator::TransportWay> &TransportSimulator::getTransportWays()
{
    return transportWays;
}

TransportSimulator::Vehicle &TransportSimulator::getVehicleById(int id)
{
    for(auto& it : vehicleCollection.vehicles)
    {
        if(it.id == id)
            return it;
    }
    return dummyVehicle;
}

void TransportSimulator::initializeFromOverpassXML(QString fileName)
{
    bool ok = QMetaObject::invokeMethod(this, "initialize", Qt::QueuedConnection, Q_ARG(QString, fileName));
    if(!ok)
    {
        qDebug() << "TransportSimulator::initializeFromOverpassXML() - invokeMethod failed!";
        emit initializationError();
    }
}

void TransportSimulator::start(int timeout)
{
    bool ok = QMetaObject::invokeMethod(this, "runSimulation", Qt::QueuedConnection, Q_ARG(int, timeout));
    if(!ok)
    {
        qDebug() << "TransportSimulator::start() - invokeMethod failed!";
    }
}

void TransportSimulator::stop()
{
    bool ok = QMetaObject::invokeMethod(this, "stopSimulation", Qt::QueuedConnection);
    if(!ok)
    {
        qDebug() << "TransportSimulator::stop() - invokeMethod failed!";
    }
}

void TransportSimulator::clearContent()
{
    bool ok = QMetaObject::invokeMethod(this, "clearData", Qt::QueuedConnection);
    if(!ok)
    {
        qDebug() << "TransportSimulator::clearData() - invokeMethod failed!";
    }
}

void TransportSimulator::destroy()
{
    if(timer)
    {
        connect(timer, &QTimer::destroyed, this, &TransportSimulator::destroyObject);
        timer->deleteLater();
    }
    else
    {
        bool ok = QMetaObject::invokeMethod(this, "destroyObject", Qt::QueuedConnection);
        if(!ok)
        {
            qDebug() << "TransportSimulator::destroyObject() - invokeMethod failed!";
        }
    }
}

void TransportSimulator::initialize(QString fileName)
{
    if (fileName.isEmpty())
    {
        qDebug() << "TransportSimulator::initialize() - fileName isEmpty!";
        emit initializationError();
        return;
    }

    clearData();

    XmlHandler handler(this);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "TransportSimulator::initialize() - can't open: " << fileName;
        emit initializationError();
        return;
    }

    QXmlInputSource xmlInputSource(&file);
    if(reader.parse(xmlInputSource))
    {
        madeTransportWays();
        madeVehicles();
        emit vehiclesCountChanged(vehicleCollection.vehicles.size());
        QByteArray ba;
        QDataStream ds(&ba, QIODevice::WriteOnly);
        vehicleCollection.Serialize(ds);
        emit initialized(ba);
    }
    else
    {
        qDebug() << "TransportSimulator::initialize() - QXmlSimpleReader - parse error : " << reader.errorHandler()->errorString();
        clearData();
        emit initializationError();
    }
}

void TransportSimulator::runSimulation(int timeout)
{
    if(timer)
        timer->stop();
    else
    {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TransportSimulator::updateScene);
    }

    timer->start(timeout);
    emit started();
}

void TransportSimulator::stopSimulation()
{
    if(timer)
    {
        timer->stop();
        emit stopped();
    }
}

void TransportSimulator::updateScene()
{
    qint64 currentTime =  QDateTime::currentMSecsSinceEpoch();
    if(lastTickTime == 0)
    {
        lastTickTime = currentTime;
        return;
    }
    qint64 elapsedTime = currentTime - lastTickTime;
    //qint64 elapsedTime = 60;

    for(auto& it : vehicleCollection.vehicles)
    {
        TransportWay& way = getTransportWayById(it.wayId);

//        if(it.distFromStart + it.speed > way.wayLength)
//            it.speed *= -1;
//        if(it.distFromStart + it.speed < 0)
//            it.speed *= -1;

        qreal dist = - 1.0;
        int stopCounter = 0;
        for(auto& jt : way.stopPoints)
        {
            qreal tmpDist = calcDist(it.coords, jt);
            if(tmpDist <= stopRadius && tmpDist > 0.0)
            {
                dist = tmpDist;
                break;
            }
            stopCounter++;
        }        

        if(dist > 0.0)
        {
            //148             3             4            5

            if(it.accelerationTimeLeft < 0 &&
                    it.parkingTimeLeft < 0 &&
                    it.brakingTimeLeft < 0 &&
                    it.lastStopPointId != stopCounter)
            {
                it.speedBeforeStop = it.speed;
                it.speedFactorBeforeStop = it.speedUsageFactor;
                it.lastStopPointId = stopCounter;
                it.brakingTimeLeft = 0;

                it.brakingTime = it.baseBrakingTime * (rand()%100 + 50) / (qreal)100;
                it.parkingTime = it.baseParkingTime * (rand()%100 + 50) / (qreal)100;
                it.accelerationTime = it.baseAccelerationTime * (rand()%100 + 50) / (qreal)100;

            }
            if(it.brakingTimeLeft >= 0)
            {
                it.brakingTimeLeft += elapsedTime;
                qreal btf = (it.brakingTimeLeft / (qreal)it.brakingTime);
                if(btf > 1.0)
                {
                    it.brakingTimeLeft = -1;
                    it.parkingTimeLeft = 0;
                    it.speedUsageFactor = 0.0;                    
                }
                else
                {
                    it.speedUsageFactor = it.speedFactorBeforeStop - (it.speedFactorBeforeStop*sin(M_PI_2*btf));
                    if(it.speedUsageFactor < 0.0)
                        it.speedUsageFactor = 0.0;

//                    if(it.ref == "19" && it.id == 148)
//                        qDebug() << btf << " : " << it.speedUsageFactor;
                }
            }
            if(it.parkingTimeLeft >= 0)
            {                
                it.parkingTimeLeft += elapsedTime;
                if(it.parkingTimeLeft > it.parkingTime)
                {                    
                    it.parkingTimeLeft = -1;
                    it.accelerationTimeLeft = 0;
                }

            }
            if(it.accelerationTimeLeft >= 0)
            {
                it.accelerationTimeLeft += elapsedTime;
                qreal atf = (it.accelerationTimeLeft / (qreal)it.accelerationTime);
                if(atf > 1.0)
                {
                    it.accelerationTimeLeft = -1;
                }
                else
                {
                    it.speedUsageFactor = it.speedFactorBeforeStop*sin(M_PI_2*atf);

//                    if(it.ref == "19" && it.id == 148)
//                        qDebug() << it.speedUsageFactor;

                    if(it.speedUsageFactor > it.speedFactorBeforeStop)
                        it.speedUsageFactor = it.speedFactorBeforeStop;
                }
            }
        }
        else
        {
            ////////////////////////

            if(way.points.size() > 2)
            {
                double traveledDist = 0.0;
                for(int i = 0; i < way.points.size() - 1; i++)
                {
                    double localDist = calcDist(way.points[i+1], way.points[i]);
                    traveledDist += localDist;
                    if(it.distFromStart > traveledDist)
                        continue;

                    if(i + 2 < way.points.size())
                    {
                        QLineF thisLine(way.points[i],way.points[i+1]);
                        QLineF nextLine(way.points[i+1],way.points[i+2]);
                        qreal ang = thisLine.angleTo(nextLine);

                        qreal corrector = -1.0;
                        if(ang > 0 && ang < 360)
                        {
                            if(ang < 90.0)
                                corrector = 1.0 - ang / 90.0;
                            else if(ang > 270)
                                corrector = ang / 360.0;
                        }

                        if(corrector > 0)
                            it.speedUsageFactor = 1.0 * corrector * RBF;
                        else
                            it.speedUsageFactor = 1.0;

    //                    if(it.ref == "19" && it.id == 148)
    //                        qDebug() << "ANG: " << ang << " | COR: " << corrector << " | SF:" << it.speedUsageFactor;
                    }

                    break;
                }
            }

            /////////////////////
        }

        if(it.distFromStart + it.speed < way.wayLength)
        {
            //qreal offset = SCF * it.speed * it.speedUsageFactor * elapsedTime;
//            if(it.ref == "19" && it.id == 148)
//                qDebug() << "-----OFFSET: " << offset;

            it.distFromStart += SCF * it.speed * it.speedUsageFactor * elapsedTime;
            it.coords = calcCoordsByWay(it.distFromStart, way);
        }
        else
            it.distFromStart = 1.0;

        it.coordsInOriginal = applyInverseGlobalTransform(it.coords);
        bool canCalcAcc = true;
        if(it.lastSpeed < 0)
            canCalcAcc = false;
        it.lastSpeed = it.currentSpeed;
        it.currentSpeed = it.maxSpeed * it.speedUsageFactor;
        if(canCalcAcc)
            it.currentAcceleration = ((it.currentSpeed - it.lastSpeed) / 3.6)
                    / (elapsedTime * 1000.0);

        it.currentPower = it.maxPower*(1.0 - it.speedUsageFactor);
        if(it.maxPower <= 0)
            it.currentPower += it.maxPower*((rand()%3)/100.0);
        if(it.currentPower > it.maxPower)
            it.currentPower = it.maxPower;
//        if(it.ref == "19" && it.id == 148)
//            qDebug() << it.currentPower;

        QByteArray ba;
        QDataStream ds(&ba, QIODevice::WriteOnly);
        ds << it;
        emit tickSingle(ba);
        QPointF merc = it.coords;
        merc.ry() *= -1;
        emit vehicleUpdated(merc, it.id, (int)it.type, it.currentPower, it.currentSpeed, it.currentAcceleration, it.ref);
    }

    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    vehicleCollection.Serialize(ds);
    emit tickMulti(ba);

    lastTickTime = currentTime;
}

void TransportSimulator::clearData()
{
    nodes.clear();
    ways.clear();
    relations.clear();
    transportWays.clear();
    globalId = -1;
    vehicleId = -1;
    if(timer)
    {
        timer->stop();
        delete timer;
    }
}

void TransportSimulator::destroyObject()
{
    timer = nullptr;
    this->deleteLater();
}

QPointF TransportSimulator::applyGlobalTransform(QPointF p)
{
    p = sphereMercator(p, ER);
    p.setY((-1.0)*p.y());
    return p;
}

QPointF TransportSimulator::applyInverseGlobalTransform(QPointF p)
{
    p.setY((-1.0)*p.y());
    p = invSphereMercator(p, ER);
    return p;
}

double TransportSimulator::calcDist(QPointF a, QPointF b)
{
    return sqrt(pow(b.x() - a.x(), 2.0) + pow(b.y() - a.y(), 2.0));
}

void TransportSimulator::madeTransportWays()
{
    for(auto& it : relations)
    {
        TransportWay tWay;        
        for(auto& tag : it.tag)
        {
            if(tag.k == "route")
            {
                if(tag.v == "tram")
                    tWay.transportType = Tram;
                else if(tag.v == "trolleybus")
                    tWay.transportType = Trolleybus;
                break;
            }
            else if(tag.k == "ref")
                tWay.ref = tag.v;
            else if(tag.k == "name")
                tWay.name = tag.v;
            else if(tag.k == "from")
                tWay.from = tag.v;
            else if(tag.k == "to")
                tWay.to = tag.v;
        }
        if(tWay.transportType == Unknown)
            continue;
        for(auto& jt : it.member)
        {
            if(jt.type == "way")
            {
                Way& way = getWayById(jt.ref);
                if(way.nd.size() > 1)
                {
                    for(auto& nd : way.nd)
                    {
                        Node& node = getNodeById(nd);
                        tWay.points.append(applyGlobalTransform(QPointF(node.lat, node.lon)));
                    }

                }
            }
            else if(jt.type == "node")
            {
                if(jt.role == "stop" || jt.role == "stop_entry_only" || jt.role == "stop_exit_only")
                {
                    for(auto& p : nodes)
                    {
                        if(jt.ref == p.id)
                        {
                            tWay.stopPoints.append(applyGlobalTransform(QPointF(p.lat, p.lon)));
                            break;
                        }
                    }
                }
            }
        }
        if(tWay.points.size() > 1)
        {
            tWay.id = getFreeId();
            double wayLength = 0.0;
            for(int i = 0; i < tWay.points.size() - 1; i++)
                wayLength += calcDist(tWay.points[i + 1], tWay.points[i]);
            tWay.wayLength = wayLength;

            transportWays.append(tWay);
        }        
    }
}

void TransportSimulator::madeVehicles()
{
    if(transportWays.empty())
        return;

    for(auto& it : transportWays)
    {
        if(it.transportType != Tram && it.transportType != Trolleybus)
            continue;
        if(it.points.size() < 2)
            continue;

        double totalWayDist = 0.0;
        for(int i = 0; i < it.points.size() - 1; i++)
            totalWayDist += calcDist(it.points[i + 1], it.points[i]);

        double distBetween = 0.0;

        if(it.transportType == Tram)
        {
            distBetween = totalWayDist / tramsPerKm;
            for(int i = 0; i < tramsPerKm; i++)
            {
                Vehicle vehicle;
                vehicle.id = getFreeVehicleId();
                vehicle.type = it.transportType;
                vehicle.distFromStart = distBetween * i;
                vehicle.wayId = it.id;
                vehicle.coords = calcCoordsByWay(vehicle.distFromStart, it);
                vehicle.coordsInOriginal = applyInverseGlobalTransform(vehicle.coords);
                vehicle.ref = it.ref;
                vehicle.from = it.from;
                vehicle.to = it.to;
                vehicle.name = it.name;
                vehicle.speed = tramsSpeed;
                vehicleCollection.vehicles.append(vehicle);
            }
        }
        else if(it.transportType == Trolleybus)
        {
            distBetween = totalWayDist / trolleybusPerKm;
            for(int i = 0; i < tramsPerKm; i++)
            {
                Vehicle vehicle;
                vehicle.id = getFreeVehicleId();
                vehicle.type = it.transportType;
                vehicle.distFromStart = distBetween * i;
                vehicle.wayId = it.id;
                vehicle.coords = calcCoordsByWay(vehicle.distFromStart, it);
                vehicle.coordsInOriginal = applyInverseGlobalTransform(vehicle.coords);
                vehicle.ref = it.ref;
                vehicle.from = it.from;
                vehicle.to = it.to;
                vehicle.name = it.name;
                vehicle.speed = trolleybusSpeed;
                vehicleCollection.vehicles.append(vehicle);
            }
        }
    }
}

TransportSimulator::TransportWay& TransportSimulator::getTransportWayById(int id)
{
    for(auto& it : transportWays)
    {
        if(it.id == id)
            return it;
    }
    return dummyTW;
}

QPointF TransportSimulator::calcCoordsByWay(double wayOffset, TransportWay& way)
{
    if(way.points.size() < 2)
        return QPointF();

    double traveledDist = 0.0;
    for(int i = 0; i < way.points.size() - 1; i++)
    {
        double localDist = calcDist(way.points[i+1], way.points[i]);
        traveledDist += localDist;
        if(wayOffset > traveledDist)
            continue;

        double localOffset = localDist - (traveledDist - wayOffset);
        QPainterPath pp;
        QPolygonF p;
        p << way.points[i] << way.points[i+1];
        pp.addPolygon(p);

        double kof = localOffset / localDist;

        return pp.pointAtPercent(kof);
    }
    return QPointF();
}

int TransportSimulator::getFreeId()
{
    return ++globalId;
}

int TransportSimulator::getFreeVehicleId()
{
    return ++vehicleId;
}
int TransportSimulator::getTrolleybusPerKm() const
{
    return trolleybusPerKm;
}

void TransportSimulator::setTrolleybusPerKm(int value)
{
    trolleybusPerKm = value;
}

int TransportSimulator::getTrolleybusSpeed() const
{
    return trolleybusSpeed;
}

void TransportSimulator::setTrolleybusSpeed(int value)
{
    trolleybusSpeed = value;
}

int TransportSimulator::getTramsSpeed() const
{
    return tramsSpeed;
}

void TransportSimulator::setTramsSpeed(int value)
{
    if(value <= 0)
        return;
    tramsSpeed = value;
}

int TransportSimulator::getTramsPerKm() const
{
    return tramsPerKm;
}

void TransportSimulator::setTramsPerKm(int value)
{
    if(value <= 0)
        return;
    tramsPerKm = value;
}


