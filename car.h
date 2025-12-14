#ifndef CAR_H
#define CAR_H

#include "vehicle.h"

class Car : public Vehicle
{
public:
    Car(int x = 0, int y = 0, Direction dir = NORTH);

    void draw(QPainter &painter, const QPixmap &image) override;
    void move() override;

    int getLane() const { return lane; }
    void setLane(int l) { lane = l; }

    int getBaseSpeed() const { return baseSpeed; }
    void setBaseSpeed(int s) { baseSpeed = s; speed = s; }

    int getDesiredSpeed() const { return desiredSpeed; }
    void setDesiredSpeed(int s) { desiredSpeed = s; }



private:
    int lane; // 0 = left, 1 = center, 2 = right
    int baseSpeed;
    int desiredSpeed;
};

#endif
