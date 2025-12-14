#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <QPainter>

class TrafficLight
{
public:
    enum State { RED, YELLOW, GREEN };

    TrafficLight(int x = 0, int y = 0);

    int getX() const { return x; }
    int getY() const { return y; }
    State getState() const { return state; }

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setState(State s) { state = s; }

    void draw(QPainter &painter) const;

    static int count;

private:
    int x, y;
    State state;
};

#endif
