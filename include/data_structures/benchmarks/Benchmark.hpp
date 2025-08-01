#pragma once

#include "../core/Common.hpp"
#include "../core/Hash.hpp"
#include "../core/Aliases.hpp"
#include "../../Point.hpp"
#include <chrono>
#include <random>
#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <sstream>

/**
 * @file Benchmark.hpp
 * @brief Comprehensive benchmarking system for data structures
 * 
 * This system provides detailed performance comparisons between custom
 * implementations and STL containers, with specific focus on pathfinding
 * workloads using Point-based keys.
 */

namespace ds {
    namespace benchmark {
        
        /**
         * @brief Benchmark result structure
         */
        struct BenchmarkResult {
            std::string test_name;
            std::string container_type;
            size_t data_size;
            double execution_time_ms;
            size_t memory_usage_bytes;
            double throughput_ops_per_sec;
            
            // Additional metrics for hash containers
            double load_factor = 0.0;
            double average_probe_distance = 0.0;
            size_t collision_count = 0;
            
            std::string to_string() const {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(3);
                oss << test_name << " [" << container_type << "] ";
                oss << "Size: " << data_size << ", ";
                oss << "Time: " << execution_time_ms << "ms, ";
                oss << "Throughput: " << throughput_ops_per_sec << " ops/sec";
                
                if (load_factor > 0.0) {
                    oss << ", Load: " << load_factor;
                    oss << ", AvgProbe: " << average_probe_distance;
                    oss << ", Collisions: " << collision_count;
                }
                
                return oss.str();
            }
        };
        
        /**
         * @brief Benchmark suite for comprehensive testing
         */
        class BenchmarkSuite {
        private:
            std::vector<BenchmarkResult> results_;
            std::mt19937 random_generator_;
            
            /**
             * @brief Generate random Point data for testing
             */
            std::vector<Point> generate_point_data(size_t count, int coordinate_range = 10000) {
                std::vector<Point> points;
                points.reserve(count);
                
                std::uniform_int_distribution<int> coord_dist(0, coordinate_range);
                
                for (size_t i = 0; i < count; ++i) {
                    points.emplace_back(coord_dist(random_generator_), coord_dist(random_generator_));
                }
                
                return points;
            }
            
            /**
             * @brief Generate pathfinding-like workload data
             */
            struct PathfindingWorkload {
                std::vector<Point> nodes;
                std::vector<std::pair<Point, double>> distances;
                std::vector<std::pair<Point, Point>> parent_relationships;
                std::vector<std::pair<double, Point>> priority_queue_data;
                
                PathfindingWorkload(size_t node_count, std::mt19937& rng) {
                    std::uniform_int_distribution<int> coord_dist(0, 1000);
                    std::uniform_real_distribution<double> distance_dist(1.0, 100.0);
                    
                    // Generate nodes
                    nodes.reserve(node_count);
                    for (size_t i = 0; i < node_count; ++i) {
                        nodes.emplace_back(coord_dist(rng), coord_dist(rng));
                    }
                    
                    // Generate distances (g-scores)
                    distances.reserve(node_count);
                    for (const auto& node : nodes) {
                        distances.emplace_back(node, distance_dist(rng));
                    }
                    
                    // Generate parent relationships
                    parent_relationships.reserve(node_count / 2);
                    for (size_t i = 1; i < nodes.size(); i += 2) {
                        parent_relationships.emplace_back(nodes[i], nodes[i-1]);
                    }
                    
                    // Generate priority queue data (f-scores with nodes)
                    priority_queue_data.reserve(node_count);
                    for (const auto& node : nodes) {
                        priority_queue_data.emplace_back(distance_dist(rng), node);
                    }
                }
            };
            
            /**
             * @brief Measure execution time of a function
             */
            template<typename Func>
            double measure_execution_time(Func&& func) {
                auto start = std::chrono::high_resolution_clock::now();
                func();
                auto end = std::chrono::high_resolution_clock::now();
                
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                return duration.count() / 1000.0; // Convert to milliseconds
            }
            
