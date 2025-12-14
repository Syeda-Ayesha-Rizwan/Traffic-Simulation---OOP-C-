#include "vehicle.h"
#include <cmath>

int Vehicle::total = 0;

Vehicle::Vehicle(int x, int y, Direction dir)
    : x(x), y(y), speed(2), direction(dir), width(30), height(15)
{
    total++;
}

QRect Vehicle::getBoundingBox() const
{
    return QRect(x - width/2, y - height/2, width, height);
}

bool Vehicle::collidesWith(const Vehicle* other) const
{
    if (!other) return false;

    QRect thisRect = getBoundingBox();
    QRect otherRect = other->getBoundingBox();

    return thisRect.intersects(otherRect);
}

bool Vehicle::isTooClose(const Vehicle* other, int minDistance) const
{
    if (!other) return false;

    // Calculate distance between vehicle centers
    int dx = x - other->x;
    int dy = y - other->y;
    double distance = sqrt(dx*dx + dy*dy);

    return distance < minDistance;
}
