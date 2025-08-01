#include "../tests/TestRunner.hpp"
#include "../trees/AVLTree.hpp"
#include "../trees/RedBlackTree.hpp"
#include "../graph/Graph.hpp"

namespace ds {
    namespace test {
        
        // ========================
        // TREE TESTS
        // ========================
        
        class TreeTestRunner : public TestRunner {
        public:
            
            void test_avl_tree_operations() {
                auto result = run_single_test("AVL Tree Operations", []() {
                    ds::AVLTree<int, std::string> tree;
                    
                    // Test insertion and balancing
                    TestAssertions::assert_true(tree.empty());
                    
                    auto [it1, inserted1] = tree.insert(50, "fifty");
                    TestAssertions::assert_true(inserted1);
                    TestAssertions::assert_equal(1u, tree.size());
                    
                    // Insert in ascending order to trigger rotations
                    tree.insert(60, "sixty");
                    tree.insert(70, "seventy");
                    tree.insert(80, "eighty");
                    tree.insert(90, "ninety");
                    
                    TestAssertions::assert_equal(5u, tree.size());
                    TestAssertions::assert_true(tree.is_balanced());
                    
                    // Test search
                    TestAssertions::assert_true(tree.contains(70));
                    TestAssertions::assert_false(tree.contains(100));
                    
                    // Test deletion
                    TestAssertions::assert_equal(1u, tree.erase(60));
                    TestAssertions::assert_equal(4u, tree.size());
                    TestAssertions::assert_false(tree.contains(60));
                });
                results_.push_back(result);
            }
            
            void test_red_black_tree_operations() {
                auto result = run_single_test("Red-Black Tree Operations", []() {
                    ds::RedBlackTree<int, std::string> tree;
                    
                    // Test insertion
                    TestAssertions::assert_true(tree.empty());
                    
                    auto [it1, inserted1] = tree.insert(10, "ten");
                    TestAssertions::assert_true(inserted1);
                    TestAssertions::assert_equal(1u, tree.size());
                    
                    // Insert multiple values
                    tree.insert(20, "twenty");
                    tree.insert(30, "thirty");
                    tree.insert(15, "fifteen");
                    tree.insert(25, "twentyfive");
                    
                    TestAssertions::assert_equal(5u, tree.size());
                    TestAssertions::assert_true(tree.is_valid_red_black_tree());
                    
                    // Test search
                    TestAssertions::assert_true(tree.contains(20));
                    TestAssertions::assert_false(tree.contains(35));
                    
                    // Test access
                    TestAssertions::assert_equal(std::string("fifteen"), tree.at(15));
                });
                results_.push_back(result);
            }
            
            void test_tree_balancing() {
                auto result = run_single_test("Tree Balancing Verification", []() {
                    ds::AVLTree<Point, double> avl_tree;
                    ds::RedBlackTree<Point, double> rb_tree;
                    
                    // Insert many points to test balancing
                    for (int x = 0; x < 10; ++x) {
                        for (int y = 0; y < 10; ++y) {
                            Point p{x, y};
                            double distance = x * x + y * y;
                            
                            avl_tree.insert(p, distance);
                            rb_tree.insert(p, distance);
                        }
                    }
                    
                    TestAssertions::assert_equal(100u, avl_tree.size());
                    TestAssertions::assert_equal(100u, rb_tree.size());
                    
                    // Check balancing properties
                    TestAssertions::assert_true(avl_tree.is_balanced());
                    TestAssertions::assert_true(rb_tree.is_valid_red_black_tree());
                    
                    // Verify height efficiency
                    int avl_height = avl_tree.height();
                    int rb_height = rb_tree.black_height();
                    
                    // For 100 nodes, height should be logarithmic
                    TestAssertions::assert_true(avl_height < 15); // log2(100) ≈ 6.6, AVL allows 1.44*log2(n)
                    TestAssertions::assert_true(rb_height < 15);  // Similar bound for RB tree
                });
                results_.push_back(result);
            }
            
            // ========================
            // GRAPH TESTS
            // ========================
            
