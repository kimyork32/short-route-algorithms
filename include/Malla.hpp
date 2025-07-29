#ifndef MALLA_HPP
#define MALLA_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <functional>
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

class StaticDisplayMap {
public:
    StaticDisplayMap(int width, int height, int pointSize, int sizeNodes);

    const Point* getPointIfExists(Point& point);
    bool existsEdge(Point& from, Point& to);
    void insertPointWindow(Point node);
    void insertEdgeWindow(Point from, Point to);
    void insertPoint(Point node);
    void insertEdge(Point from, Point to, const std::vector<Point>& geometry);
    void insertEdge(Point from, Point to); // sobrecarga de funcion 
    void insertStartEndNode(Point from, Point to);

    void removeNodeWindow(Point node);
    void removeEdgeWindow(Point from, Point to);

    float getDistance(Point& from, Point& to);
    float getDistanceNodes(const Point& from, const Point& to);
    void dijkstra();
    void aStar();

    void genRandGraph();
    void genLima(int mapWidth, int mapHeight);
    void genArequipa(int mapWidth, int mapHeight);
    void genNewYork(int mapWidth, int mapHeight);

    void render(sf::RenderWindow& window);
    void updateTextureUnit(Point* from, Point* to);
    void updateTextureRoute();
    void updateTextureAll();
    void updateMinimap();
    
    int orientation(const Point& a, const Point& b, const Point& c);
    bool inSegment(const Point& a, const Point& b, const Point& c);
    bool ifIntersect(const Point& p1, const Point& p2, const Point& p3, const Point& p4);

    // OBTENCION DE PUNTOS
    // vector = 0: old_min, 1: old_max, 2: new_min, 3: new_max)
    double remap(double value, double range[4]);
    // 0: o, 1: e, 2: s, 3: n, 4: x1, 5: x2, 6: y1, 7: y2
    std::vector<int> normalizedPoints(std::vector<double> points, double range[8]);
    void txtPointsToGraph(int mapWidth, int mapHeight, std::string path);

    int getSizeNodes() const;
    sf::Sprite getMapSprite() const;

private:
    int width, height, pointSize;

    std::unordered_map<Point, std::vector<std::pair<Point, std::pair<bool, std::vector<Point>>>>> graph;
    std::vector<std::pair<Point, std::pair<Point, std::vector<Point>>>> route;
    sf::RenderTexture renderTexture;
    sf::Sprite mapSprite;
    int sizeNodes;

    // para ruta
    Point source;
    Point target;
};

#endif
