#include "../include/Malla.hpp"
#include <iostream>
#include <random>
#include <fstream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef K::Point_2 CGALPoint;

StaticDisplayMap::StaticDisplayMap(int width, int height, int size, int sizeNodes)
    : width(width), 
    height(height), 
    pointSize(size), 
    sizeNodes(sizeNodes)
{

    renderTexture.create(width, height);
    source = {-1, -1};
    target = {-1, -1};
    genRandGraph();
    // genArequipa(10000, 10000);
}


const Point* StaticDisplayMap::getPointIfExists(Point& point) {
    // preguntar si point colisiona con alguno de esots puntos con el tamaño de 3 (radio ciruclar)
    for (const auto& [from, edge] : graph) {
        // verificar colision
        if (pow(point.x - from.x, 2) + pow(point.y - from.y, 2) < pow(pointSize, 2))
            return &from;
    }
    return nullptr;
}

bool StaticDisplayMap::existsEdge(Point& from, Point& to) {
    const Point* fromTmp = getPointIfExists(from);
    const Point* toTmp = getPointIfExists(to);
    if (fromTmp && toTmp) {
        auto it = graph.find(*fromTmp);
        if (it != graph.end()) {
            const auto& vec = it->second;
            for (const auto& pair : vec) {
                if (pair.first == *toTmp) {
                    return true;
                }
            }
        }
    }
    return false;
}

void StaticDisplayMap::insertPointWindow(Point node) {
    const Point* tmp = getPointIfExists(node);
    if (tmp) {
        // std::cout << "existe node" << std::endl;
        return;
    }
    insertPoint(node);
    updateTextureUnit(&node, nullptr);
    sizeNodes++;
}

void StaticDisplayMap::insertEdgeWindow(Point from, Point to) {
    if (existsEdge(from, to)) {
        // std::cout << "existe arista" << std::endl;
        return;
    }
    const Point* f = getPointIfExists(from);
    const Point* t = getPointIfExists(to);
    if (f && t) {
        Point cf = *f;
        Point ct = *t;
        insertEdge(cf, ct);
        updateTextureUnit(&cf, &ct);
        return;
    }
    else if (f) {
        Point cf = *f;
        insertEdge(cf, to);
        updateTextureUnit(&cf, &to);
        return;
    }
    else if (t) {
        Point ct = *t;
        insertEdge(ct, to);
        updateTextureUnit(&ct, &to);
        return;
    }
    else {
        insertEdge(from, to);
        updateTextureUnit(&from, &to);
    }
}

void StaticDisplayMap::insertPoint(Point node) {
    if (node.x < 0 || node.x >= width || node.y < 0 || node.y >= height) return;
    graph[node];
}

void StaticDisplayMap::insertEdge(Point from, Point to, const std::vector<Point>& geometry) {
    if (!geometry.empty()) {
        graph[from].push_back({to, {true, geometry}});
        graph[to].push_back({from, {true, {}}});
    }
    else {
        graph[from].push_back({to, {false, {}}});
        graph[to].push_back({from, {false, {}}});
    }
    // updateTexture();
}

void StaticDisplayMap::insertEdge(Point from, Point to) {
    insertEdge(from, to, {});
    // updateTexture();
}

// #################### REMOVE #######################
//
// ################## FUNCION INTERSECCION ###############
// Calcula la orientación de tripleta (a, b, c)
// Devuelve:
// 0 -> colineales
// 1 -> sentido horario
// 2 -> antihorario
int StaticDisplayMap::orientation(const Point& a, const Point& b, const Point& c) {
    double val = (b.y - a.y) * (c.x - b.x) - 
                 (b.x - a.x) * (c.y - b.y);
    
    if (val == 0) return 0;           // Colineales
    return (val > 0) ? 1 : 2;         // Horario o antihorario
}

// Verifica si punto c está sobre el segmento ab
bool StaticDisplayMap::inSegment(const Point& a, const Point& b, const Point& c) {
    return c.x <= std::max(a.x, b.x) && c.x >= std::min(a.x, b.x) &&
           c.y <= std::max(a.y, b.y) && c.y >= std::min(a.y, b.y);
}

// Función principal: determina si dos segmentos se intersecan
bool StaticDisplayMap::ifIntersect(const Point& p1, const Point& p2, const Point& p3, const Point& p4) {

    int o1 = orientation(p1, p2, p3);
    int o2 = orientation(p1, p2, p4);
    int o3 = orientation(p3, p4, p1);
    int o4 = orientation(p3, p4, p2);

    // Caso general
    if (o1 != o2 && o3 != o4) 
        return true; 

    // Casos especiales: colineales y uno sobre el otro
    if (o1 == 0 && inSegment(p1, p2, p3)) return true;
    if (o2 == 0 && inSegment(p1, p2, p4)) return true;
    if (o3 == 0 && inSegment(p3, p4, p1)) return true;
    if (o4 == 0 && inSegment(p3, p4, p2)) return true;

    return false;
}
 // #####################################################


