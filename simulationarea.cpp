#include "simulationarea.h"
#include "trafficlight.h"
#include "car.h"
#include "road.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QDebug>
#include <algorithm>
#include <climits>
#include <cmath>

int SimulationArea::totalCars = 0;

SimulationArea::SimulationArea(QWidget *parent)
    : QWidget(parent), carCount(0), currentGreen(0), lightCounter(0)
{
    setFixedSize(800, 500);
    // Load images
    background.load(":/images/ImageTANCE/backgroundimage");

    // Load and scale - make ALL cars the same visual size
    // North/South cars: vertical orientation (taller)
    carImages[0].load(":/images/ImageTANCE/carnorth.png");
    carImages[0] = carImages[0].scaled(50, 80, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 50x80

    carImages[1].load(":/images/ImageTANCE/carsouth.png");
    carImages[1] = carImages[1].scaled(50, 80, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 50x80

    // East/West cars: horizontal orientation (wider)
    carImages[2].load(":/images/ImageTANCE/careast.png");
    carImages[2] = carImages[2].scaled(80, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 80x50

    carImages[3].load(":/images/ImageTANCE/carwest.png");
    carImages[3] = carImages[3].scaled(80, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);  // 80x50
    // Load images
 /*   background.load(":/images/ImageTANCE/backgroundimage");
    carImages[0].load(":/images/ImageTANCE/carnorth.png");
    carImages[1].load(":/images/ImageTANCE/carsouth.png");
    carImages[2].load(":/images/ImageTANCE/careast.png");
    carImages[3].load(":/images/ImageTANCE/carwest.png");
*/
    // Create roads - FIXED ALIGNMENT
    int centerX = 400, centerY = 250;
    int roadWidth = 240;  // 8 lanes * 30px = 240px (4 lanes each direction)
    int roadHalfWidth = roadWidth / 2;  // 120px

    northRoad = new Road("North", centerX - roadHalfWidth, 0, roadWidth, centerY, Road::VERTICAL);
    southRoad = new Road("South", centerX - roadHalfWidth, centerY, roadWidth, 250, Road::VERTICAL);
    eastRoad = new Road("East", centerX, centerY - roadHalfWidth, 400, roadWidth, Road::HORIZONTAL);
    westRoad = new Road("West", 0, centerY - roadHalfWidth, 400, roadWidth, Road::HORIZONTAL);

    // Create traffic lights at correct positions
    northLight = new TrafficLight(centerX - 30, centerY - 110);
    southLight = new TrafficLight(centerX + 10, centerY + 110);
    eastLight = new TrafficLight(centerX + 110, centerY - 25);
    westLight = new TrafficLight(centerX - 110, centerY + 5);

    // Set initial states - North-South get green first
    northLight->setState(TrafficLight::GREEN);
    southLight->setState(TrafficLight::GREEN);
    eastLight->setState(TrafficLight::RED);
    westLight->setState(TrafficLight::RED);

    // Initialize cars array
    for (int i = 0; i < MAX_CARS; i++) {
        cars[i] = nullptr;
    }

    // Create initial cars
    for (int i = 0; i < 4; i++) {
        spawnCar();
    }

    // Setup timers
    timer = new QTimer(this);
    lightTimer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &SimulationArea::update);
    connect(lightTimer, &QTimer::timeout, [this]() {
        lightCounter++;

        // Traffic light cycle: 6 seconds per phase
        if (lightCounter < 120) { // 6 seconds = 120 * 50ms
            northLight->setState(TrafficLight::GREEN);
            southLight->setState(TrafficLight::GREEN);
            eastLight->setState(TrafficLight::RED);
            westLight->setState(TrafficLight::RED);
        }
        else if (lightCounter < 140) { // 1 second yellow = 20 * 50ms
            northLight->setState(TrafficLight::YELLOW);
            southLight->setState(TrafficLight::YELLOW);
        }
        else if (lightCounter < 160) { // 1 second all red = 20 * 50ms
            northLight->setState(TrafficLight::RED);
            southLight->setState(TrafficLight::RED);
            eastLight->setState(TrafficLight::RED);
            westLight->setState(TrafficLight::RED);
        }
        else if (lightCounter < 280) { // 6 seconds
            eastLight->setState(TrafficLight::GREEN);
            westLight->setState(TrafficLight::GREEN);
            northLight->setState(TrafficLight::RED);
            southLight->setState(TrafficLight::RED);
        }
        else if (lightCounter < 300) { // 1 second yellow
            eastLight->setState(TrafficLight::YELLOW);
            westLight->setState(TrafficLight::YELLOW);
        }
        else if (lightCounter < 320) { // 1 second all red
            northLight->setState(TrafficLight::RED);
            southLight->setState(TrafficLight::RED);
            eastLight->setState(TrafficLight::RED);
            westLight->setState(TrafficLight::RED);
        }
        else {
            lightCounter = 0;
        }
    });

    lightTimer->start(50);
    timer->start(30);
}

SimulationArea::~SimulationArea()
{
    delete northRoad;
    delete southRoad;
    delete eastRoad;
    delete westRoad;
    delete northLight;
    delete southLight;
    delete eastLight;
    delete westLight;

    for (int i = 0; i < carCount; i++) {
        delete cars[i];
    }
}

void SimulationArea::startSimulation()
{
    if (!timer->isActive()) {
        timer->start(30);
    }
    if (!lightTimer->isActive()) {
        lightTimer->start(50);
    }
}

void SimulationArea::stopSimulation()
{
    if (timer->isActive()) {
        timer->stop();
    }
    if (lightTimer->isActive()) {
        lightTimer->stop();
    }
}

void SimulationArea::resetSimulation()
{
    stopSimulation();

    for (int i = 0; i < carCount; i++) {
        delete cars[i];
        cars[i] = nullptr;
    }
    carCount = 0;
    totalCars = 0;

    currentGreen = 0;
    lightCounter = 0;
    northLight->setState(TrafficLight::GREEN);
    southLight->setState(TrafficLight::GREEN);
    eastLight->setState(TrafficLight::RED);
    westLight->setState(TrafficLight::RED);

    for (int i = 0; i < 4; i++) {
        spawnCar();
    }

    update();
}

void SimulationArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background
    painter.drawPixmap(0, 0, width(), height(), background);

    // Draw roads - FIXED: Proper width and alignment
    painter.setBrush(QColor(50, 50, 50));
    painter.setPen(Qt::NoPen);

    int centerX = 400, centerY = 250;
    int roadWidth = 240;  // 240px wide roads
    int roadHalfWidth = roadWidth / 2;  // 120px

    // Road positions: center ± 120px
    int roadLeft = centerX - roadHalfWidth;    // 400 - 120 = 280
    int roadRight = centerX + roadHalfWidth;   // 400 + 120 = 520
    int roadTop = centerY - roadHalfWidth;     // 250 - 120 = 130
    int roadBottom = centerY + roadHalfWidth;  // 250 + 120 = 370

    // North road (from top to center)
    painter.drawRect(roadLeft, 0, roadWidth, centerY);
    // South road (from center to bottom)
    painter.drawRect(roadLeft, centerY, roadWidth, 250);
    // East road (from center to right) - FIXED ALIGNMENT
    painter.drawRect(centerX, roadTop, 400, roadWidth);
    // West road (from left to center) - FIXED ALIGNMENT
    painter.drawRect(0, roadTop, 400, roadWidth);

    // Draw intersection
    painter.setBrush(QColor(70, 70, 70));
    painter.drawRect(300, 150, 200, 200);

    // Draw lane markings - FIXED: Proper lane positions for all directions
    painter.setBrush(Qt::white);

    // Lane positions (3 lanes each direction, 30px per lane):
    // Southbound lanes (coming from North): 295, 325, 355
    // Northbound lanes (coming from South): 445, 475, 505
    // Eastbound lanes (coming from West): 145, 175, 205
    // Westbound lanes (coming from East): 295, 325, 355

    // Southbound lane markers (from North, going South)
    painter.drawRect(295 - 5, 20, 10, 40);   // Left lane (295)
    painter.drawRect(325 - 5, 20, 10, 40);   // Center lane (325)
    painter.drawRect(355 - 5, 20, 10, 40);   // Right lane (355)

    // Northbound lane markers (from South, going North)
    painter.drawRect(445 - 5, 460, 10, 40);  // Left lane (445)
    painter.drawRect(475 - 5, 460, 10, 40);  // Center lane (475)
    painter.drawRect(505 - 5, 460, 10, 40);  // Right lane (505)

    // Eastbound lane markers (from West, going East) - FIXED ALIGNMENT
    painter.drawRect(20, 145 - 5, 40, 10);   // Top lane (145)
    painter.drawRect(20, 175 - 5, 40, 10);   // Center lane (175)
    painter.drawRect(20, 205 - 5, 40, 10);   // Bottom lane (205)

    // Westbound lane markers (from East, going West) - FIXED ALIGNMENT
    painter.drawRect(740, 295 - 5, 40, 10);  // Top lane (295)
    painter.drawRect(740, 325 - 5, 40, 10);  // Center lane (325)
    painter.drawRect(740, 355 - 5, 40, 10);  // Bottom lane (355)

    // Draw center divider (yellow line) - THINNER
    painter.setBrush(Qt::yellow);
    // Vertical divider
    painter.drawRect(centerX - 3, 0, 6, centerY);
    painter.drawRect(centerX - 3, centerY, 6, 250);
    // Horizontal divider
    painter.drawRect(centerX, centerY - 3, 400, 6);
    painter.drawRect(0, centerY - 3, 400, 6);

    // Draw stop lines - FIXED: Match road width
    painter.setBrush(Qt::white);
    // Northbound stop line
    painter.drawRect(roadLeft, 350, roadWidth, 5);
    // Southbound stop line
    painter.drawRect(roadLeft, 145, roadWidth, 5);
    // Eastbound stop line
    painter.drawRect(295, roadTop, 5, roadWidth);
    // Westbound stop line
    painter.drawRect(500, roadTop, 5, roadWidth);

    // Draw traffic lights
    northLight->draw(painter);
    southLight->draw(painter);
    eastLight->draw(painter);
    westLight->draw(painter);

    // Draw cars WITH ROTATION based on direction
    for (int i = 0; i < carCount; i++) {
        if (cars[i]) {
            int imageIndex = -1;
            qreal rotationAngle = 0;

            switch (cars[i]->getDirection()) {
            case Vehicle::NORTH:
                imageIndex = 0;  // car_n.png
                rotationAngle = 0;  // Pointing up (default)
                break;
            case Vehicle::SOUTH:
                imageIndex = 1;  // car_s.png
                rotationAngle = 180;  // Pointing down
                break;
            case Vehicle::EAST:
                imageIndex = 2;  // car_e.png
                rotationAngle = 90;   // Pointing right
                break;
            case Vehicle::WEST:
                imageIndex = 3;  // car_w.png
                rotationAngle = 270;  // Pointing left
                break;
            }

            if (imageIndex >= 0) {
                // Get the car image
                QPixmap carImage = carImages[imageIndex];

                // Apply rotation if needed
                if (rotationAngle != 0) {
                    QTransform transform;
                    transform.rotate(rotationAngle);
                    carImage = carImage.transformed(transform, Qt::SmoothTransformation);
                }

                // Draw the car with the (possibly rotated) image
                cars[i]->draw(painter, carImage);
            }
        }
    }

    // Draw info panel - FIXED: Shows simulation time properly
    painter.setPen(Qt::white);
    painter.setBrush(Qt::black);
    painter.drawRect(10, 10, 200, 90);

    // Calculate simulation time in seconds
    int seconds = lightCounter / 20; // 50ms ticks, 20 per second

    QString phase;
    if (lightCounter < 120) phase = "North-South GREEN";
    else if (lightCounter < 140) phase = "North-South YELLOW";
    else if (lightCounter < 160) phase = "ALL RED";
    else if (lightCounter < 280) phase = "East-West GREEN";
    else if (lightCounter < 300) phase = "East-West YELLOW";
    else phase = "ALL RED";

    painter.drawText(20, 30, QString("Time: %1s").arg(seconds));
    painter.drawText(20, 50, QString("Phase: %1").arg(phase));
    painter.drawText(20, 70, QString("Cars: %1/%2").arg(carCount).arg(MAX_CARS));
    painter.drawText(20, 90, QString("Status: %1").arg(timer->isActive() ? "RUNNING" : "STOPPED"));
}

void SimulationArea::update()
{
    // Spawn cars dynamically
    if (carCount < MAX_CARS) {
        int spawnRate = 20 - (carCount / 2);
        if (spawnRate < 5) spawnRate = 5;

        if (QRandomGenerator::global()->bounded(100) < spawnRate) {
            spawnCar();
        }
    }

    moveCars();
    repaint();
}

void SimulationArea::spawnCar()
{
    int direction = QRandomGenerator::global()->bounded(4);
    int lane = QRandomGenerator::global()->bounded(3);
    int x = 0, y = 0;
    Vehicle::Direction dir;

    for (int attempt = 0; attempt < 5; attempt++) {
        switch (direction) {
        case 0: // Southbound (from North to South)
            // FIXED: Lane positions 295, 325, 355
            x = 295 + lane * 30;  // 295, 325, 355
            y = -50;
            dir = Vehicle::SOUTH;
            break;
        case 1: // Northbound (from South to North)
            // FIXED: Lane positions 445, 475, 505
            x = 445 + lane * 30;  // 445, 475, 505
            y = 550;
            dir = Vehicle::NORTH;
            break;
        case 2: // Westbound (from East to West)
            // FIXED: Lane positions 295, 325, 355
            x = 850;
            y = 295 + lane * 30;  // 295, 325, 355
            dir = Vehicle::WEST;
            break;
        case 3: // Eastbound (from West to East)
            // FIXED: Lane positions 145, 175, 205
            x = -50;
            y = 145 + lane * 30;  // 145, 175, 205
            dir = Vehicle::EAST;
            break;
        }

        if (isSpawnPositionValid(x, y, dir)) {
            Car* newCar = new Car(x, y, dir);
            newCar->setLane(lane);

            // Set speeds: middle lane fastest
            if (lane == 1) {
                newCar->setBaseSpeed(3);
            } else {
                newCar->setBaseSpeed(2);
            }

            cars[carCount] = newCar;
            carCount++;
            totalCars++;
            return;
        }

        lane = (lane + 1) % 3;
    }
}

void SimulationArea::moveCars()
{
    int centerX = 400, centerY = 250;

    // First pass: Calculate desired speed for each car
    for (int i = 0; i < carCount; i++) {
        if (!cars[i]) continue;

        Car* car = cars[i];
        Vehicle::Direction dir = car->getDirection();
        int x = car->getX();
        int y = car->getY();

        // Base speed
        int desiredSpeed = car->getBaseSpeed();

        // Determine which traffic light controls this direction
        TrafficLight::State lightState = TrafficLight::GREEN;

        switch (dir) {
        case Vehicle::SOUTH: lightState = northLight->getState(); break;
        case Vehicle::NORTH: lightState = southLight->getState(); break;
        case Vehicle::WEST: lightState = eastLight->getState(); break;
        case Vehicle::EAST: lightState = westLight->getState(); break;
        }

        // Check if car is approaching or in intersection
        bool inIntersection = (x > 300 && x < 500 && y > 150 && y < 350);

        // Calculate distance to intersection for stopping logic
        int distToIntersection = 0;

        switch (dir) {
        case Vehicle::SOUTH: // Coming from North, going South
            distToIntersection = 150 - y; // Stop line at y=150
            break;
        case Vehicle::NORTH: // Coming from South, going North
            distToIntersection = y - 350; // Stop line at y=350
            break;
        case Vehicle::WEST: // Coming from East, going West
            distToIntersection = x - 500; // Stop line at x=500
            break;
        case Vehicle::EAST: // Coming from West, going East
            distToIntersection = 300 - x; // Stop line at x=300
            break;
        }

        bool approachingIntersection = (distToIntersection < 100 && distToIntersection > -20);
        bool atStopLine = (abs(distToIntersection) < 10);

        // Apply traffic light rules
        if (approachingIntersection || atStopLine || inIntersection) {
            if (lightState == TrafficLight::RED) {
                if (inIntersection) {
                    // Already in intersection - continue to clear it
                    desiredSpeed = std::min(desiredSpeed, 2);
                } else if (atStopLine || distToIntersection < 0) {
                    // At or past stop line - continue
                    desiredSpeed = std::min(desiredSpeed, 2);
                } else {
                    // Stop before intersection
                    if (distToIntersection < 50) {
                        desiredSpeed = 0;
                    } else {
                        desiredSpeed = std::max(0, distToIntersection / 30);
                    }
                }
            }
            else if (lightState == TrafficLight::YELLOW) {
                if (inIntersection) {
                    // In intersection - continue
                    desiredSpeed = std::min(desiredSpeed, 2);
                } else if (distToIntersection < 30) {
                    // Too close to stop safely - continue
                    desiredSpeed = std::min(desiredSpeed, 2);
                } else {
                    // Slow down
                    desiredSpeed = std::max(1, distToIntersection / 40);
                }
            }
            // Green light: no change to desired speed
        }

        // Check for car ahead in same lane
        Car* carAhead = nullptr;
        int distanceToCarAhead = INT_MAX;
        int currentLane = car->getLane();

        for (int j = 0; j < carCount; j++) {
            if (i != j && cars[j]) {
                if (cars[j]->getDirection() == dir &&
                    cars[j]->getLane() == currentLane) {

                    int dx = cars[j]->getX() - x;
                    int dy = cars[j]->getY() - y;

                    bool isAhead = false;
                    switch (dir) {
                    case Vehicle::SOUTH: isAhead = (dy > 0); break;
                    case Vehicle::NORTH: isAhead = (dy < 0); break;
                    case Vehicle::EAST: isAhead = (dx > 0); break;
                    case Vehicle::WEST: isAhead = (dx < 0); break;
                    }

                    if (isAhead) {
                        int distance = abs(dx) + abs(dy);
                        if (distance < distanceToCarAhead) {
                            distanceToCarAhead = distance;
                            carAhead = cars[j];
                        }
                    }
                }
            }
        }

        // Adjust speed based on distance to car ahead
        if (carAhead && distanceToCarAhead < 100) {
            int safeDistance = 40;
            if (distanceToCarAhead < safeDistance) {
                desiredSpeed = 0; // Emergency stop
            } else if (distanceToCarAhead < safeDistance * 2) {
                desiredSpeed = std::min(desiredSpeed, 1); // Slow down
            } else if (distanceToCarAhead < safeDistance * 3) {
                desiredSpeed = std::min(desiredSpeed, 2); // Moderate speed
            }
        }

        // Ensure speed is reasonable
        desiredSpeed = std::max(0, std::min(desiredSpeed, 3));

        // Store desired speed
        car->setDesiredSpeed(desiredSpeed);
    }

    // Second pass: Move cars with their desired speeds
    for (int i = 0; i < carCount; i++) {
        if (!cars[i]) continue;

        Car* car = cars[i];
        Vehicle::Direction dir = car->getDirection();
        int x = car->getX();
        int y = car->getY();

        // Get calculated desired speed
        int moveDistance = car->getDesiredSpeed();

        // Only move if speed > 0
        if (moveDistance <= 0) {
            continue;
        }

        // Store old position
        int oldX = x;
        int oldY = y;

        // Calculate new position
        switch (dir) {
        case Vehicle::NORTH: y -= moveDistance; break;
        case Vehicle::SOUTH: y += moveDistance; break;
        case Vehicle::EAST: x += moveDistance; break;
        case Vehicle::WEST: x -= moveDistance; break;
        }

        // Check for collisions
        car->setX(x);
        car->setY(y);
        bool collision = false;

        for (int j = 0; j < carCount; j++) {
            if (i != j && cars[j]) {
                if (car->collidesWith(cars[j])) {
                    collision = true;
                    break;
                }
            }
        }

        // If collision, revert to old position
        if (collision) {
            car->setX(oldX);
            car->setY(oldY);
            // Try lane change if possible
            attemptLaneChange(car);
        }

        // Remove if out of bounds
        if (x < -100 || x > 900 || y < -100 || y > 600) {
            delete cars[i];
            for (int j = i; j < carCount - 1; j++) {
                cars[j] = cars[j + 1];
            }
            cars[carCount - 1] = nullptr;
            carCount--;
            totalCars--;
            i--;
        }
    }
}

bool SimulationArea::isSpawnPositionValid(int x, int y, Vehicle::Direction dir) const
{
    QRect spawnArea;

    switch (dir) {
    case Vehicle::SOUTH:
        spawnArea = QRect(x - 25, y - 60, 50, 120);
        break;
    case Vehicle::NORTH:
        spawnArea = QRect(x - 25, y - 60, 50, 120);
        break;
    case Vehicle::WEST:
        spawnArea = QRect(x - 60, y - 25, 120, 50);
        break;
    case Vehicle::EAST:
        spawnArea = QRect(x - 60, y - 25, 120, 50);
        break;
    }

    for (int i = 0; i < carCount; i++) {
        if (cars[i]) {
            if (spawnArea.intersects(cars[i]->getBoundingBox())) {
                return false;
            }

            int dx = x - cars[i]->getX();
            int dy = y - cars[i]->getY();
            if (dx*dx + dy*dy < 3600) {
                return false;
            }
        }
    }

    return true;
}

bool SimulationArea::isLaneClear(Car* car, int lane) const
{
    Vehicle::Direction dir = car->getDirection();
    int x = car->getX();
    int y = car->getY();

    for (int i = 0; i < carCount; i++) {
        if (cars[i] && cars[i] != car) {
            if (cars[i]->getDirection() == dir &&
                cars[i]->getLane() == lane) {

                int dx = cars[i]->getX() - x;
                int dy = cars[i]->getY() - y;

                int distance = abs(dx) + abs(dy);
                if (distance < 80) {
                    return false;
                }
            }
        }
    }

    return true;
}

int SimulationArea::getLaneCenter(int lane, Vehicle::Direction dir, int otherCoord) const
{
    Q_UNUSED(otherCoord);

    if (dir == Vehicle::NORTH) {
        // Northbound lanes: 445, 475, 505
        int northLaneCenters[3] = {445, 475, 505};
        return northLaneCenters[lane];
    }
    else if (dir == Vehicle::SOUTH) {
        // Southbound lanes: 295, 325, 355
        int southLaneCenters[3] = {295, 325, 355};
        return southLaneCenters[lane];
    }
    else if (dir == Vehicle::EAST) {
        // Eastbound lanes: 145, 175, 205
        int eastLaneCenters[3] = {145, 175, 205};
        return eastLaneCenters[lane];
    }
    else { // WEST
        // Westbound lanes: 295, 325, 355
        int westLaneCenters[3] = {295, 325, 355};
        return westLaneCenters[lane];
    }
}

void SimulationArea::attemptLaneChange(Car* car)
{
    int currentLane = car->getLane();
    int desiredLane = currentLane;

    // Try left lane first
    if (currentLane > 0) {
        int newLane = currentLane - 1;
        if (isLaneClear(car, newLane)) {
            desiredLane = newLane;
        }
    }
    // Try right lane
    else if (currentLane < 2) {
        int newLane = currentLane + 1;
        if (isLaneClear(car, newLane)) {
            desiredLane = newLane;
        }
    }

    // Gradually change lane if needed
    if (desiredLane != currentLane) {
        Vehicle::Direction dir = car->getDirection();
        int x = car->getX();
        int y = car->getY();

        int targetX = getLaneCenter(desiredLane, dir, y);
        int targetY = getLaneCenter(desiredLane, dir, x);

        if (dir == Vehicle::NORTH || dir == Vehicle::SOUTH) {
            if (x < targetX) x += 1;
            else if (x > targetX) x -= 1;
        } else {
            if (y < targetY) y += 1;
            else if (y > targetY) y -= 1;
        }

        car->setX(x);
        car->setY(y);

        if (abs(x - targetX) < 2 && abs(y - targetY) < 2) {
            car->setLane(desiredLane);
        }
    }
}