        public:
            BenchmarkSuite() : random_generator_(std::random_device{}()) {}
            
            // ========================
            // HASHMAP BENCHMARKS
            // ========================
            
            /**
             * @brief Benchmark HashMap insertions
             */
            template<typename HashMap>
            void benchmark_hashmap_insert(const std::string& container_name, size_t data_size) {
                auto test_data = generate_point_data(data_size);
                
                HashMap map;
                
                double execution_time = measure_execution_time([&]() {
                    for (size_t i = 0; i < test_data.size(); ++i) {
                        map[test_data[i]] = static_cast<double>(i);
                    }
                });
                
                BenchmarkResult result;
                result.test_name = "HashMap Insert";
                result.container_type = container_name;
                result.data_size = data_size;
                result.execution_time_ms = execution_time;
                result.throughput_ops_per_sec = (data_size * 1000.0) / execution_time;
                
                // Get statistics if available (custom containers)
                if constexpr (requires { map.get_statistics(); }) {
                    auto stats = map.get_statistics();
                    result.load_factor = stats.load_factor;
                    result.average_probe_distance = stats.average_distance;
                    result.collision_count = stats.collisions;
                }
                
                results_.push_back(result);
            }
            
            /**
             * @brief Benchmark HashMap lookups
             */
            template<typename HashMap>
            void benchmark_hashmap_lookup(const std::string& container_name, size_t data_size) {
                auto test_data = generate_point_data(data_size);
                
                HashMap map;
                for (size_t i = 0; i < test_data.size(); ++i) {
                    map[test_data[i]] = static_cast<double>(i);
                }
                
                // Shuffle for random access pattern
                std::shuffle(test_data.begin(), test_data.end(), random_generator_);
                
                double sum = 0.0; // Prevent optimization
                double execution_time = measure_execution_time([&]() {
                    for (const auto& point : test_data) {
                        auto it = map.find(point);
                        if (it != map.end()) {
                            sum += it->second;
                        }
                    }
                });
                
                BenchmarkResult result;
                result.test_name = "HashMap Lookup";
                result.container_type = container_name;
                result.data_size = data_size;
                result.execution_time_ms = execution_time;
                result.throughput_ops_per_sec = (data_size * 1000.0) / execution_time;
                
                results_.push_back(result);
                
                // Prevent dead code elimination
                volatile double dummy = sum;
                (void)dummy;
            }
            
            /**
             * @brief Benchmark pathfinding-specific HashMap usage
             */
            template<typename HashMap>
            void benchmark_pathfinding_hashmap(const std::string& container_name, size_t node_count) {
                PathfindingWorkload workload(node_count, random_generator_);
                
                double execution_time = measure_execution_time([&]() {
                    // Simulate A* algorithm usage patterns
                    HashMap g_scores;
                    HashMap f_scores;
                    HashMap parent_map;
                    
                    // Initialize with starting values
                    for (const auto& [node, distance] : workload.distances) {
                        g_scores[node] = distance;
                        f_scores[node] = distance * 1.2; // Add heuristic
                    }
                    
                    // Update parent relationships
                    for (const auto& [child, parent] : workload.parent_relationships) {
                        parent_map[child] = parent;
                    }
                    
                    // Simulate pathfinding updates
                    for (const auto& node : workload.nodes) {
                        auto g_it = g_scores.find(node);
                        if (g_it != g_scores.end()) {
                            double new_g = g_it->second * 0.9;
                            g_scores[node] = new_g;
                            f_scores[node] = new_g * 1.1;
                        }
                    }
                });
                
                BenchmarkResult result;
                result.test_name = "Pathfinding HashMap Usage";
                result.container_type = container_name;
                result.data_size = node_count;
                result.execution_time_ms = execution_time;
                result.throughput_ops_per_sec = (node_count * 3 * 1000.0) / execution_time; // 3 operations per node
                
                results_.push_back(result);
            }
            
            // ========================
            // HASHSET BENCHMARKS
            // ========================
            
