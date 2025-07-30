#pragma once
#include "../GraphAlgorithmBase.hpp"
#include <queue>
#include <unordered_set>
#include <limits>


class ShortestPathDijkstra : public GraphAlgorithmBase {
public:
    PathfindingResult findShortestPath(const GraphStructure& graph, 
                                     const Point& startNode, 
                                     const Point& targetNode) override {
        
        auto startTime = std::chrono::high_resolution_clock::now();
        PathfindingResult result;
        result.algorithmName = getAlgorithmName();
        
        // ========================
        // VALIDACIÓN INICIAL
        // ========================
        if (!nodeExistsInGraph(graph, startNode) || !nodeExistsInGraph(graph, targetNode)) {
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
        // ESTRUCTURAS DEL ALGORITMO
        // ========================
        
        // Distancias mínimas conocidas desde el origen
        std::unordered_map<Point, double> shortestDistances;
        
        // Nodos padre para reconstruir el camino
        std::unordered_map<Point, Point> parentNodes;
        
        // Nodos ya procesados (con distancia final confirmada)
        std::unordered_set<Point> processedNodes;
        
        // Cola de prioridad: pair<distancia, nodo>
        // std::greater hace que sea un min-heap (menor distancia primero)
        using PriorityQueueElement = std::pair<double, Point>;
        std::priority_queue<PriorityQueueElement, 
                          std::vector<PriorityQueueElement>, 
                          std::greater<PriorityQueueElement>> priorityQueue;
        
        // ========================
        // INICIALIZACIÓN
        // ========================
        shortestDistances[startNode] = 0.0;
        priorityQueue.push({0.0, startNode});
        
        std::cout << "Dijkstra: Iniciando búsqueda desde (" << startNode.x << "," << startNode.y 
                  << ") hacia (" << targetNode.x << "," << targetNode.y << ")" << std::endl;
        
        // ========================
        // ALGORITMO PRINCIPAL
        // ========================
        while (!priorityQueue.empty()) {
            // Extraer el nodo con menor distancia
            auto [currentDistance, currentNode] = priorityQueue.top();
            priorityQueue.pop();
            
            // Si ya procesamos este nodo, skip (puede haber duplicados en la cola)
            if (processedNodes.count(currentNode)) {
                continue;
            }
            
            // Marcar como procesado (distancia final confirmada)
            processedNodes.insert(currentNode);
            result.nodesExplored++;
            
            // ¿Llegamos al destino?
            if (currentNode == targetNode) {
                std::cout << "Dijkstra: ¡Destino encontrado!" << std::endl;
                break;
            }
            
            // Explorar todos los vecinos del nodo actual
            auto nodeIterator = graph.find(currentNode);
            if (nodeIterator != graph.end()) {
                for (const auto& [neighborNode, routeGeometry] : nodeIterator->second) {
                    
                    // Si ya procesamos este vecino, skip
                    if (processedNodes.count(neighborNode)) {
                        continue;
                    }
                    
                    // Calcular nueva distancia pasando por el nodo actual
                    double edgeWeight = calculateRealDistance(currentNode, neighborNode, routeGeometry);
                    double newDistance = currentDistance + edgeWeight;
                    
                    // ¿Encontramos un camino mejor a este vecino?
                    auto distanceIterator = shortestDistances.find(neighborNode);
                    if (distanceIterator == shortestDistances.end() || 
                        newDistance < distanceIterator->second) {
                        
                        // Actualizar distancia y padre
                        shortestDistances[neighborNode] = newDistance;
                        parentNodes[neighborNode] = currentNode;
                        
                        // Agregar a la cola para procesar después
                        priorityQueue.push({newDistance, neighborNode});
                    }
                }
            }
        }
        
        // ========================
        // CONSTRUIR RESULTADO
        // ========================
        auto targetDistanceIterator = shortestDistances.find(targetNode);
        if (targetDistanceIterator != shortestDistances.end()) {
            result.pathFound = true;
            result.totalDistance = targetDistanceIterator->second;
            result.optimalPath = reconstructOptimalPath(parentNodes, startNode, targetNode);
            
            // Verificar que el camino reconstruido sea válido
            if (result.optimalPath.empty()) {
                result.pathFound = false;
                std::cout << "Dijkstra: Error al reconstruir el camino" << std::endl;
            }
        } else {
            std::cout << "Dijkstra: No se encontró camino al destino" << std::endl;
        }
        
        // Guardar información detallada
        result.calculatedDistances = std::move(shortestDistances);
        result.parentNodes = std::move(parentNodes);
        
        // Calcular tiempo de ejecución
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        return result;
    }
    
    std::string getAlgorithmName() const override {
        return "Dijkstra";
    }
    
    std::string getAlgorithmDescription() const override {
        return "Encuentra la ruta más corta explorando nodos en orden de distancia creciente";
    }
};