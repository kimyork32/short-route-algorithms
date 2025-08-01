#pragma once
#include "../GraphAlgorithmBase.hpp"
#include <queue>
#include <unordered_set>

class BestFirstSearch : public GraphAlgorithmBase {
public:
    BestFirstSearch(const GraphStructure* graphPtr) : GraphAlgorithmBase(graphPtr) {}

    PathfindingResult findShortestPath(const Point& startNode, const Point& targetNode) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        PathfindingResult result;
        result.algorithmName = getAlgorithmName();

        if (!nodeExistsInGraph(startNode) || !nodeExistsInGraph(targetNode)) {
            std::cout << "BestFS: Nodo de inicio o destino no existe\n";
            return result;
        }

        if (startNode == targetNode) {
            result.pathFound = true;
            result.optimalPath = {startNode};
            result.totalDistance = 0.0;
            return result;
        }

        std::priority_queue<std::pair<float, Point>,
                            std::vector<std::pair<float, Point>>,
                            std::greater<>> frontier;

        std::unordered_map<Point, Point> parentNodes;
        std::unordered_set<Point> visited;

        auto heuristic = [this, targetNode](const Point& a) {
            return static_cast<float>(calculateEuclideanDistance(a, targetNode));
        };

        frontier.emplace(heuristic(startNode), startNode);

        while (!frontier.empty()) {
            Point currentNode = frontier.top().second;
            frontier.pop();

            if (visited.count(currentNode)) continue;

            visited.insert(currentNode);
            result.nodesExplored++;

            if (currentNode == targetNode) {
                result.pathFound = true;
                break;
            }

            auto it = graph->find(currentNode);
            if (it != graph->end()) {
                for (const auto& [neighbor, _] : it->second) {
                    if (!visited.count(neighbor)) {
                        parentNodes[neighbor] = currentNode;
                        frontier.emplace(heuristic(neighbor), neighbor);
                    }
                }
            }
        }

        if (result.pathFound) {
            result.optimalPath = reconstructOptimalPath(parentNodes, startNode, targetNode);
            result.totalDistance = calculatePathTotalDistance(result.optimalPath);
        } else {
            std::cout << "BestFS: No se encontró camino\n";
        }

        result.parentNodes = std::move(parentNodes);
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        return result;
    }

    std::string getAlgorithmName() const override {
        return "Best-First Search";
    }

    std::string getAlgorithmDescription() const override {
        return "Greedy Best-First Search: elige el nodo más prometedor según la heurística (distancia al objetivo).";
    }
};
