#include "road.h"

int Road::total = 0;

Road::Road(const QString &name, int x, int y, int width, int height, Type type)
    : name(name), x(x), y(y), width(width), height(height), type(type)
{
    total++;
}