            void test_graph_basic_operations() {
                auto result = run_single_test("Graph Basic Operations", []() {
                    ds::Graph<int, double> graph;
                    
                    // Test vertex operations
                    TestAssertions::assert_true(graph.empty());
                    TestAssertions::assert_equal(0u, graph.vertex_count());
                    
                    TestAssertions::assert_true(graph.add_vertex(1));
                    TestAssertions::assert_false(graph.add_vertex(1)); // duplicate
                    TestAssertions::assert_equal(1u, graph.vertex_count());
                    
                    // Test edge operations
                    graph.add_vertex(2);
                    TestAssertions::assert_true(graph.add_edge(1, 2, 5.0));
                    TestAssertions::assert_equal(1u, graph.edge_count());
                    TestAssertions::assert_true(graph.contains_edge(1, 2));
                    TestAssertions::assert_equal(5.0, graph.get_edge_weight(1, 2));
                    
                    // Test undirected graph (should add reverse edge)
                    if (!graph.is_directed()) {
                        TestAssertions::assert_true(graph.contains_edge(2, 1));
                        TestAssertions::assert_equal(2u, graph.edge_count()); // forward + backward
                    }
                });
                results_.push_back(result);
            }
            
            void test_graph_pathfinding_algorithms() {
                auto result = run_single_test("Graph Pathfinding Algorithms", []() {
                    // Create simple graph: 1 -> 2 -> 3
                    //                          \-> 4
                    ds::Graph<int, double> graph;
                    graph.add_edge(1, 2, 1.0);
                    graph.add_edge(2, 3, 1.0);
                    graph.add_edge(2, 4, 2.0);
                    graph.add_edge(1, 4, 5.0); // longer direct path
                    
                    // Test Dijkstra
                    auto dijkstra_result = graph.dijkstra(1, 4);
                    TestAssertions::assert_true(dijkstra_result.path_found);
                    TestAssertions::assert_equal(3.0, dijkstra_result.total_distance); // 1->2->4
                    TestAssertions::assert_equal(3u, dijkstra_result.path.size());
                    
                    // Test A* (without heuristic, should behave like Dijkstra)
                    auto astar_result = graph.a_star(1, 4);
                    TestAssertions::assert_true(astar_result.path_found);
                    TestAssertions::assert_equal(3.0, astar_result.total_distance);
                    
                    // Test unreachable path
                    graph.add_vertex(5); // isolated vertex
                    auto unreachable_result = graph.dijkstra(1, 5);
                    TestAssertions::assert_false(unreachable_result.path_found);
                });
                results_.push_back(result);
            }
            
            void test_grid_graph_creation() {
                auto result = run_single_test("Grid Graph Creation", []() {
                    // Create 3x3 grid
                    auto grid = ds::create_grid_graph(3, 3, false); // 4-connected
                    
                    TestAssertions::assert_equal(9u, grid.vertex_count());
                    TestAssertions::assert_true(grid.contains_vertex({0, 0}));
                    TestAssertions::assert_true(grid.contains_vertex({2, 2}));
                    
                    // Test 4-connectivity
                    TestAssertions::assert_true(grid.contains_edge({1, 1}, {1, 0})); // up
                    TestAssertions::assert_true(grid.contains_edge({1, 1}, {1, 2})); // down
                    TestAssertions::assert_true(grid.contains_edge({1, 1}, {0, 1})); // left
                    TestAssertions::assert_true(grid.contains_edge({1, 1}, {2, 1})); // right
                    
                    // No diagonal connections
                    TestAssertions::assert_false(grid.contains_edge({1, 1}, {0, 0}));
                    
                    // Test pathfinding on grid
                    auto path_result = grid.dijkstra({0, 0}, {2, 2});
                    TestAssertions::assert_true(path_result.path_found);
                    TestAssertions::assert_equal(4.0, path_result.total_distance); // Manhattan distance
                });
                results_.push_back(result);
            }
            
            void run_tree_and_graph_tests() {
                std::cout << "\n=== Tree and Graph Data Structures Test Suite ===\n\n";
                results_.clear();
                
                // Tree tests
                test_avl_tree_operations();
                test_red_black_tree_operations();
                test_tree_balancing();
                
                // Graph tests
                test_graph_basic_operations();
                test_graph_pathfinding_algorithms();
                test_grid_graph_creation();
                
                print_summary();
            }
        };
        
    } // namespace test
} // namespace ds
