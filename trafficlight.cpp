#include "trafficlight.h"

int TrafficLight::count = 0;

TrafficLight::TrafficLight(int x, int y) : x(x), y(y), state(RED)
{
    count++;
}

void TrafficLight::draw(QPainter &painter) const
{
    painter.save();

    // Pole
    painter.setBrush(QColor(100, 100, 100));
    painter.drawRect(x - 5, y, 10, 30);

    // Light box
    painter.setBrush(QColor(50, 50, 50));
    painter.drawRect(x - 15, y - 40, 30, 50);

    // Red light
    painter.setBrush(state == RED ? Qt::red : Qt::darkRed);
    painter.drawEllipse(x - 10, y - 35, 20, 20);

    // Yellow light
    painter.setBrush(state == YELLOW ? Qt::yellow : Qt::darkYellow);
    painter.drawEllipse(x - 10, y - 10, 20, 20);

    // Green light
    painter.setBrush(state == GREEN ? Qt::green : Qt::darkGreen);
    painter.drawEllipse(x - 10, y + 15, 20, 20);

    painter.restore();
}