            /**
             * @brief Benchmark HashSet insertions and lookups
             */
            template<typename HashSet>
            void benchmark_hashset_operations(const std::string& container_name, size_t data_size) {
                auto test_data = generate_point_data(data_size);
                
                HashSet set;
                
                // Benchmark insertions
                double insert_time = measure_execution_time([&]() {
                    for (const auto& point : test_data) {
                        set.insert(point);
                    }
                });
                
                // Benchmark lookups
                std::shuffle(test_data.begin(), test_data.end(), random_generator_);
                size_t found_count = 0;
                double lookup_time = measure_execution_time([&]() {
                    for (const auto& point : test_data) {
                        if (set.contains(point)) {
                            found_count++;
                        }
                    }
                });
                
                BenchmarkResult insert_result;
                insert_result.test_name = "HashSet Insert";
                insert_result.container_type = container_name;
                insert_result.data_size = data_size;
                insert_result.execution_time_ms = insert_time;
                insert_result.throughput_ops_per_sec = (data_size * 1000.0) / insert_time;
                
                BenchmarkResult lookup_result;
                lookup_result.test_name = "HashSet Lookup";
                lookup_result.container_type = container_name;
                lookup_result.data_size = data_size;
                lookup_result.execution_time_ms = lookup_time;
                lookup_result.throughput_ops_per_sec = (data_size * 1000.0) / lookup_time;
                
                results_.push_back(insert_result);
                results_.push_back(lookup_result);
                
                // Prevent optimization
                volatile size_t dummy = found_count;
                (void)dummy;
            }
            
            // ========================
            // PRIORITY QUEUE BENCHMARKS
            // ========================
            
            /**
             * @brief Benchmark PriorityQueue operations
             */
            template<typename PriorityQueue>
            void benchmark_priority_queue(const std::string& container_name, size_t data_size) {
                PathfindingWorkload workload(data_size, random_generator_);
                
                // Benchmark insertions
                PriorityQueue pq;
                double insert_time = measure_execution_time([&]() {
                    for (const auto& item : workload.priority_queue_data) {
                        pq.push(item);
                    }
                });
                
                // Benchmark extractions
                std::vector<std::pair<double, Point>> extracted;
                extracted.reserve(data_size);
                double extract_time = measure_execution_time([&]() {
                    while (!pq.empty()) {
                        extracted.push_back(pq.top());
                        pq.pop();
                    }
                });
                
                BenchmarkResult insert_result;
                insert_result.test_name = "PriorityQueue Push";
                insert_result.container_type = container_name;
                insert_result.data_size = data_size;
                insert_result.execution_time_ms = insert_time;
                insert_result.throughput_ops_per_sec = (data_size * 1000.0) / insert_time;
                
                BenchmarkResult extract_result;
                extract_result.test_name = "PriorityQueue Pop";
                extract_result.container_type = container_name;
                extract_result.data_size = data_size;
                extract_result.execution_time_ms = extract_time;
                extract_result.throughput_ops_per_sec = (data_size * 1000.0) / extract_time;
                
                results_.push_back(insert_result);
                results_.push_back(extract_result);
            }
            
            // ========================
            // COMPREHENSIVE BENCHMARKS
            // ========================
            
            /**
             * @brief Run all HashMap benchmarks
             */
            void run_hashmap_benchmarks() {
                std::vector<size_t> sizes = {100, 1000, 10000, 50000};
                
                std::cout << "\n=== HashMap Benchmarks ===" << std::endl;
                
                for (size_t size : sizes) {
                    // Custom implementation
                    benchmark_hashmap_insert<ds::HashMap<Point, double>>("Custom HashMap", size);
                    benchmark_hashmap_lookup<ds::HashMap<Point, double>>("Custom HashMap", size);
                    benchmark_pathfinding_hashmap<ds::HashMap<Point, double>>("Custom HashMap", size);
                    
                    // STL implementation
                    benchmark_hashmap_insert<std::unordered_map<Point, double>>("STL unordered_map", size);
                    benchmark_hashmap_lookup<std::unordered_map<Point, double>>("STL unordered_map", size);
                    benchmark_pathfinding_hashmap<std::unordered_map<Point, double>>("STL unordered_map", size);
                }
            }
            
