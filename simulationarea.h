#ifndef SIMULATIONAREA_H
#define SIMULATIONAREA_H

#include <QWidget>
#include <QTimer>
#include "vehicle.h"

class TrafficLight;
class Car;
class Road;

class SimulationArea : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationArea(QWidget *parent = nullptr);
    ~SimulationArea();

    void startSimulation();
    void stopSimulation();
    void resetSimulation();

    static int totalCars;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void update();

private:
    static const int MAX_CARS = 20;

    Road *northRoad, *southRoad, *eastRoad, *westRoad;
    TrafficLight *northLight, *southLight, *eastLight, *westLight;
    Car* cars[MAX_CARS];
    int carCount;

    QTimer *timer;
    QTimer *lightTimer;
    int currentGreen;
    int lightCounter;

    QPixmap background;
    QPixmap carImages[4];

    void spawnCar();
    void moveCars();
    bool isSpawnPositionValid(int x, int y, Vehicle::Direction dir) const;
    bool isLaneClear(Car* car, int lane) const;
    int getLaneCenter(int lane, Vehicle::Direction dir, int otherCoord) const;
    void attemptLaneChange(Car* car);  // ADD THIS LINE
};

#endif
