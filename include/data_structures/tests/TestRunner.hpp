#pragma once

#include "../core/Common.hpp"
#include "../core/Hash.hpp"
#include "../hash/HashMap.hpp"
#include "../hash/HashSet.hpp"
#include "../linear/Vector.hpp"
#include "../linear/PriorityQueue.hpp"
#include "../../Point.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <functional>
#include <exception>

/**
 * @file TestRunner.hpp
 * @brief Simple unit testing framework for data structures
 */

namespace ds {
    namespace test {
        
        /**
         * @brief Test result structure
         */
        struct TestResult {
            std::string test_name;
            bool passed;
            std::string error_message;
            double execution_time_ms;
            
            TestResult(const std::string& name, bool success, const std::string& error = "", double time = 0.0)
                : test_name(name), passed(success), error_message(error), execution_time_ms(time) {}
        };
        
        /**
         * @brief Simple test assertion macros
         */
        class TestAssertions {
        public:
            static void assert_true(bool condition, const std::string& message = "Assertion failed") {
                if (!condition) {
                    throw std::runtime_error(message);
                }
            }
            
            static void assert_false(bool condition, const std::string& message = "Assertion failed") {
                assert_true(!condition, message);
            }
            
            template<typename T, typename U>
            static void assert_equal(const T& expected, const U& actual, const std::string& message = "Values not equal") {
                if (!(expected == actual)) {
                    std::ostringstream oss;
                    oss << message << " (expected: " << expected << ", actual: " << actual << ")";
                    throw std::runtime_error(oss.str());
                }
            }
            
            template<typename T, typename U>
            static void assert_not_equal(const T& expected, const U& actual, const std::string& message = "Values are equal") {
                if (expected == actual) {
                    std::ostringstream oss;
                    oss << message << " (both values: " << expected << ")";
                    throw std::runtime_error(oss.str());
                }
            }
            
            static void assert_throws(std::function<void()> func, const std::string& message = "Expected exception not thrown") {
                bool threw = false;
                try {
                    func();
                } catch (...) {
                    threw = true;
                }
                assert_true(threw, message);
            }
        };
        
        /**
         * @brief Test runner class
         */
        class TestRunner {
        private:
            std::vector<TestResult> results_;
            
            template<typename TestFunc>
            TestResult run_single_test(const std::string& test_name, TestFunc test_func) {
                auto start = std::chrono::high_resolution_clock::now();
                
                try {
                    test_func();
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    return TestResult(test_name, true, "", duration.count() / 1000.0);
                } catch (const std::exception& e) {
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    return TestResult(test_name, false, e.what(), duration.count() / 1000.0);
                } catch (...) {
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    return TestResult(test_name, false, "Unknown exception", duration.count() / 1000.0);
                }
            }
            
        public:
            // ========================
            // HASHMAP TESTS
            // ========================
            
            void test_hashmap_basic_operations() {
                auto result = run_single_test("HashMap Basic Operations", []() {
                    ds::HashMap<int, std::string> map;
                    
                    // Test insertion
                    TestAssertions::assert_true(map.empty());
                    TestAssertions::assert_equal(0u, map.size());
                    
                    auto [iter1, inserted1] = map.insert(1, "one");
                    TestAssertions::assert_true(inserted1);
                    TestAssertions::assert_equal(1u, map.size());
                    
                    auto [iter2, inserted2] = map.insert(1, "ONE");
                    TestAssertions::assert_false(inserted2);
                    TestAssertions::assert_equal(1u, map.size());
                    TestAssertions::assert_equal(std::string("ONE"), map.at(1));
                    
                    // Test access
                    map[2] = "two";
                    TestAssertions::assert_equal(2u, map.size());
                    TestAssertions::assert_equal(std::string("two"), map[2]);
                    
                    // Test find
                    auto find_iter = map.find(2);
                    TestAssertions::assert_true(find_iter != map.end());
                    TestAssertions::assert_equal(std::string("two"), find_iter->second);
                    
                    // Test erase
                    TestAssertions::assert_equal(1u, map.erase(2));
                    TestAssertions::assert_equal(0u, map.erase(2));
                    TestAssertions::assert_equal(1u, map.size());
                    
                    // Test contains
                    TestAssertions::assert_true(map.contains(1));
                    TestAssertions::assert_false(map.contains(2));
                });
                
                results_.push_back(result);
            }
            
