#ifndef MALLA_HPP
#define MALLA_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <functional>
#include "Point.hpp"
#include "GraphAlgorithmBase.hpp"
#include "../include/CGALincludes.hpp"

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

    void genRandPoints(std::vector<CGALPoint>& puntos, int start, int end, unsigned seed);
    void genRandGraph();
    void genLima(int mapWidth, int mapHeight);
    void genArequipa(int mapWidth, int mapHeight);
    void genNewYork(int mapWidth, int mapHeight);

    void removeVisualRoute(const Point* from, const Point* to);
    void drawSegment(const Point* from, const Point* to, sf::Color color);
    void drawCircle(const Point* node, sf::Color color);

    void render(sf::RenderWindow& window);
    void updateTexturePoint(Point* node, sf::Color color);
    void updateTextureUnit(Point* from, Point* to);
    void updateTextureUnit(Point* from, Point* to, sf::Color color, sf::Color colorPoints);
    void updateTextureSegment(Point* from, Point* to);
    void updateTextureRoute();
    void updateTextureAll();
    // void updateMinimap();
    
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

    float getLastBuildTime() const;
    float getLastSearchTime() const;
    int getLastRouteSize() const;
    float getEstimatedMemoryUsage() const;

private:
    int width, height, pointSize;

    std::unordered_map<Point, std::vector<std::pair<Point, std::pair<bool, std::vector<Point>>>>> graph;
    std::vector<Point> route;
    sf::RenderTexture renderTexture;
    sf::Sprite mapSprite;
    int sizeNodes;

    // para ruta
    Point source;
    Point target;
    
    // Instancias de algoritmos
    ShortestPathDijkstra dijkstraAlgorithm;
    ShortestPathAStar astarAlgorithm;
    PathfindingResult currentPathResult;

    float lastBuildTime = 0.f;
    float lastSearchTime = 0.f;
    int lastRouteSize = 0;
    float estimatedMemoryUsage = 0.f;
};

#endif
