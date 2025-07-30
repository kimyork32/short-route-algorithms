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
    
    bool operator<(const Point& other) const {
    }
    
    bool operator==(const sf::Vector2i& other) const {
        return x == other.x && y == other.y;
    }
    
    bool isValid() {
        return x >= 0 && y >= 0;
    }
};

// Especialización de hash para Point
namespace std {
    template <>
    struct hash<Point> {
        std::size_t operator()(const Point& p) const noexcept {
            std::size_t hx = std::hash<int>{}(p.x);
            std::size_t hy = std::hash<int>{}(p.y);
            return hx ^ (hy << 1);
        }
    };
}

#endif
