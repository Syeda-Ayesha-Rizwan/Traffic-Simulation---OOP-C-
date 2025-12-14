#include "car.h"
#include <QRandomGenerator>

Car::Car(int x, int y, Direction dir)
    : Vehicle(x, y, dir), lane(1) // Start in center lane by default
{
    speed = 2 + QRandomGenerator::global()->bounded(2);
}

void Car::draw(QPainter &painter, const QPixmap &image)
{
    painter.save();

    if (!image.isNull()) {
        painter.translate(x, y);
        switch (direction) {
        case NORTH: painter.rotate(180); break;
        case SOUTH: break;
        case EAST: painter.rotate(90); break;
        case WEST: painter.rotate(270); break;
        }
        painter.drawPixmap(-width/2, -height/2, width, height, image);
    }

    painter.restore();
}

void Car::move()
{
    // Movement is controlled by SimulationArea with lane changing logic
    // This function is kept for interface compatibility
    switch (direction) {
    case NORTH: y -= speed; break;
    case SOUTH: y += speed; break;
    case EAST: x += speed; break;
    case WEST: x -= speed; break;
    }
}
