#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);    

    renderArea = new RenderArea();
    ui->verticalLayout_2->addWidget(renderArea);
    renderScene = new RenderScene;
    renderArea->setScene(renderScene);
    ui->statusBar->showMessage("no action");

    on_actionLoadMapData_triggered();
}

MainWindow::~MainWindow()
{
    if(ts)    
        ts->destroy();

    delete renderScene;
    delete renderArea;
    delete ui;
}

void MainWindow::tsInitOk(QByteArray ba)
{
    if(!ts)
        return;

    ui->statusBar->showMessage("MapData : loaded");
    updateMap();
    connect(ts, &TransportSimulator::tickMulti, this, &MainWindow::tick);
    connect(ts, &TransportSimulator::tickSingle, this, &MainWindow::tickSingle);
    ts->start(50);
}

void MainWindow::tsInitFailed()
{
    ui->statusBar->showMessage("MapData : parsing error!");
}

void MainWindow::tick(QByteArray ba)
{
    QDataStream ds(&ba, QIODevice::ReadOnly);
    TransportSimulator::VehicleCollection vc;
    vc.Deserialize(ds);

    for(auto& it : vc.vehicles)
    {
        for(auto& jt : graphicVehicles)
        {
            if(jt.id == it.id)
            {
                jt.visualElement->setPos(it.coords);
                jt.textElement->setPos(it.coords.x() - jt.visualElement->rect().width() / 2, it.coords.y() - jt.visualElement->rect().height() / 2);
                break;
            }
        }
    }
    renderScene->update();
}

void MainWindow::tickSingle(QByteArray ba)
{
    QDataStream ds(&ba, QIODevice::ReadOnly);
    TransportSimulator::Vehicle vehicle;
    ds >> vehicle;
    for(auto& it : graphicVehicles)
    {
        if(it.id == vehicle.id)
        {
            it.visualElement->setPos(vehicle.coords);
            it.textElement->setPos(vehicle.coords.x() - it.visualElement->rect().width() / 2, vehicle.coords.y() - it.visualElement->rect().height() / 2);
            break;
        }
    }
}

void MainWindow::on_actionLoadMapData_triggered()
{
    if(ts)
        SafeDelete(&ts);

    ts = new TransportSimulator;
    connect(ts, &TransportSimulator::initialized, this, &MainWindow::tsInitOk);
    connect(ts, &TransportSimulator::initializationError, this, &MainWindow::tsInitFailed);
    ui->statusBar->showMessage("MapData : parsing...");  
    ts->initializeFromOverpassXML("C:/1/data");
}