            void test_hashmap_point_specialization() {
                auto result = run_single_test("HashMap Point Specialization", []() {
                    ds::HashMap<Point, double> map;
                    
                    Point p1{10, 20};
                    Point p2{30, 40};
                    Point p3{10, 20}; // Same as p1
                    
                    map[p1] = 1.5;
                    map[p2] = 2.5;
                    
                    TestAssertions::assert_equal(2u, map.size());
                    TestAssertions::assert_equal(1.5, map[p1]);
                    TestAssertions::assert_equal(2.5, map[p2]);
                    TestAssertions::assert_equal(1.5, map[p3]); // Should find p1
                    
                    // Test that p1 and p3 are treated as the same key
                    map[p3] = 3.5;
                    TestAssertions::assert_equal(2u, map.size()); // Still 2 elements
                    TestAssertions::assert_equal(3.5, map[p1]); // p1 value updated
                });
                
                results_.push_back(result);
            }
            
            void test_hashmap_pathfinding_workflow() {
                auto result = run_single_test("HashMap Pathfinding Workflow", []() {
                    // Simulate typical A* usage
                    ds::HashMap<Point, double> g_scores;
                    ds::HashMap<Point, double> f_scores;
                    ds::HashMap<Point, Point> parent_map;
                    
                    Point start{0, 0};
                    Point goal{10, 10};
                    Point intermediate{5, 5};
                    
                    // Initialize start node
                    g_scores[start] = 0.0;
                    f_scores[start] = 14.14; // Heuristic to goal
                    
                    // Process intermediate node
                    g_scores[intermediate] = 7.07;
                    f_scores[intermediate] = 14.14;
                    parent_map[intermediate] = start;
                    
                    // Process goal
                    g_scores[goal] = 14.14;
                    f_scores[goal] = 14.14;
                    parent_map[goal] = intermediate;
                    
                    // Verify pathfinding data structures
                    TestAssertions::assert_equal(3u, g_scores.size());
                    TestAssertions::assert_equal(3u, f_scores.size());
                    TestAssertions::assert_equal(2u, parent_map.size());
                    
                    // Verify path reconstruction
                    std::vector<Point> path;
                    Point current = goal;
                    while (parent_map.contains(current)) {
                        path.push_back(current);
                        current = parent_map[current];
                    }
                    path.push_back(start);
                    
                    TestAssertions::assert_equal(3u, path.size());
                    TestAssertions::assert_equal(goal, path[0]);
                    TestAssertions::assert_equal(intermediate, path[1]);
                    TestAssertions::assert_equal(start, path[2]);
                });
                
                results_.push_back(result);
            }
            
            // ========================
            // HASHSET TESTS
            // ========================
            
            void test_hashset_basic_operations() {
                auto result = run_single_test("HashSet Basic Operations", []() {
                    ds::HashSet<int> set;
                    
                    TestAssertions::assert_true(set.empty());
                    TestAssertions::assert_equal(0u, set.size());
                    
                    // Test insertion
                    auto [iter1, inserted1] = set.insert(1);
                    TestAssertions::assert_true(inserted1);
                    TestAssertions::assert_equal(1u, set.size());
                    
                    auto [iter2, inserted2] = set.insert(1);
                    TestAssertions::assert_false(inserted2);
                    TestAssertions::assert_equal(1u, set.size());
                    
                    // Test contains
                    TestAssertions::assert_true(set.contains(1));
                    TestAssertions::assert_false(set.contains(2));
                    
                    // Test erase
                    TestAssertions::assert_equal(1u, set.erase(1));
                    TestAssertions::assert_equal(0u, set.erase(1));
                    TestAssertions::assert_true(set.empty());
                });
                
                results_.push_back(result);
            }
            