            /**
             * @brief Run all HashSet benchmarks
             */
            void run_hashset_benchmarks() {
                std::vector<size_t> sizes = {100, 1000, 10000, 50000};
                
                std::cout << "\n=== HashSet Benchmarks ===" << std::endl;
                
                for (size_t size : sizes) {
                    // Custom implementation
                    benchmark_hashset_operations<ds::HashSet<Point>>("Custom HashSet", size);
                    
                    // STL implementation
                    benchmark_hashset_operations<std::unordered_set<Point>>("STL unordered_set", size);
                }
            }
            
            /**
             * @brief Run all PriorityQueue benchmarks
             */
            void run_priority_queue_benchmarks() {
                std::vector<size_t> sizes = {100, 1000, 10000, 50000};
                
                std::cout << "\n=== PriorityQueue Benchmarks ===" << std::endl;
                
                for (size_t size : sizes) {
                    // Custom implementation
                    benchmark_priority_queue<ds::PriorityQueue<std::pair<double, Point>>>("Custom PriorityQueue", size);
                    
                    // STL implementation
                    benchmark_priority_queue<std::priority_queue<std::pair<double, Point>, 
                                                               std::vector<std::pair<double, Point>>, 
                                                               std::greater<std::pair<double, Point>>>>("STL priority_queue", size);
                }
            }
            