void StaticDisplayMap::removeNodeWindow(Point node) {
    const Point* n = getPointIfExists(node);
    if (n) {
        // std::cout << "existe nodo a remover" << std::endl;
        Point p = *n;
        for (auto& [from, edge] : graph) {
            edge.erase(
                std::remove_if(edge.begin(), edge.end(),
                    [&](const std::pair<Point, std::pair<bool, std::vector<Point>>>& e) {
                        return e.first == p;
                    }
                ),
                edge.end()
            );
        }
        graph.erase(p);
        updateTextureAll();
    }
}

void StaticDisplayMap::removeEdgeWindow(Point from, Point to) {
    bool change = false;
    // std::cout << "removiento arista" << std::endl;
    for (auto& [from2, edge] : graph) {
        edge.erase(
            std::remove_if(edge.begin(), edge.end(),
                [&](const std::pair<Point, std::pair<bool, std::vector<Point>>>& e) {
                    const Point& to2 = e.first;
                    bool c = ifIntersect(from, to, from2, to2);
                    if (c) {
                        // std::cout << "se intersecan (" << from2.x << ", " << from2.y << "), (" << to.x << ", " << to.y << ")" << std::endl;
                        change = true;

                    }
                    return c;
                }
            ),
            edge.end()
        );
    }
    if (change) {
        updateTextureAll();
    }
}

void  StaticDisplayMap::insertStartEndNode(Point source, Point target) {
    const Point* s = getPointIfExists(source);
    const Point* t = getPointIfExists(target);
    if (!s || !t) {
        std::cerr << "no existe un nodo seleccionado. intente de nuevo\n";
        return;
    }

    Point cS = *s;
    Point cT = *t;

    this->source = cS;
    this->target = cT;
    updateTexturePoint(&cS, sf::Color::Green);
    updateTexturePoint(&cT, sf::Color::Green);
}

float StaticDisplayMap::getDistanceNodes(const Point& from, const Point& to) {
    return std::sqrtl(std::pow(from.x - to.x, 2) + std::pow(from.y - to.y, 2));
}

float StaticDisplayMap::getDistance(Point& from, Point& to) {
    auto it = graph.find(from);
    if (it != graph.end()) {
        const auto& edgeList = it->second;
        for (const auto& [to2, edgedata] : edgeList) {
            if (to2 == to) {
                if (edgedata.first) {
                    float distance = 0.f;
                    Point current = from;
                    const auto& geometry = edgedata.second;
                    for (const Point& node : geometry) {
                        distance += getDistanceNodes(current, node);
                        current = node;
                    }
                    distance += getDistanceNodes(current, to);
                    return distance;
                }
                else {
                    break;
                }
            }
        }
    }
    return getDistanceNodes(from, to);
}

void StaticDisplayMap::dijkstra() {
    if (!source.isValid() || !target.isValid()) {
        std::cout << "nodos start y end no iniciados" << std::endl;
    }

}

void StaticDisplayMap::aStar() {
    // TODO
    // route = {};
}

void StaticDisplayMap::genRandGraph() {
    std::cout << "creando mapa random" << std::endl;
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Delaunay = CGAL::Delaunay_triangulation_2<Kernel>;
    using CGALPoint = Kernel::Point_2;

    std::random_device rd;
    std::mt19937 gen(rd());
    // std::uniform_int_distribution<> distrib(0, coordMax);
    std::uniform_int_distribution<> distribX(0, width - 1);
    std::uniform_int_distribution<> distribY(0, height - 1);

    std::vector<CGALPoint> puntos;

    for (int i = 0; i < sizeNodes; ++i) {
        int x = distribX(gen);
        int y = distribY(gen);
        puntos.emplace_back(x, y);
    }

    Delaunay dt;
    dt.insert(puntos.begin(), puntos.end());

    graph.clear();

    for (const auto& p : puntos) {
        Point pt{static_cast<int>(p.x()), static_cast<int>(p.y())};
        insertPoint(pt);
    }

    for (auto it = dt.finite_edges_begin(); it != dt.finite_edges_end(); ++it) {
        auto face = it->first;
        int i = it->second;

        auto v1 = face->vertex((i + 1) % 3)->point();
        auto v2 = face->vertex((i + 2) % 3)->point();

        Point p1{static_cast<int>(v1.x()), static_cast<int>(v1.y())};
        Point p2{static_cast<int>(v2.x()), static_cast<int>(v2.y())};

        // graph[p1].emplace_back(p2, std::vector<Point>{false, {}});
        // graph[p2].emplace_back(p1, std::vector<Point>{false, {}});

        graph[p1].emplace_back(p2, std::pair<bool, std::vector<Point>>{false, {}});
        graph[p2].emplace_back(p1, std::pair<bool, std::vector<Point>>{false, {}});
    }
    updateTextureAll();
}

