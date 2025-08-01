#pragma once
#include "../GraphAlgorithmBase.hpp"
#include <queue>
#include <unordered_set>


class ShortestPathAStar : public GraphAlgorithmBase {
public:
    ShortestPathAStar(const GraphStructure* graphPtr) : GraphAlgorithmBase(graphPtr) {}

    PathfindingResult findShortestPath(const Point& startNode, 
                                     const Point& targetNode) override {
        
        auto startTime = std::chrono::high_resolution_clock::now();
        PathfindingResult result;
        result.algorithmName = getAlgorithmName();
        
        // ========================
        // VALIDACIÓN INICIAL
        // ========================
        if (!nodeExistsInGraph(startNode) || !nodeExistsInGraph(targetNode)) {
            std::cout << "Error: Nodo de inicio o destino no existe en el grafo" << std::endl;
            return result;
        }
        
        if (startNode == targetNode) {
            result.pathFound = true;
            result.optimalPath = {startNode};
            result.totalDistance = 0.0;
            return result;
        }
        
        // ========================
        // ESTRUCTURAS DEL ALGORITMO A*
        // ========================
        
        // g(n): Costo real desde el origen hasta n
        std::unordered_map<Point, double> gScore;
        
        // f(n): g(n) + h(n) - estimación total del costo pasando por n
        std::unordered_map<Point, double> fScore;
        
        // Nodos padre para reconstruir el camino
        std::unordered_map<Point, Point> parentNodes;
        
        // Nodos ya procesados completamente
        std::unordered_set<Point> closedSet;
        
        // Cola de prioridad ordenada por f(n): pair<fScore, nodo>
        using PriorityQueueElement = std::pair<double, Point>;
        std::priority_queue<PriorityQueueElement, 
                          std::vector<PriorityQueueElement>, 
                          std::greater<PriorityQueueElement>> openSet;
        
        // ========================
        // INICIALIZACIÓN
        // ========================
        gScore[startNode] = 0.0;
        fScore[startNode] = calculateHeuristic(startNode, targetNode);
        openSet.push({fScore[startNode], startNode});
        
        std::cout << "A*: Iniciando búsqueda desde (" << startNode.x << "," << startNode.y 
                  << ") hacia (" << targetNode.x << "," << targetNode.y << ")" << std::endl;
        std::cout << "A*: Heurística inicial: " << fScore[startNode] << std::endl;
        
        // ========================
        // ALGORITMO PRINCIPAL A*
        // ========================
        while (!openSet.empty()) {
            // Extraer el nodo con menor f(n)
            auto [currentF, currentNode] = openSet.top();
            openSet.pop();
            
            // Si ya procesamos este nodo, skip
            if (closedSet.count(currentNode)) {
                continue;
            }
            
            // Marcar como procesado
            closedSet.insert(currentNode);
            result.nodesExplored++;
            
            // ¿Llegamos al destino?
            if (currentNode == targetNode) {
                std::cout << "A*: ¡Destino encontrado!" << std::endl;
                break;
            }
            
            // Explorar vecinos
            auto nodeIterator = (*graph).find(currentNode);
            if (nodeIterator != (*graph).end()) {
                for (const auto& [neighborNode, edgeData] : nodeIterator->second) {
                    
                    // Si ya procesamos este vecino, skip
                    if (closedSet.count(neighborNode)) {
                        continue;
                    }
                    
                    // Calcular g(n) tentativo para el vecino
                    double edgeWeight = calculateRealDistance(currentNode, neighborNode, edgeData);
                    double tentativeGScore = gScore[currentNode] + edgeWeight;
                    
                    // ¿Es este un camino mejor al vecino?
                    auto gScoreIterator = gScore.find(neighborNode);
                    if (gScoreIterator == gScore.end() || tentativeGScore < gScoreIterator->second) {
                        
                        // Actualizar scores
                        parentNodes[neighborNode] = currentNode;
                        gScore[neighborNode] = tentativeGScore;
                        
                        // f(n) = g(n) + h(n)
                        double heuristicValue = calculateHeuristic(neighborNode, targetNode);
                        fScore[neighborNode] = tentativeGScore + heuristicValue;
                        
                        // Agregar a la cola para explorar
                        openSet.push({fScore[neighborNode], neighborNode});
                    }
                }
            }
        }
        
        // ========================
        // CONSTRUIR RESULTADO
        // ========================
        auto targetGScoreIterator = gScore.find(targetNode);
        if (targetGScoreIterator != gScore.end()) {
            result.pathFound = true;
            result.totalDistance = targetGScoreIterator->second;
            result.optimalPath = reconstructOptimalPath(parentNodes, startNode, targetNode);
            
            if (result.optimalPath.empty()) {
                result.pathFound = false;
                std::cout << "A*: Error al reconstruir el camino" << std::endl;
            }
        } else {
            std::cout << "A*: No se encontró camino al destino" << std::endl;
        }
        
        // Guardar información detallada (usar gScore como distancias)
        result.calculatedDistances = std::move(gScore);
        result.parentNodes = std::move(parentNodes);
        
        // Calcular tiempo de ejecución
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        return result;
    }
    
    std::string getAlgorithmName() const override {
        return "A* (A-Star)";
    }
    
    std::string getAlgorithmDescription() const override {
        return "Optimización de Dijkstra que usa heurística para dirigir la búsqueda hacia el objetivo";
    }



private:
  
    double calculateHeuristic(const Point& from, const Point& target) const {
        return calculateEuclideanDistance(from, target);
    }
};
