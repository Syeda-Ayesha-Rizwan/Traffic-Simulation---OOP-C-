#ifndef ROAD_H
#define ROAD_H

#include <QString>

class Road
{
public:
    enum Type { HORIZONTAL, VERTICAL };

    Road(const QString &name, int x, int y, int width, int height, Type type);

    QString getName() const { return name; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    Type getType() const { return type; }

    void setName(const QString &n) { name = n; }
    void setPosition(int x, int y) { this->x = x; this->y = y; }
    void setSize(int w, int h) { width = w; height = h; }

    static int total;

private:
    QString name;
    int x, y;
    int width, height;
    Type type;
};

#endif