            /**
             * @brief Run comprehensive pathfinding simulation
             */
            void run_pathfinding_simulation_benchmark(size_t graph_size = 10000) {
                std::cout << "\n=== Pathfinding Simulation Benchmark ===" << std::endl;
                
                PathfindingWorkload workload(graph_size, random_generator_);
                
                // Custom implementation simulation
                double custom_time = measure_execution_time([&]() {
                    using namespace ds::pathfinding;
                    
                    #define USE_CUSTOM_CONTAINERS
                    #include "../core/Aliases.hpp"
                    
                    auto g_scores = create_distance_map(graph_size);
                    auto f_scores = create_distance_map(graph_size);
                    auto parent_map = create_parent_map(graph_size);
                    auto closed_set = create_closed_set(graph_size);
                    auto open_queue = create_pathfinding_queue(graph_size / 10);
                    
                    // Simulate A* algorithm
                    for (const auto& [node, distance] : workload.distances) {
                        g_scores[node] = distance;
                        f_scores[node] = distance * 1.2;
                        open_queue.push({f_scores[node], node});
                    }
                    
                    size_t processed = 0;
                    while (!open_queue.empty() && processed < graph_size / 2) {
                        auto [f_score, current] = open_queue.top();
                        open_queue.pop();
                        
                        if (closed_set.contains(current)) continue;
                        
                        closed_set.insert(current);
                        processed++;
                        
                        // Simulate neighbor processing
                        for (size_t i = 0; i < 4 && i < workload.nodes.size(); ++i) {
                            const Point& neighbor = workload.nodes[i];
                            if (!closed_set.contains(neighbor)) {
                                double tentative_g = g_scores[current] + 1.0;
                                auto g_it = g_scores.find(neighbor);
                                
                                if (g_it == g_scores.end() || tentative_g < g_it->second) {
                                    g_scores[neighbor] = tentative_g;
                                    f_scores[neighbor] = tentative_g + 5.0; // heuristic
                                    parent_map[neighbor] = current;
                                    open_queue.push({f_scores[neighbor], neighbor});
                                }
                            }
                        }
                    }
                });
                
                // STL implementation simulation
                double stl_time = measure_execution_time([&]() {
                    std::unordered_map<Point, double> g_scores;
                    std::unordered_map<Point, double> f_scores;
                    std::unordered_map<Point, Point> parent_map;
                    std::unordered_set<Point> closed_set;
                    std::priority_queue<std::pair<double, Point>, 
                                      std::vector<std::pair<double, Point>>, 
                                      std::greater<std::pair<double, Point>>> open_queue;
                    
                    g_scores.reserve(graph_size);
                    f_scores.reserve(graph_size);
                    parent_map.reserve(graph_size);
                    closed_set.reserve(graph_size);
                    
                    // Same simulation logic as above
                    for (const auto& [node, distance] : workload.distances) {
                        g_scores[node] = distance;
                        f_scores[node] = distance * 1.2;
                        open_queue.push({f_scores[node], node});
                    }
                    
                    size_t processed = 0;
                    while (!open_queue.empty() && processed < graph_size / 2) {
                        auto [f_score, current] = open_queue.top();
                        open_queue.pop();
                        
                        if (closed_set.count(current)) continue;
                        
                        closed_set.insert(current);
                        processed++;
                        
                        for (size_t i = 0; i < 4 && i < workload.nodes.size(); ++i) {
                            const Point& neighbor = workload.nodes[i];
                            if (!closed_set.count(neighbor)) {
                                double tentative_g = g_scores[current] + 1.0;
                                auto g_it = g_scores.find(neighbor);
                                
                                if (g_it == g_scores.end() || tentative_g < g_it->second) {
                                    g_scores[neighbor] = tentative_g;
                                    f_scores[neighbor] = tentative_g + 5.0;
                                    parent_map[neighbor] = current;
                                    open_queue.push({f_scores[neighbor], neighbor});
                                }
                            }
                        }
                    }
                });
                
                BenchmarkResult custom_result;
                custom_result.test_name = "Full Pathfinding Simulation";
                custom_result.container_type = "Custom Implementation";
                custom_result.data_size = graph_size;
                custom_result.execution_time_ms = custom_time;
                custom_result.throughput_ops_per_sec = (graph_size * 1000.0) / custom_time;
                
                BenchmarkResult stl_result;
                stl_result.test_name = "Full Pathfinding Simulation";
                stl_result.container_type = "STL Implementation";
                stl_result.data_size = graph_size;
                stl_result.execution_time_ms = stl_time;
                stl_result.throughput_ops_per_sec = (graph_size * 1000.0) / stl_time;
                
                results_.push_back(custom_result);
                results_.push_back(stl_result);
                
                // Performance comparison
                double speedup = stl_time / custom_time;
                std::cout << "\nPathfinding Simulation Results:" << std::endl;
                std::cout << "Custom: " << custom_time << "ms" << std::endl;
                std::cout << "STL:    " << stl_time << "ms" << std::endl;
                std::cout << "Speedup: " << std::fixed << std::setprecision(2) << speedup << "x";
                if (speedup > 1.0) {
                    std::cout << " (Custom is faster)";
                } else {
                    std::cout << " (STL is faster)";
                }
                std::cout << std::endl;
            }
            
            // ========================
            // TREE BENCHMARKS
            // ========================
            
            void run_avl_tree_benchmarks() {
                std::cout << "Running AVL Tree benchmarks..." << std::endl;
                
                for (size_t size : {1000, 5000, 10000, 50000}) {
                    // Generate test points
                    auto points = generate_test_points(size);
                    
                    // Benchmark AVL tree insertion
                    auto start = std::chrono::high_resolution_clock::now();
                    ds::AVLTree<Point, double> avl_tree;
                    
                    for (size_t i = 0; i < points.size(); ++i) {
                        avl_tree.insert(points[i], static_cast<double>(i));
                    }
                    
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    BenchmarkResult result;
                    result.test_name = "AVL Tree Insert";
                    result.container_type = "AVL";
                    result.data_size = size;
                    result.execution_time_ms = duration.count() / 1000.0;
                    result.throughput_ops_per_sec = size / (result.execution_time_ms / 1000.0);
                    result.memory_usage_bytes = size * (sizeof(Point) + sizeof(double) + 64); // Approximate
                    
                    results_.push_back(result);
                    
                    // Benchmark search operations
                    start = std::chrono::high_resolution_clock::now();
                    
                    for (size_t i = 0; i < points.size(); ++i) {
                        volatile bool found = avl_tree.contains(points[i]);
                        (void)found; // Suppress unused variable warning
                    }
                    
                    end = std::chrono::high_resolution_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    result.test_name = "AVL Tree Search";
                    result.execution_time_ms = duration.count() / 1000.0;
                    result.throughput_ops_per_sec = size / (result.execution_time_ms / 1000.0);
                    
                    results_.push_back(result);
                    
                    // Print tree statistics
                    auto stats = avl_tree.get_statistics();
                    std::cout << "  AVL Tree (size " << size << "): Height=" << stats.tree_height 
                              << ", Avg Depth=" << std::fixed << std::setprecision(2) << stats.average_depth << std::endl;
                }
            }
            