void MainWindow::updateMap()
{
    if(!ts)
        return;

    QList<TransportSimulator::Relation>& relations = ts->getRelation();
    for(auto& it : relations)
    {
        for(auto& jt : it.member)
        {
            if(jt.type == "node")
            {
                if(jt.role == "stop" || jt.role == "stop_entry_only" || jt.role == "stop_exit_only")
                {
                    TransportSimulator::Node& node = ts->getNodeById(jt.ref);
                    QPointF center = TransportSimulator::applyGlobalTransform(QPointF(node.lat,node.lon));
                    qreal bw = 50.0;
                    QGraphicsEllipseItem* el = renderScene->addEllipse(center.x()-(bw/2.0),center.y()-(bw/2.0),bw,bw,QPen(QBrush(Qt::red), 3));
                    el->setZValue(1.0);

                    qreal bw2 = 150;
                    renderScene->addEllipse(center.x()-(bw2/2.0),center.y()-(bw2/2.0),bw2,bw2,QPen(QBrush(Qt::red), 6));
                }
            }
        }
    }

    QList<TransportSimulator::TransportWay>& ways = ts->getTransportWays();
    int counter = 0;
    for(auto& it : ways)
    {
        QPolygonF p;
        p.reserve(it.points.size());
        for(auto& jt : it.points)
            p.append(jt);
        QPainterPath pp;
        pp.addPolygon(p);
        if(counter == 0)
            renderScene->addPath(pp, QPen(QBrush(Qt::blue), 3));
        else if(counter == 1)
            renderScene->addPath(pp, QPen(QBrush(Qt::red), 3));
        else if(counter == 2)
            renderScene->addPath(pp, QPen(QBrush(Qt::yellow), 3));
        else if(counter == 3)
            renderScene->addPath(pp, QPen(QBrush(Qt::green), 3));
        else if(counter == 4)
            renderScene->addPath(pp, QPen(QBrush(Qt::magenta), 3));
        else if(counter == 5)
            renderScene->addPath(pp, QPen(QBrush(Qt::darkBlue), 3));
        else if(counter == 6)
            renderScene->addPath(pp, QPen(QBrush(Qt::darkCyan), 3));
        else if(counter == 7)
            renderScene->addPath(pp, QPen(QBrush(Qt::darkMagenta), 3));
        else
            renderScene->addPath(pp, QPen(QBrush(Qt::black), 3));
        counter++;
    }

    TransportSimulator::VehicleCollection& vc = ts->getVehicles();
    for(auto& it : vc.vehicles)
    {
        int bw = 80.0;
        QBrush vb;
        if(it.type == TransportSimulator::Tram)
            vb = QBrush(QColor(0,150,150));
        else if(it.type == TransportSimulator::Trolleybus)
            vb = QBrush(Qt::yellow);
        else
            continue;
        QGraphicsEllipseItem* el = renderScene->addEllipse(-bw/2.0,-bw/2.0,bw,bw,QPen(QBrush(Qt::black), 2), vb);
        el->setPos(it.coords.x(), it.coords.y());

        QGraphicsTextItem* ti = renderScene->addText(it.ref, QFont("Arial", 40));
        ti->setPos(it.coords.x() - el->rect().width() / 2, it.coords.y() - el->rect().height() / 2);

        GraphicVehicle gv;
        gv.id = it.id;
        gv.visualElement = el;
        gv.textElement = ti;
        graphicVehicles.append(gv);
    }

    renderArea->fitInView(renderScene->sceneRect());
}

void MainWindow::clearMap()
{
    if(renderScene)
    {
        for(auto& it : renderScene->items())
        {
            if(it)
                delete it;
        }
    }
    renderScene->clear();
    graphicVehicles.clear();

    if(ts)
        ts->clearContent();
}

void MainWindow::on_actionClearMap_triggered()
{
    clearMap();
}

void MainWindow::on_actionINC_SPEED_triggered()
{
    if(ts)
        ts->setTramsSpeed(ts->getTramsSpeed()+1);
}

void MainWindow::on_actionDEC_SPEED_triggered()
{
    if(ts)
        ts->setTramsSpeed(ts->getTramsSpeed()-1);
}

void MainWindow::on_actionINC_TROLL_SPEED_triggered()
{
    if(ts)
        ts->setTrolleybusSpeed(ts->getTrolleybusSpeed()+1);
}

void MainWindow::on_actionDEC_TROLL_SPEED_triggered()
{
    if(ts)
        ts->setTrolleybusSpeed(ts->getTrolleybusSpeed()-1);
}

void MainWindow::on_actionTEST_triggered()
{
    if(!timer)
    {
        timer = new QTimer;
        connect(timer, &QTimer::timeout, this, &MainWindow::on_actionTEST_triggered);
        timer->start(100);
    }

    QList<TransportSimulator::TransportWay>& www = ts->getTransportWays();
    for(auto& it : www)
    {
        if(it.ref == "18")
        {
            qreal radius = 50.0;
            QPen pen = QPen(QBrush(Qt::blue),1);
            if(counter > it.points.size() - 2)
                    pen = QPen(QBrush(Qt::red), 5);
            else
            {
                renderScene->addLine(it.points[counter].x(), it.points[counter].y(), it.points[counter+1].x(),it.points[counter+1].y());
            }
            renderScene->addEllipse(it.points[counter].x()-radius, it.points[counter].y()-radius, 2*radius, 2*radius, pen);

            counter++;
            if(counter > it.points.size() - 1)
                    counter--;
            break;
        }
    }
}