            void test_hashset_point_closed_set() {
                auto result = run_single_test("HashSet Point Closed Set", []() {
                    ds::HashSet<Point> closed_set;
                    
                    Point p1{1, 1};
                    Point p2{2, 2};
                    Point p3{1, 1}; // Duplicate of p1
                    
                    closed_set.insert(p1);
                    closed_set.insert(p2);
                    
                    TestAssertions::assert_equal(2u, closed_set.size());
                    TestAssertions::assert_true(closed_set.contains(p1));
                    TestAssertions::assert_true(closed_set.contains(p2));
                    TestAssertions::assert_true(closed_set.contains(p3)); // Should find p1
                    
                    // Simulate pathfinding closed set usage
                    std::vector<Point> nodes_to_process = {{3, 3}, {4, 4}, {1, 1}};
                    
                    for (const auto& node : nodes_to_process) {
                        if (!closed_set.contains(node)) {
                            closed_set.insert(node);
                        }
                    }
                    
                    TestAssertions::assert_equal(4u, closed_set.size()); // p1 not inserted again
                });
                
                results_.push_back(result);
            }
            
            // ========================
            // PRIORITY QUEUE TESTS
            // ========================
            
            void test_priority_queue_basic_operations() {
                auto result = run_single_test("PriorityQueue Basic Operations", []() {
                    ds::PriorityQueue<int> pq; // Min-heap by default
                    
                    TestAssertions::assert_true(pq.empty());
                    TestAssertions::assert_equal(0u, pq.size());
                    
                    // Test insertion and ordering
                    pq.push(5);
                    pq.push(2);
                    pq.push(8);
                    pq.push(1);
                    
                    TestAssertions::assert_equal(4u, pq.size());
                    TestAssertions::assert_equal(1, pq.top()); // Min element
                    
                    // Test extraction in sorted order
                    std::vector<int> extracted;
                    while (!pq.empty()) {
                        extracted.push_back(pq.top());
                        pq.pop();
                    }
                    
                    std::vector<int> expected = {1, 2, 5, 8};
                    TestAssertions::assert_equal(expected.size(), extracted.size());
                    for (size_t i = 0; i < expected.size(); ++i) {
                        TestAssertions::assert_equal(expected[i], extracted[i]);
                    }
                });
                
                results_.push_back(result);
            }
            
            void test_priority_queue_pathfinding() {
                auto result = run_single_test("PriorityQueue Pathfinding Usage", []() {
                    // Test with pair<double, Point> as used in pathfinding
                    ds::PriorityQueue<std::pair<double, Point>> open_queue;
                    
                    Point start{0, 0};
                    Point goal{10, 10};
                    Point mid1{5, 3};
                    Point mid2{3, 7};
                    
                    // Insert nodes with f-scores (priority, node)
                    open_queue.push({14.14, goal});   // f=14.14
                    open_queue.push({8.49, mid1});    // f=8.49 (should be extracted first)
                    open_queue.push({10.44, mid2});   // f=10.44
                    open_queue.push({0.0, start});    // f=0.0 (should be extracted first)
                    
                    TestAssertions::assert_equal(4u, open_queue.size());
                    
                    // Extract in priority order (lowest f-score first)
                    auto [f1, node1] = open_queue.top(); open_queue.pop();
                    TestAssertions::assert_equal(0.0, f1);
                    TestAssertions::assert_equal(start, node1);
                    
                    auto [f2, node2] = open_queue.top(); open_queue.pop();
                    TestAssertions::assert_equal(8.49, f2);
                    TestAssertions::assert_equal(mid1, node2);
                    
                    auto [f3, node3] = open_queue.top(); open_queue.pop();
                    TestAssertions::assert_equal(10.44, f3);
                    TestAssertions::assert_equal(mid2, node3);
                    
                    auto [f4, node4] = open_queue.top(); open_queue.pop();
                    TestAssertions::assert_equal(14.14, f4);
                    TestAssertions::assert_equal(goal, node4);
                    
                    TestAssertions::assert_true(open_queue.empty());
                });
                
                results_.push_back(result);
            }
            
