#ifndef VEHICLE_H
#define VEHICLE_H

#include <QPainter>
#include <QRect>

class Vehicle
{
public:
    enum Direction { NORTH, SOUTH, EAST, WEST };

    Vehicle(int x = 0, int y = 0, Direction dir = NORTH);
    virtual ~Vehicle() {}

    virtual void draw(QPainter &painter, const QPixmap &image) = 0;
    virtual void move() = 0;

    int getX() const { return x; }
    int getY() const { return y; }
    int getSpeed() const { return speed; }
    Direction getDirection() const { return direction; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    QRect getBoundingBox() const;

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setSpeed(int s) { speed = s; }
    void setDirection(Direction d) { direction = d; }
    void setSize(int w, int h) { width = w; height = h; }

    bool collidesWith(const Vehicle* other) const;
    bool isTooClose(const Vehicle* other, int minDistance = 40) const;

    static int total;

protected:
    int x, y;
    int speed;
    Direction direction;
    int width, height;
};

#endif
