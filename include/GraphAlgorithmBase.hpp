#pragma once
#include "Point.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <functional>

// ========================
// ALIAS PARA SIMPLIFICAR
// ========================
using GraphStructure = std::unordered_map<Point, std::vector<std::pair<Point, std::pair<bool, std::vector<Point>>>>>;

// ========================
// RESULTADO DE ALGORITMOS
// ========================
struct PathfindingResult {
    // Resultado principal
    std::vector<Point> optimalPath;         // Camino más corto encontrado
    double totalDistance = 0.0;            // Distancia total del camino
    bool pathFound = false;                 // ¿Se encontró un camino?
    
    // Estadísticas del algoritmo
    int nodesExplored = 0;                  // Cuántos nodos se visitaron
    std::chrono::milliseconds executionTime{0}; // Tiempo de ejecución
    std::string algorithmName;              // Nombre del algoritmo usado
    
    // Información detallada (para visualización avanzada)
    std::unordered_map<Point, double> calculatedDistances;    // Distancias a cada nodo
    std::unordered_map<Point, Point> parentNodes;             // Padres para reconstruir camino
    
    // Constructores
    PathfindingResult() = default;
    PathfindingResult(const PathfindingResult&) = default;
    PathfindingResult& operator=(const PathfindingResult&) = default;
    PathfindingResult(PathfindingResult&&) = default;
    PathfindingResult& operator=(PathfindingResult&&) = default;
    
    // Utilidades
    void clear() {
        optimalPath.clear();
        calculatedDistances.clear();
        parentNodes.clear();
        totalDistance = 0.0;
        pathFound = false;
        nodesExplored = 0;
        executionTime = std::chrono::milliseconds{0};
        algorithmName.clear();
    }
    
    bool isEmpty() const { 
        return !pathFound || optimalPath.empty(); 
    }
    
    // Información para debug
    void printStatistics() const {
        std::cout << "=== " << algorithmName << " Statistics ===" << std::endl;
        std::cout << "Path found: " << (pathFound ? "YES" : "NO") << std::endl;
        if (pathFound) {
            std::cout << "Path length: " << optimalPath.size() << " nodes" << std::endl;
            std::cout << "Total distance: " << totalDistance << std::endl;
        }
        std::cout << "Nodes explored: " << nodesExplored << std::endl;
        std::cout << "Execution time: " << executionTime.count() << "ms" << std::endl;
    }
};

// ========================
// CLASE BASE ABSTRACTA
// ========================
class GraphAlgorithmBase {
public:
    virtual ~GraphAlgorithmBase() = default;
    
    // Método principal que debe implementar cada algoritmo específico
    virtual PathfindingResult findShortestPath(const GraphStructure& graph, 
                                              const Point& startNode, 
                                              const Point& targetNode) = 0;
    
    // Método con verificación de barreras (por defecto llama al método sin barreras)
    virtual PathfindingResult findShortestPathWithBarriers(const GraphStructure& graph, 
                                                         const Point& startNode, 
                                                         const Point& targetNode,
                                                         std::function<bool(const Point&)> isNodeBlocked,
                                                         std::function<bool(const Point&, const Point&)> isEdgeBlocked) {
        // Implementación por defecto que ignora las barreras
        return findShortestPath(graph, startNode, targetNode);
    }
    
    // Información del algoritmo
    virtual std::string getAlgorithmName() const = 0;
    virtual std::string getAlgorithmDescription() const = 0;

protected:
    // ========================
    // UTILIDADES MATEMÁTICAS COMPARTIDAS
    // ========================
    
    /**
     * Calcula distancia euclidiana entre dos puntos
     * Formula: sqrt((x2-x1)² + (y2-y1)²)
     */
    double calculateEuclideanDistance(const Point& from, const Point& to) const {
        double dx = static_cast<double>(to.x - from.x);
        double dy = static_cast<double>(to.y - from.y);
        return std::sqrt(dx * dx + dy * dy);
    }
    
    /**
     * Calcula distancia real siguiendo la geometría de la ruta
     * Maneja la estructura pair<bool, vector<Point>>
     */
    double calculateRealDistance(const Point& from, const Point& to, 
                               const std::pair<bool, std::vector<Point>>& edgeData) const {
        const auto& [hasGeometry, routeGeometry] = edgeData;
        
        if (!hasGeometry || routeGeometry.empty()) {
            return calculateEuclideanDistance(from, to);
        }
        
        double totalDistance = 0.0;
        Point currentPoint = from;
        
        // Sumar distancias de cada segmento de la ruta
        for (const auto& geometryPoint : routeGeometry) {
            totalDistance += calculateEuclideanDistance(currentPoint, geometryPoint);
            currentPoint = geometryPoint;
        }
        
        // Último segmento hasta el destino
        totalDistance += calculateEuclideanDistance(currentPoint, to);
        return totalDistance;
    }
    
    // ========================
    // UTILIDADES DE ALGORITMOS
    // ========================
    
    /**
     * Reconstruye el camino desde el nodo destino hasta el origen
     * usando la tabla de nodos padre
     */
    std::vector<Point> reconstructOptimalPath(const std::unordered_map<Point, Point>& parentNodes,
                                            const Point& startNode, 
                                            const Point& targetNode) const {
        std::vector<Point> path;
        Point currentNode = targetNode;
        
        // Caminar hacia atrás desde destino hasta origen
        while (!(currentNode == startNode)) {
            path.push_back(currentNode);
            
            auto parentIterator = parentNodes.find(currentNode);
            if (parentIterator == parentNodes.end()) {
                // No hay camino válido
                return {};
            }
            currentNode = parentIterator->second;
        }
        
        // Agregar el nodo de inicio
        path.push_back(startNode);
        
        // Invertir para tener el camino desde origen a destino
        std::reverse(path.begin(), path.end());
        return path;
    }
    
    /**
     * Verifica si un nodo existe en el grafo
     */
    bool nodeExistsInGraph(const GraphStructure& graph, const Point& node) const {
        return graph.find(node) != graph.end();
    }
    
    /**
     * Obtiene la distancia entre dos nodos adyacentes en el grafo
     * Retorna -1 si no están conectados directamente
     */
    double getDistanceBetweenAdjacentNodes(const GraphStructure& graph, 
                                         const Point& from, const Point& to) const {
        auto nodeIterator = graph.find(from);
        if (nodeIterator == graph.end()) return -1.0;
        
        for (const auto& [neighbor, edgeData] : nodeIterator->second) {
            if (neighbor == to) {
                return calculateRealDistance(from, to, edgeData);
            }
        }
        
        return -1.0; // No están conectados directamente
    }
    
    /**
     * Calcula la distancia total de un camino completo
     */
    double calculatePathTotalDistance(const GraphStructure& graph, 
                                    const std::vector<Point>& path) const {
        if (path.size() < 2) return 0.0;
        
        double totalDistance = 0.0;
        
        for (size_t i = 0; i < path.size() - 1; ++i) {
            const Point& from = path[i];
            const Point& to = path[i + 1];
            
            // Buscar la arista en el grafo para obtener la geometría
            auto nodeIterator = graph.find(from);
            if (nodeIterator != graph.end()) {
                for (const auto& [neighbor, edgeData] : nodeIterator->second) {
                    if (neighbor == to) {
                        totalDistance += calculateRealDistance(from, to, edgeData);
                        break;
                    }
                }
            }
        }
        
        return totalDistance;
    }
};