            void run_red_black_tree_benchmarks() {
                std::cout << "Running Red-Black Tree benchmarks..." << std::endl;
                
                for (size_t size : {1000, 5000, 10000, 50000}) {
                    auto points = generate_test_points(size);
                    
                    // Benchmark Red-Black tree insertion
                    auto start = std::chrono::high_resolution_clock::now();
                    ds::RedBlackTree<Point, double> rb_tree;
                    
                    for (size_t i = 0; i < points.size(); ++i) {
                        rb_tree.insert(points[i], static_cast<double>(i));
                    }
                    
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    BenchmarkResult result;
                    result.test_name = "Red-Black Tree Insert";
                    result.container_type = "RB Tree";
                    result.data_size = size;
                    result.execution_time_ms = duration.count() / 1000.0;
                    result.throughput_ops_per_sec = size / (result.execution_time_ms / 1000.0);
                    result.memory_usage_bytes = size * (sizeof(Point) + sizeof(double) + 64);
                    
                    results_.push_back(result);
                    
                    // Benchmark search operations
                    start = std::chrono::high_resolution_clock::now();
                    
                    for (size_t i = 0; i < points.size(); ++i) {
                        volatile bool found = rb_tree.contains(points[i]);
                        (void)found;
                    }
                    
                    end = std::chrono::high_resolution_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    result.test_name = "Red-Black Tree Search";
                    result.execution_time_ms = duration.count() / 1000.0;
                    result.throughput_ops_per_sec = size / (result.execution_time_ms / 1000.0);
                    
                    results_.push_back(result);
                    
                    // Print tree statistics
                    auto stats = rb_tree.get_statistics();
                    std::cout << "  RB Tree (size " << size << "): Black Height=" << stats.black_height 
                              << ", Red Nodes=" << stats.red_node_count 
                              << ", Black Nodes=" << stats.black_node_count << std::endl;
                }
            }
            
            // ========================
            // GRAPH BENCHMARKS
            // ========================
            
