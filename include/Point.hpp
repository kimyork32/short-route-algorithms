#ifndef POINT_HPP
#define POINT_HPP

#include <SFML/Graphics.hpp>
#include <functional>

struct Point {
    int x;
    int y;
    Point(int x, int y) : x(x), y(y) {}
    Point() : x(0), y(0) {}
    Point(const sf::Vector2i& v) : x(v.x), y(v.y) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    bool operator==(const sf::Vector2i& other) const {
        return x == other.x && y == other.y;
    }
    bool isValid() {
        return x >= 0 && y >= 0;
    }
};

#endif