void StaticDisplayMap::genArequipa(int mapWidth, int mapHeight) {
    txtPointsToGraph(mapWidth, mapHeight, "resources/Arequipa-Miraflores.txt");
}

void StaticDisplayMap::genLima(int mapWidth, int mapHeight) {
    txtPointsToGraph(mapWidth, mapHeight, "resources/Surco-Lima.txt");
}

void StaticDisplayMap::genNewYork(int mapWidth, int mapHeight) {
    txtPointsToGraph(mapWidth, mapHeight, "resources/NewYork-USA.txt");
}

void StaticDisplayMap::render(sf::RenderWindow& window) {
    window.draw(mapSprite);
}


void StaticDisplayMap::updateTexturePoint(Point* node, sf::Color color) {
    sf::CircleShape circle(pointSize);
    circle.setOrigin(pointSize, pointSize);
    circle.setPosition(node->x, node->y);
    circle.setFillColor(color);
    renderTexture.draw(circle);
}

void StaticDisplayMap::updateTextureUnit(Point* from, Point* to) {
    updateTextureUnit(from, to, sf::Color::White);
}

void StaticDisplayMap::updateTextureUnit(Point* from, Point* to, sf::Color color) {
    if (from && to) {
        sf::VertexArray finalSegment(sf::Lines, 2);
        finalSegment[0].position = sf::Vector2f(from->x, from->y);
        finalSegment[0].color = sf::Color::White;
        finalSegment[1].position = sf::Vector2f(to->x, to->y);
        finalSegment[1].color = color;
        renderTexture.draw(finalSegment);
    }
    if (from) {
        sf::CircleShape circle(pointSize);
        circle.setOrigin(pointSize, pointSize);
        circle.setPosition(from->x, from->y);
        circle.setFillColor(sf::Color::Red);
        renderTexture.draw(circle);
    }
    if (to) {
        sf::CircleShape circle(pointSize);
        circle.setOrigin(pointSize, pointSize);
        circle.setPosition(to->x, to->y);
        circle.setFillColor(sf::Color::Red);
        renderTexture.draw(circle);
    }
    renderTexture.display();
    mapSprite.setTexture(renderTexture.getTexture());
}

void StaticDisplayMap::updateTextureRoute(){
    if (route.empty()) {
        std::cerr << "ruta vacia" << std::endl;
        return;
    }
    for (const auto& [from, to]: route) {
        auto it = graph.find(from);
        if (it != graph.end()) {
            const auto& edgeList = it->second;
            for (const auto& [to2, edgeData] : edgeList) {
                if (to2 == to) {
                    // existe geometry
                    if (edgeData.first) {
                        // si geometry esta vacio, entocnes está en el otro nodo 
                        if (edgeData.second.empty()) {
                            auto it2 = graph.find(to);
                            if (it2 != graph.end()) {
                                const auto& edgeList2 = it2->second;
                                for (const auto& [to3, edgeData2] : edgeList2) {
                                    if (to3 == to) {
                                        Point current = to3;
                                        const auto& geometry = edgeData2.second;
                                        for (const Point& node : geometry) {
                                            sf::VertexArray segment(sf::Lines, 2);
                                            segment[0].position = sf::Vector2f(current.x, current.y);
                                            segment[0].color = sf::Color::Blue;
                                            segment[1].position = sf::Vector2f(node.x, node.y);
                                            segment[1].color = sf::Color::Blue;
                                            renderTexture.draw(segment);
                                        }

                                        sf::VertexArray segment(sf::Lines, 2);
                                        segment[0].position = sf::Vector2f(current.x, current.y);
                                        segment[0].color = sf::Color::Blue;
                                        segment[1].position = sf::Vector2f(to3.x, to3.y);
                                        segment[1].color = sf::Color::Blue;
                                        renderTexture.draw(segment);
                                    }
                                }
                            }
                        }
                        else {
                            Point current = from;
                            const auto& geometry = edgeData.second;
                            for (const Point& node : geometry) {
                                sf::VertexArray segment(sf::Lines, 2);
                                segment[0].position = sf::Vector2f(current.x, current.y);
                                segment[0].color = sf::Color::Blue;
                                segment[1].position = sf::Vector2f(node.x, node.y);
                                segment[1].color = sf::Color::Blue;
                                renderTexture.draw(segment);
                            }
                            sf::VertexArray segment(sf::Lines, 2);
                            segment[0].position = sf::Vector2f(current.x, current.y);
                            segment[0].color = sf::Color::Blue;
                            segment[1].position = sf::Vector2f(to2.x, to2.y);
                            segment[1].color = sf::Color::Blue;
                            renderTexture.draw(segment);
                        }
                    }
                    else {
                        sf::VertexArray segment(sf::Lines, 2);
                        segment[0].position = sf::Vector2f(from.x, from.y);
                        segment[0].color = sf::Color::Blue;
                        segment[1].position = sf::Vector2f(to2.x, to2.y);
                        segment[1].color = sf::Color::Blue;
                        renderTexture.draw(segment);
                    }
                }
            }
        }
    }
    renderTexture.display();
    mapSprite.setTexture(renderTexture.getTexture());
}

