#ifndef PARSER_HPP
#define PARSER_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include "Point.hpp"

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

class Parser {
public: 
    Parser();
    void getGraph();
    void buildGraph();

private: 
    std::string filename;
    std::fstream file;
    std::unordered_map<Point, std::vector<std::pair<Point, std::vector<Point>>>> graph;

    // (x1, y1) -> [{(x2, y2), [(xa, ya), ..., (xi, yi)]}, {(x3, y3), []}, (...)]
    // x1 y1
    // x2 y2 xa ya ... xi yi
    // x3 y3 
    // -1 -1
    // x2 y2
};

#endif
