#ifndef MALLA_HPP
#define MALLA_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>
#include "Point.hpp"
#include "GraphAlgorithmBase.hpp"
#include "pathfinding/ShortestPathDijkstra.hpp"
#include "pathfinding/ShortestPathAStar.hpp"

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

    // SISTEMA DE BARRERAS
    void placeBarrierNode(Point node);
    void removeBarrierNode(Point node);
    void placeBarrierEdge(Point from, Point to);
    void removeBarrierEdge(Point from, Point to);
    bool isNodeBlocked(const Point& node) const;
    bool isEdgeBlocked(const Point& from, const Point& to) const;
    void clearAllBarriers();

    float getDistance(Point& from, Point& to);
    float getDistanceNodes(const Point& from, const Point& to);
    
    // ALGORITMOS DE PATHFINDING
    bool executePathfindingDijkstra();
    bool executePathfindingAStar();
    void clearCurrentPath();
    void printPathStatistics() const;

    void genRandGraph();
    void genLima(int mapWidth, int mapHeight);
    void genArequipa(int mapWidth, int mapHeight);
    void genNewYork(int mapWidth, int mapHeight);

    void render(sf::RenderWindow& window);
    void updateTextureUnit(Point* from, Point* to);
    void updateTexturePoint(Point* node, sf::Color color);
    void updateTextureUnit(Point* from, Point* to, sf::Color color);
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
    std::vector<std::pair<Point, Point>> route;
    sf::RenderTexture renderTexture;
    sf::Sprite mapSprite;
    int sizeNodes;

    // para ruta
    Point source;
    Point target;
    
    // SISTEMA DE BARRERAS
    std::unordered_set<Point> blockedNodes;
    std::unordered_set<std::pair<Point, Point>, std::hash<std::pair<Point, Point>>> blockedEdges;
    
    // Instancias de algoritmos
    ShortestPathDijkstra dijkstraAlgorithm;
    ShortestPathAStar astarAlgorithm;
    PathfindingResult currentPathResult;
};

#endif
