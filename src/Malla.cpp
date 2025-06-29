#include "../include/Malla.hpp"
#include <iostream>
#include <random>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef K::Point_2 CGALPoint;

StaticDisplayMap::StaticDisplayMap(int width, int height, int size, int sizeNodes)
    : width(width), height(height), pointSize(size), sizeNodes(sizeNodes) {

    renderTexture.create(width, height);
    genRandGraph();
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
        std::cout << "existe node" << std::endl;
        return;
    }
    insertPoint(node);
    updateTextureUnit(&node, nullptr);
    sizeNodes++;
}

void StaticDisplayMap::insertEdgeWindow(Point from, Point to) {
    if (existsEdge(from, to)) {
        std::cout << "existe arista" << std::endl;
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

void StaticDisplayMap::insertEdge(Point from, Point to) {
    graph[from].push_back({to, {}});
    graph[to].push_back({from, {}});
    // updateTexture();
}

// #################### REMOVE #######################
//
// ################## FUNCION INSERSECCION ###############
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
        std::cout << "existe nodo a remover" << std::endl;
        Point p = *n;
        for (auto& [from, edge] : graph) {
            edge.erase(
                std::remove_if(edge.begin(), edge.end(),
                    [&](const std::pair<Point, std::vector<Point>>& e) {
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
    std::cout << "removiento arista" << std::endl;
    for (auto& [from2, edge] : graph) {
        edge.erase(
            std::remove_if(edge.begin(), edge.end(),
                [&](const std::pair<Point, std::vector<Point>>& e) {
                    const Point& to2 = e.first;
                    bool c = ifIntersect(from, to, from2, to2);
                    if (c) {
                        std::cout << "se intersecan (" << from2.x << ", " << from2.y << "), (" << to.x << ", " << to.y << ")" << std::endl;
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

void StaticDisplayMap::genRandGraph() {
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Delaunay = CGAL::Delaunay_triangulation_2<Kernel>;
    using CGALPoint = Kernel::Point_2;

    // Generador aleatorio
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

    // triangulacion
    Delaunay dt;
    dt.insert(puntos.begin(), puntos.end());

    // limpiar el grafo
    graph.clear();

    // insertar puntos como nodos
    for (const auto& p : puntos) {
        Point pt{static_cast<int>(p.x()), static_cast<int>(p.y())};
        insertPoint(pt);
    }

    // insertar aristas bidireccionales
    for (auto it = dt.finite_edges_begin(); it != dt.finite_edges_end(); ++it) {
        auto face = it->first;
        int i = it->second;

        auto v1 = face->vertex((i + 1) % 3)->point();
        auto v2 = face->vertex((i + 2) % 3)->point();

        Point p1{static_cast<int>(v1.x()), static_cast<int>(v1.y())};
        Point p2{static_cast<int>(v2.x()), static_cast<int>(v2.y())};

        graph[p1].emplace_back(p2, std::vector<Point>{});
        graph[p2].emplace_back(p1, std::vector<Point>{});
    }
    updateTextureAll();
}

void StaticDisplayMap::render(sf::RenderWindow& window) {
    window.draw(mapSprite);
}

void StaticDisplayMap::updateTextureUnit(Point* from, Point* to) {
    if (from && to) {
        sf::VertexArray finalSegment(sf::Lines, 2);
        finalSegment[0].position = sf::Vector2f(from->x, from->y);
        finalSegment[0].color = sf::Color::White;
        finalSegment[1].position = sf::Vector2f(to->x, to->y);
        finalSegment[1].color = sf::Color::White;
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

void StaticDisplayMap::updateTextureAll() {
    renderTexture.clear(sf::Color::Black);

    sf::CircleShape circle(pointSize);
    circle.setOrigin(pointSize, pointSize);
    
    for (const auto& [from, edge] : graph) {
        for (const auto& [to, path] : edge) {
            // Si hay puntos intermedios (path), dibujarlos como líneas consecutivas
            if (!path.empty()) {
                Point current = from;
                for (const auto& mid : path) {
                    sf::VertexArray segment(sf::Lines, 2);
                    segment[0].position = sf::Vector2f(current.x, current.y);
                    segment[0].color = sf::Color::Green;
                    segment[1].position = sf::Vector2f(mid.x, mid.y);
                    segment[1].color = sf::Color::Green;
                    renderTexture.draw(segment);
                    current = mid;
                }
                // Último tramo: del último intermedio a `to`
                sf::VertexArray finalSegment(sf::Lines, 2);
                finalSegment[0].position = sf::Vector2f(current.x, current.y);
                finalSegment[0].color = sf::Color::Green;
                finalSegment[1].position = sf::Vector2f(to.x, to.y);
                finalSegment[1].color = sf::Color::Green;
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

int StaticDisplayMap::getSizeNodes() const {
    return sizeNodes;
}
