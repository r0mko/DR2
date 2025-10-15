#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPolygonF>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QByteArray>
#include <QDataStream>
#include "transportsimulator.h"
#include "renderarea.h"
#include "renderscene.h"

struct GraphicVehicle
{
    int id;
    QGraphicsEllipseItem* visualElement = nullptr;
    QGraphicsTextItem* textElement = nullptr;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void tsInitOk(QByteArray ba);
    void tsInitFailed();
    void tick(QByteArray ba);
    void tickSingle(QByteArray ba);

private slots:
    void on_actionLoadMapData_triggered();
    void on_actionClearMap_triggered();

    void on_actionINC_SPEED_triggered();

    void on_actionDEC_SPEED_triggered();

    void on_actionINC_TROLL_SPEED_triggered();

    void on_actionDEC_TROLL_SPEED_triggered();

    void on_actionTEST_triggered();

private:
    Ui::MainWindow *ui;
    TransportSimulator* ts = nullptr;
    RenderArea* renderArea = nullptr;
    RenderScene* renderScene = nullptr;

    void updateMap();
    void clearMap();

    QList<GraphicVehicle> graphicVehicles;

    QList<QGraphicsItem*> testItems;
    QTimer* timer = nullptr;
    int counter = 0;
};

#endif // MAINWINDOW_H