void StaticDisplayMap::updateTextureAll() {
    // std::cout << "actualizanfo mapa" << std::endl;
    renderTexture.clear(sf::Color::Black);

    sf::CircleShape circle(pointSize);
    circle.setOrigin(pointSize, pointSize);
    
    for (const auto& [from, edge] : graph) {
        for (const auto& [to, edgeData] : edge) {
            const auto& [isGeo, geometry] = edgeData;
            // Si hay puntos intermedios (geometry), dibujarlos como líneas consecutivas
            std::vector<Point> tempGeometry = geometry;
            if (isGeo && geometry.empty()) {
                continue;
            }
            if (!geometry.empty()) {
            // if (isGeo) {
                // if (tempGeometry.empty()) {
                //     auto it = graph.find(to);
                //     if (it != graph.end()) {
                //         const auto& edgeList = it->second;
                //         for (const auto& [to2, edgeData2] : edgeList) {
                //             if (to2 == from) {
                //                 const auto& backGeometry = edgeData2.second;
                //                 tempGeometry = backGeometry;
                //                 break;
                //             }
                //         }
                //     }
                // }
                // std::cout << "tempGeometry.size(): " << tempGeometry.size() << std::endl;
                Point current = from;
                for (const auto& mid : tempGeometry) {
                    sf::VertexArray segment(sf::Lines, 2);
                    segment[0].position = sf::Vector2f(current.x, current.y);
                    segment[0].color = sf::Color::White;
                    segment[1].position = sf::Vector2f(mid.x, mid.y);
                    segment[1].color = sf::Color::White;
                    // std::cout << "segment: " << current.x << " " << current.y << " | " << mid.x << " " << mid.y << "\n";
                    
                    renderTexture.draw(segment);
                    current = mid;
                }
                // Último tramo: del último intermedio a `to`
                sf::VertexArray finalSegment(sf::Lines, 2);
                finalSegment[0].position = sf::Vector2f(current.x, current.y);
                finalSegment[0].color = sf::Color::White;
                finalSegment[1].position = sf::Vector2f(to.x, to.y);
                finalSegment[1].color = sf::Color::White;
                // std::cout << "segment final: " << current.x << " " << current.y << " | " << to.x << " " << to.y << "\n";
                renderTexture.draw(finalSegment);
            }
            else {
                // Dibujar línea directa de from a to
                sf::VertexArray line(sf::Lines, 2);
                line[0].position = sf::Vector2f(from.x, from.y);
                line[0].color = sf::Color::White;
                line[1].position = sf::Vector2f(to.x, to.y);
                line[1].color = sf::Color::White;
                renderTexture.draw(line);
            }
            circle.setPosition(to.x, to.y);
            circle.setFillColor(sf::Color::Red);
            renderTexture.draw(circle);
        }
        circle.setPosition(from.x, from.y);
        circle.setFillColor(sf::Color::Red);
        renderTexture.draw(circle);
    }
    renderTexture.display();
    mapSprite.setTexture(renderTexture.getTexture());
}

// vector = 0: old_min, 1: old_max, 2: new_min, 3: new_max)
double StaticDisplayMap::remap(double value, double range[4]) {
    if (range[1] == range[0]) {
        std::cerr << "División por cero en remap (rango inválido)\n";
        return 0;
    }
    std::cout << "valores remap: " << range[0] << " " << range[1] << " " << range[2] << " " << range[3] << std::endl;
    double result = range[2] + (value - range[0]) * (range[3] - range[2]) / (range[1] - range[0]);
    std::cout << "result: " << result << "\n";
    return result;
}

