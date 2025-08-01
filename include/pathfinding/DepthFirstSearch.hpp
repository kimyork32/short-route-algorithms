#pragma once
#include "../GraphAlgorithmBase.hpp"
#include <unordered_set>
#include <stack>

class DepthFirstSearch : public GraphAlgorithmBase {
public:
    DepthFirstSearch(const GraphStructure* graphPtr) : GraphAlgorithmBase(graphPtr) {}

    PathfindingResult findShortestPath(const Point& startNode, const Point& targetNode) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        PathfindingResult result;
        result.algorithmName = getAlgorithmName();

        if (!nodeExistsInGraph(startNode) || !nodeExistsInGraph(targetNode)) {
            std::cout << "DFS: Nodo de inicio o destino no existe\n";
            return result;
        }

        if (startNode == targetNode) {
            result.pathFound = true;
            result.optimalPath = {startNode};
            result.totalDistance = 0.0;
            return result;
        }

        std::unordered_map<Point, Point> parentNodes;
        std::unordered_set<Point> visited;
        std::stack<Point> stack;

        stack.push(startNode);
        visited.insert(startNode);

        bool found = false;

        while (!stack.empty()) {
            Point currentNode = stack.top();
            stack.pop();
            result.nodesExplored++;

            if (currentNode == targetNode) {
                found = true;
                break;
            }

            auto it = graph->find(currentNode);
            if (it != graph->end()) {
                for (const auto& [neighbor, edgeData] : it->second) {
                    if (visited.count(neighbor) == 0) {
                        visited.insert(neighbor);
                        parentNodes[neighbor] = currentNode;
                        stack.push(neighbor);
                    }
                }
            }
        }

        if (found) {
            result.optimalPath = reconstructOptimalPath(parentNodes, startNode, targetNode);
            result.totalDistance = calculatePathTotalDistance(result.optimalPath);
            result.pathFound = true;
        } else {
            std::cout << "DFS: No se encontró camino\n";
        }

        result.parentNodes = std::move(parentNodes);
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        return result;
    }

    std::string getAlgorithmName() const override {
        return "Depth-First Search";
    }

    std::string getAlgorithmDescription() const override {
        return "Explora lo más profundo posible antes de retroceder.";
    }
};
