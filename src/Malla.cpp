#include "../include/Malla.hpp"
#include <iostream>
#include <random>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef K::Point_2 CGALPoint;

StaticDisplayMap::StaticDisplayMap(int width, int height, int size)
    : width(width), height(height), pointSize(size) {
    genRandGraph();
}

void StaticDisplayMap::insertPoint(Point node) {
    if (node.x < 0 || node.x >= width || node.y < 0 || node.y >= height) return;
    graph[node];
}

void StaticDisplayMap::insertEdge(Point from, Point to) {
    graph[from].push_back({to, {}});
}

void StaticDisplayMap::genRandGraph() {
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Delaunay = CGAL::Delaunay_triangulation_2<Kernel>;
    using CGALPoint = Kernel::Point_2;

    const int n = 7000;

    // Generador aleatorio
    std::random_device rd;
    std::mt19937 gen(rd());
    // std::uniform_int_distribution<> distrib(0, coordMax);
    std::uniform_int_distribution<> distribX(0, width - 1);
    std::uniform_int_distribution<> distribY(0, height - 1);

    std::vector<CGALPoint> puntos;

    for (int i = 0; i < n; ++i) {
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
}

void StaticDisplayMap::render(sf::RenderWindow& window) {
    sf::CircleShape circle(3);
    circle.setOrigin(3, 3);
    
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
                    window.draw(segment);
                    current = mid;
                }
                // Último tramo: del último intermedio a `to`
                sf::VertexArray finalSegment(sf::Lines, 2);
                finalSegment[0].position = sf::Vector2f(current.x, current.y);
                finalSegment[0].color = sf::Color::Green;
                finalSegment[1].position = sf::Vector2f(to.x, to.y);
                finalSegment[1].color = sf::Color::Green;
                window.draw(finalSegment);
            }
            else {
                // Dibujar línea directa de from a to
                sf::VertexArray line(sf::Lines, 2);
                line[0].position = sf::Vector2f(from.x, from.y);
                line[0].color = sf::Color::White;
                line[1].position = sf::Vector2f(to.x, to.y);
                line[1].color = sf::Color::White;
                window.draw(line);
            }
            circle.setPosition(to.x, to.y);
            circle.setFillColor(sf::Color::Red);
            window.draw(circle);
        }
        circle.setPosition(from.x, from.y);
        circle.setFillColor(sf::Color::Red);
        window.draw(circle);
    }
}