            // ========================
            // VECTOR TESTS
            // ========================
            
            void test_vector_basic_operations() {
                auto result = run_single_test("Vector Basic Operations", []() {
                    ds::Vector<int> vec;
                    
                    TestAssertions::assert_true(vec.empty());
                    TestAssertions::assert_equal(0u, vec.size());
                    
                    // Test push_back
                    vec.push_back(1);
                    vec.push_back(2);
                    vec.push_back(3);
                    
                    TestAssertions::assert_equal(3u, vec.size());
                    TestAssertions::assert_equal(1, vec[0]);
                    TestAssertions::assert_equal(2, vec[1]);
                    TestAssertions::assert_equal(3, vec[2]);
                    
                    // Test iterators
                    std::vector<int> std_vec(vec.begin(), vec.end());
                    TestAssertions::assert_equal(3u, std_vec.size());
                    TestAssertions::assert_equal(1, std_vec[0]);
                    TestAssertions::assert_equal(2, std_vec[1]);
                    TestAssertions::assert_equal(3, std_vec[2]);
                    
                    // Test pop_back
                    vec.pop_back();
                    TestAssertions::assert_equal(2u, vec.size());
                    TestAssertions::assert_equal(2, vec.back());
                });
                
                results_.push_back(result);
            }
            
            void test_vector_path_storage() {
                auto result = run_single_test("Vector Path Storage", []() {
                    ds::Vector<Point> path;
                    
                    // Simulate path construction
                    path.push_back({0, 0});
                    path.push_back({1, 1});
                    path.push_back({2, 3});
                    path.push_back({4, 5});
                    path.push_back({10, 10});
                    
                    TestAssertions::assert_equal(5u, path.size());
                    
                    // Test path access
                    TestAssertions::assert_equal(Point(0, 0), path.front());
                    TestAssertions::assert_equal(Point(10, 10), path.back());
                    
                    // Test path iteration
                    double total_distance = 0.0;
                    for (size_t i = 1; i < path.size(); ++i) {
                        double dx = path[i].x - path[i-1].x;
                        double dy = path[i].y - path[i-1].y;
                        total_distance += std::sqrt(dx*dx + dy*dy);
                    }
                    
                    TestAssertions::assert_true(total_distance > 0.0);
                });
                
                results_.push_back(result);
            }
            
            // ========================
            // INTEGRATED TESTS
            // ========================
            
