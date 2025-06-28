#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <functional>

struct Point {
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
    Point(const sf::Vector2i& v) : x(v.x), y(v.y) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    bool operator==(const sf::Vector2i& other) const {
        return x == other.x && y == other.y;
    }
};

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

class StaticDisplayMap {
public:
    StaticDisplayMap(int width, int height, int pointSize = 1);

    void insertPoint(Point node);
    void insertEdge(Point from, Point to);
    void genRandGraph();
    void render(sf::RenderWindow& window);

private:
    int width, height, pointSize;
    std::unordered_map<Point, std::vector<std::pair<Point, std::vector<Point>>>> graph;
};