            void run_graph_benchmarks() {
                std::cout << "Running Graph benchmarks..." << std::endl;
                
                for (int grid_size : {10, 20, 50, 100}) {
                    size_t vertex_count = grid_size * grid_size;
                    
                    // Create grid graph
                    auto start = std::chrono::high_resolution_clock::now();
                    auto graph = ds::create_grid_graph(grid_size, grid_size, false);
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    BenchmarkResult result;
                    result.test_name = "Grid Graph Creation";
                    result.container_type = "Graph";
                    result.data_size = vertex_count;
                    result.execution_time_ms = duration.count() / 1000.0;
                    result.throughput_ops_per_sec = vertex_count / (result.execution_time_ms / 1000.0);
                    result.memory_usage_bytes = vertex_count * 100; // Approximate
                    
                    results_.push_back(result);
                    
                    // Benchmark Dijkstra pathfinding
                    Point start_point{0, 0};
                    Point goal_point{grid_size-1, grid_size-1};
                    
                    start = std::chrono::high_resolution_clock::now();
                    auto dijkstra_result = graph.dijkstra(start_point, goal_point);
                    end = std::chrono::high_resolution_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    result.test_name = "Dijkstra Pathfinding";
                    result.execution_time_ms = duration.count() / 1000.0;
                    result.throughput_ops_per_sec = dijkstra_result.nodes_explored / (result.execution_time_ms / 1000.0);
                    
                    results_.push_back(result);
                    
                    // Benchmark A* pathfinding with Manhattan distance heuristic
                    graph.set_heuristic([](const Point& a, const Point& b) -> double {
                        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
                    });
                    
                    start = std::chrono::high_resolution_clock::now();
                    auto astar_result = graph.a_star(start_point, goal_point);
                    end = std::chrono::high_resolution_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    result.test_name = "A* Pathfinding";
                    result.execution_time_ms = duration.count() / 1000.0;
                    result.throughput_ops_per_sec = astar_result.nodes_explored / (result.execution_time_ms / 1000.0);
                    
                    results_.push_back(result);
                    
                    std::cout << "  Grid " << grid_size << "x" << grid_size 
                              << ": Dijkstra explored " << dijkstra_result.nodes_explored 
                              << " nodes, A* explored " << astar_result.nodes_explored << " nodes" << std::endl;
                }
            }
            
            /**
             * @brief Run all benchmarks
             */
            void run_all_benchmarks() {
                std::cout << "Running comprehensive data structure benchmarks..." << std::endl;
                std::cout << "Implementation: " << ds::pathfinding::get_implementation_info() << std::endl;
                
                run_hashmap_benchmarks();
                run_hashset_benchmarks();
                run_priority_queue_benchmarks();
                run_avl_tree_benchmarks();
                run_red_black_tree_benchmarks();
                run_graph_benchmarks();
                run_pathfinding_simulation_benchmark();
                
                print_summary();
            }
            
            /**
             * @brief Print benchmark results summary
             */
            void print_summary() {
                std::cout << "\n" << std::string(80, '=') << std::endl;
                std::cout << "BENCHMARK RESULTS SUMMARY" << std::endl;
                std::cout << std::string(80, '=') << std::endl;
                
                // Group results by test type
                std::map<std::string, std::vector<BenchmarkResult>> grouped_results;
                for (const auto& result : results_) {
                    grouped_results[result.test_name].push_back(result);
                }
                
                for (const auto& [test_name, test_results] : grouped_results) {
                    std::cout << "\n" << test_name << ":" << std::endl;
                    std::cout << std::string(50, '-') << std::endl;
                    
                    for (const auto& result : test_results) {
                        std::cout << "  " << result.to_string() << std::endl;
                    }
                    
                    // Calculate speedup if we have both custom and STL results
                    if (test_results.size() >= 2) {
                        auto custom_it = std::find_if(test_results.begin(), test_results.end(),
                            [](const BenchmarkResult& r) { return r.container_type.find("Custom") != std::string::npos; });
                        auto stl_it = std::find_if(test_results.begin(), test_results.end(),
                            [](const BenchmarkResult& r) { return r.container_type.find("STL") != std::string::npos; });
                        
                        if (custom_it != test_results.end() && stl_it != test_results.end() &&
                            custom_it->data_size == stl_it->data_size) {
                            double speedup = stl_it->execution_time_ms / custom_it->execution_time_ms;
                            std::cout << "    Speedup: " << std::fixed << std::setprecision(2) << speedup << "x";
                            if (speedup > 1.0) {
                                std::cout << " (Custom faster)";
                            } else {
                                std::cout << " (STL faster)";
                            }
                            std::cout << std::endl;
                        }
                    }
                }
                
                std::cout << "\n" << std::string(80, '=') << std::endl;
            }
            
            /**
             * @brief Clear all results
             */
            void clear_results() {
                results_.clear();
            }
            
            /**
             * @brief Get all results
             */
            const std::vector<BenchmarkResult>& get_results() const {
                return results_;
            }
        };
        
    } // namespace benchmark
} // namespace ds
