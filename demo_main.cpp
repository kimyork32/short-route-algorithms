#include "data_structures/DataStructures.hpp"
#include <iostream>
#include <chrono>

// Enable custom data structures
#define USE_CUSTOM_CONTAINERS

using namespace ds;

int main() {
    std::cout << "\n=== Custom Data Structures Demo ===\n\n";
    
    // 1. Demonstrate HashMap with Point specialization
    std::cout << "1. HashMap with Point Hash Specialization:\n";
    HashMap<Point, double> distance_map;
    distance_map[{0, 0}] = 0.0;
    distance_map[{1, 1}] = 1.414;
    distance_map[{2, 3}] = 3.606;
    
    std::cout << "   Distance from (0,0) to (1,1): " << distance_map[{1, 1}] << std::endl;
    std::cout << "   Map size: " << distance_map.size() << std::endl;
    
    // 2. Demonstrate HashSet for closed set tracking
    std::cout << "\n2. HashSet for Point Tracking:\n";
    HashSet<Point> visited_points;
    visited_points.insert({0, 0});
    visited_points.insert({1, 1});
    visited_points.insert({2, 3});
    
    std::cout << "   Point (1,1) visited: " << (visited_points.contains({1, 1}) ? "Yes" : "No") << std::endl;
    std::cout << "   Point (5,5) visited: " << (visited_points.contains({5, 5}) ? "Yes" : "No") << std::endl;
    
    // 3. Demonstrate PriorityQueue for pathfinding
    std::cout << "\n3. PriorityQueue for Pathfinding:\n";
    PriorityQueue<std::pair<double, Point>> open_queue;
    open_queue.push({10.5, {0, 0}});
    open_queue.push({5.2, {1, 1}});
    open_queue.push({8.7, {2, 2}});
    
    std::cout << "   Next point to explore: (" << open_queue.top().second.x 
              << "," << open_queue.top().second.y << ") with priority " << open_queue.top().first << std::endl;
    
    // 4. Demonstrate AVL Tree
    std::cout << "\n4. AVL Tree Operations:\n";
    AVLTree<int, std::string> avl_tree;
    avl_tree.insert(50, "Node50");
    avl_tree.insert(30, "Node30");
    avl_tree.insert(70, "Node70");
    avl_tree.insert(20, "Node20");
    avl_tree.insert(80, "Node80");
    
    std::cout << "   AVL Tree size: " << avl_tree.size() << std::endl;
    std::cout << "   Tree height: " << avl_tree.height() << std::endl;
    std::cout << "   Is balanced: " << (avl_tree.is_balanced() ? "Yes" : "No") << std::endl;
    std::cout << "   Contains 70: " << (avl_tree.contains(70) ? "Yes" : "No") << std::endl;
    
    // 5. Demonstrate Red-Black Tree
    std::cout << "\n5. Red-Black Tree Operations:\n";
    RedBlackTree<Point, double> rb_tree;
    rb_tree.insert({0, 0}, 0.0);
    rb_tree.insert({1, 0}, 1.0);
    rb_tree.insert({0, 1}, 1.0);
    rb_tree.insert({1, 1}, 1.414);
    rb_tree.insert({2, 2}, 2.828);
    
    std::cout << "   RB Tree size: " << rb_tree.size() << std::endl;
    std::cout << "   Black height: " << rb_tree.black_height() << std::endl;
    std::cout << "   Is valid RB tree: " << (rb_tree.is_valid_red_black_tree() ? "Yes" : "No") << std::endl;
    
    // 6. Demonstrate Graph with pathfinding
    std::cout << "\n6. Graph with Pathfinding:\n";
    auto grid = create_grid_graph(5, 5, false); // 5x5 grid, 4-connected
    
    std::cout << "   Grid vertices: " << grid.vertex_count() << std::endl;
    std::cout << "   Grid edges: " << grid.edge_count() << std::endl;
    std::cout << "   Is connected: " << (grid.is_connected() ? "Yes" : "No") << std::endl;
    
    // Set Manhattan distance heuristic for A*
    grid.set_heuristic([](const Point& a, const Point& b) -> double {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    });
    
    // Run Dijkstra pathfinding
    Point start{0, 0};
    Point goal{4, 4};
    
    auto dijkstra_result = grid.dijkstra(start, goal);
    std::cout << "   Dijkstra path found: " << (dijkstra_result.path_found ? "Yes" : "No") << std::endl;
    if (dijkstra_result.path_found) {
        std::cout << "   Path length: " << dijkstra_result.total_distance << std::endl;
        std::cout << "   Nodes explored: " << dijkstra_result.nodes_explored << std::endl;
        std::cout << "   Computation time: " << dijkstra_result.computation_time_ms << " ms" << std::endl;
    }
    
    // Run A* pathfinding
    auto astar_result = grid.a_star(start, goal);
    std::cout << "   A* path found: " << (astar_result.path_found ? "Yes" : "No") << std::endl;
    if (astar_result.path_found) {
        std::cout << "   Path length: " << astar_result.total_distance << std::endl;
        std::cout << "   Nodes explored: " << astar_result.nodes_explored << std::endl;
        std::cout << "   Computation time: " << astar_result.computation_time_ms << " ms" << std::endl;
        std::cout << "   A* efficiency: " << (100.0 * astar_result.nodes_explored / dijkstra_result.nodes_explored) << "% of Dijkstra" << std::endl;
    }
    
    // 7. Performance comparison example
    std::cout << "\n7. Performance Comparison:\n";
    const size_t test_size = 10000;
    
    // Custom HashMap performance
    auto start_time = std::chrono::high_resolution_clock::now();
    
    HashMap<Point, double> custom_map;
    for (int i = 0; i < test_size; ++i) {
        custom_map[{i, i}] = static_cast<double>(i);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto custom_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    // STL unordered_map performance
    start_time = std::chrono::high_resolution_clock::now();
    
    std::unordered_map<Point, double, Hash<Point>> stl_map;
    for (int i = 0; i < test_size; ++i) {
        stl_map[{i, i}] = static_cast<double>(i);
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    auto stl_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "   Custom HashMap: " << custom_duration.count() / 1000.0 << " ms" << std::endl;
    std::cout << "   STL unordered_map: " << stl_duration.count() / 1000.0 << " ms" << std::endl;
    
    double speedup = static_cast<double>(stl_duration.count()) / custom_duration.count();
    std::cout << "   Speedup: " << std::fixed << std::setprecision(2) << speedup << "x ";
    if (speedup > 1.0) {
        std::cout << "(Custom is faster)" << std::endl;
    } else {
        std::cout << "(STL is faster)" << std::endl;
    }
    
    // 8. Memory usage comparison
    std::cout << "\n8. Memory Usage Statistics:\n";
    auto custom_stats = custom_map.get_statistics();
    std::cout << "   Custom HashMap load factor: " << std::fixed << std::setprecision(3) << custom_stats.load_factor << std::endl;
    std::cout << "   Average probe distance: " << custom_stats.average_distance << std::endl;
    std::cout << "   Collision count: " << custom_stats.collisions << std::endl;
    
    std::cout << "\n=== Demo Complete ===\n";
    
    return 0;
}