            void test_integrated_pathfinding_simulation() {
                auto result = run_single_test("Integrated Pathfinding Simulation", []() {
                    // Simulate a complete A* algorithm workflow
                    ds::HashMap<Point, double> g_scores;
                    ds::HashMap<Point, double> f_scores;
                    ds::HashMap<Point, Point> parent_map;
                    ds::HashSet<Point> closed_set;
                    ds::PriorityQueue<std::pair<double, Point>> open_queue;
                    
                    Point start{0, 0};
                    Point goal{3, 3};
                    
                    // Initialize
                    g_scores[start] = 0.0;
                    f_scores[start] = 4.24; // Heuristic distance to goal
                    open_queue.push({f_scores[start], start});
                    
                    // Simulate algorithm steps
                    int iterations = 0;
                    while (!open_queue.empty() && iterations < 10) {
                        auto [current_f, current] = open_queue.top();
                        open_queue.pop();
                        
                        if (closed_set.contains(current)) continue;
                        
                        closed_set.insert(current);
                        iterations++;
                        
                        if (current == goal) break;
                        
                        // Simulate neighbor exploration
                        std::vector<Point> neighbors = {
                            {current.x + 1, current.y},
                            {current.x, current.y + 1},
                            {current.x + 1, current.y + 1}
                        };
                        
                        for (const auto& neighbor : neighbors) {
                            if (closed_set.contains(neighbor)) continue;
                            
                            double tentative_g = g_scores[current] + 1.0;
                            
                            auto g_it = g_scores.find(neighbor);
                            if (g_it == g_scores.end() || tentative_g < g_it->second) {
                                g_scores[neighbor] = tentative_g;
                                double h = std::abs(goal.x - neighbor.x) + std::abs(goal.y - neighbor.y);
                                f_scores[neighbor] = tentative_g + h;
                                parent_map[neighbor] = current;
                                open_queue.push({f_scores[neighbor], neighbor});
                            }
                        }
                    }
                    
                    // Verify algorithm completed successfully
                    TestAssertions::assert_true(closed_set.contains(goal));
                    TestAssertions::assert_true(parent_map.contains(goal));
                    
                    // Reconstruct path
                    ds::Vector<Point> path;
                    Point current = goal;
                    while (parent_map.contains(current)) {
                        path.push_back(current);
                        current = parent_map[current];
                    }
                    path.push_back(start);
                    
                    TestAssertions::assert_true(path.size() > 0);
                    TestAssertions::assert_equal(goal, path.front());
                    TestAssertions::assert_equal(start, path.back());
                });
                
                results_.push_back(result);
            }
            
            // ========================
            // TEST RUNNER INTERFACE
            // ========================
            
            void run_all_tests() {
                std::cout << "Running HashMap tests..." << std::endl;
                test_hashmap_basic_operations();
                test_hashmap_point_specialization();
                test_hashmap_pathfinding_workflow();
                
                std::cout << "Running HashSet tests..." << std::endl;
                test_hashset_basic_operations();
                test_hashset_point_closed_set();
                
                std::cout << "Running PriorityQueue tests..." << std::endl;
                test_priority_queue_basic_operations();
                test_priority_queue_pathfinding();
                
                std::cout << "Running Vector tests..." << std::endl;
                test_vector_basic_operations();
                test_vector_path_storage();
                
                std::cout << "Running integrated tests..." << std::endl;
                test_integrated_pathfinding_simulation();
            }
            
            void print_results() {
                size_t passed = 0;
                size_t failed = 0;
                double total_time = 0.0;
                
                std::cout << "\n" << std::string(70, '=') << std::endl;
                std::cout << "TEST RESULTS" << std::endl;
                std::cout << std::string(70, '=') << std::endl;
                
                for (const auto& result : results_) {
                    std::cout << "[" << (result.passed ? "PASS" : "FAIL") << "] ";
                    std::cout << std::left << std::setw(50) << result.test_name;
                    std::cout << " (" << std::fixed << std::setprecision(2) << result.execution_time_ms << "ms)";
                    
                    if (!result.passed) {
                        std::cout << "\n    Error: " << result.error_message;
                    }
                    std::cout << std::endl;
                    
                    if (result.passed) passed++;
                    else failed++;
                    total_time += result.execution_time_ms;
                }
                
                std::cout << std::string(70, '-') << std::endl;
                std::cout << "Total: " << results_.size() << " tests, ";
                std::cout << passed << " passed, " << failed << " failed" << std::endl;
                std::cout << "Total execution time: " << std::fixed << std::setprecision(2) << total_time << "ms" << std::endl;
                
                if (failed > 0) {
                    std::cout << "\n" << failed << " test(s) FAILED!" << std::endl;
                } else {
                    std::cout << "\nAll tests PASSED!" << std::endl;
                }
            }
            
            bool all_tests_passed() const {
                return std::all_of(results_.begin(), results_.end(),
                                 [](const TestResult& r) { return r.passed; });
            }
            
            void clear_results() {
                results_.clear();
            }
        };
        
    } // namespace test
} // namespace ds