// 0: o, 1: e, 2: s, 3: n, 4: x1, 5: x2, 6: y1, 7: y2
std::vector<int> StaticDisplayMap::normalizedPoints(std::vector<double> points, double range[8]) {
    // Rango original: x ∈ [n, o], y ∈ [e, s]
    // Nuevo rango:    x ∈ [x1, x2], y ∈ [y1, y2]
    std::vector<int> normalized;

    // Proteger contra rangos invertidos
    double oldXmin = std::min(range[3], range[2]); // oeste, este
    double oldXmax = std::max(range[3], range[2]);
    double oldYmin = std::min(range[1], range[0]); // sur, norte
    double oldYmax = std::max(range[1], range[0]);

    double xTmp[4] = {oldXmin, oldXmax, range[4], range[5]};
    double yTmp[4] = {oldYmin, oldYmax, range[6], range[7]};

    for (int i = 0; i < points.size() / 2; i++) {

        double x = points[i * 2];       // longitud
        double y = points[i * 2 + 1];   // latitud

        int x_norm = static_cast<int>(remap(x, xTmp));
        int y_norm = static_cast<int>(remap(y, yTmp));

        std::cout << "x: " << points[i * 2] << " → " << x_norm << "\n";
        std::cout << "y: " << points[i * 2 + 1] << " → " << y_norm << "\n";

        normalized.push_back(x_norm);
        normalized.push_back(y_norm);
    }

    return normalized;
}

void StaticDisplayMap::txtPointsToGraph(int mapWidth, int mapHeight, std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "fallo al abrir " << path << "\n";
        return;
    }
    double limitsFile[8];
    // norte sur este oeste
    file >> limitsFile[0] >> limitsFile[1] >> limitsFile[2] >> limitsFile[3];
    std::cout << limitsFile[0] << " "<< limitsFile[1] << " "<< limitsFile[2] << " "<< limitsFile[3] << " ";
    limitsFile[4] = 0;
    limitsFile[5] = static_cast<double>(mapWidth);
    limitsFile[6] = 0;
    limitsFile[7] = static_cast<double>(mapHeight);

    std::cout << "limpiando grafo\n";
    graph.clear();

    while (true) {
        if (file.eof()) break;
        if (file.fail()) {
            std::cerr << "error de lectura" << std::endl;
            return;
        }
        std::string line;
        getline(file, line);
        if (line.empty()) continue;
        std::cout << "line: " << line << std::endl;
        std::stringstream ss(line);
        double num;
        std::vector<double> temp;
        while (ss >> num) {
            temp.push_back(num);
        }
        // normalizar
        std::cout << "tamaño temp: " << temp.size() << std::endl;
        std::vector<int> tempInts = normalizedPoints(temp, limitsFile);
        std::cout << "tamaño tempInts: " << tempInts.size() << std::endl;
        for (auto e : tempInts) {
            std::cout << e << " ";
        }
        std::cout << "\n";
        Point source(tempInts[0], tempInts[1]);
        Point target(tempInts[tempInts.size() - 2], tempInts[tempInts.size() - 1]);
        std::vector<Point> geometry;
        std::cout << "tamaño tempInts: " << tempInts.size() << std::endl;
        if (tempInts.size() > 4) {
            // obteniendo los puntos de geometry
            for (int i = 2; i < tempInts.size() - 2; i += 2) {
                Point point(tempInts[i], tempInts[i + 1]);
                geometry.push_back(point);
            }
            std::cout << "geometry: ";
            for (Point e : geometry) std::cout << e.x << " " << e.y << " ";
            std::cout << "\n";
            insertEdge(source, target, geometry);
        }
        else {
            // no no existe geometry
            insertEdge(source, target);
        }
    }
    std::cout << "tam grafo: " << graph.size() << std::endl;
    std::cout << "actualizar grafo" << std::endl;
    updateTextureAll();

    // 100 300 200 300
    // 200 300 150 400
    // 150 400 600 200
    // 600 200 300 100

    route = {
        {{100, 300}, {200, 300}},
        {{200, 300}, {150, 400}},
        {{150, 400}, {600, 200}},
        {{600, 200}, {300, 100}}
    };

    updateTextureRoute();

    file.close();
}

int StaticDisplayMap::getSizeNodes() const {
    return sizeNodes;
}

sf::Sprite StaticDisplayMap::getMapSprite() const {
    return mapSprite;
}

