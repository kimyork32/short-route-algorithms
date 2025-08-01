#include <iostream>
#include "Benchmark.hpp"

/**
 * @file benchmark_main.cpp
 * @brief Main executable for running data structure benchmarks
 */

void print_header() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════════╗
║                    Data Structures Benchmark Suite               ║
║                   Pathfinding Algorithm Optimization             ║
╚═══════════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void print_system_info() {
    std::cout << "System Information:" << std::endl;
    std::cout << "  Compiler: " << __VERSION__ << std::endl;
    std::cout << "  C++ Standard: " << __cplusplus << std::endl;
    std::cout << "  Build Type: ";
    
    #ifdef USE_CUSTOM_CONTAINERS
        std::cout << "Custom Data Structures" << std::endl;
    #elif defined(USE_HYBRID_CONTAINERS)
        std::cout << "Hybrid (Custom + STL)" << std::endl;
    #else
        std::cout << "Standard Library (STL)" << std::endl;
    #endif
    
    std::cout << "  Optimization: ";
    #ifdef NDEBUG
        std::cout << "Release (-O2)" << std::endl;
    #else
        std::cout << "Debug (-g)" << std::endl;
    #endif
    
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    print_header();
    print_system_info();
    
    try {
        ds::benchmark::BenchmarkSuite suite;
        
        if (argc > 1) {
            std::string arg = argv[1];
            
            if (arg == "--hashmap" || arg == "-m") {
                std::cout << "Running HashMap benchmarks only..." << std::endl;
                suite.run_hashmap_benchmarks();
            }
            else if (arg == "--hashset" || arg == "-s") {
                std::cout << "Running HashSet benchmarks only..." << std::endl;
                suite.run_hashset_benchmarks();
            }
            else if (arg == "--priority-queue" || arg == "-p") {
                std::cout << "Running PriorityQueue benchmarks only..." << std::endl;
                suite.run_priority_queue_benchmarks();
            }
            else if (arg == "--pathfinding" || arg == "-f") {
                std::cout << "Running pathfinding simulation benchmark..." << std::endl;
                suite.run_pathfinding_simulation_benchmark();
            }
            else if (arg == "--quick" || arg == "-q") {
                std::cout << "Running quick benchmark suite..." << std::endl;
                suite.run_pathfinding_simulation_benchmark(1000); // Smaller dataset
            }
            else if (arg == "--help" || arg == "-h") {
                std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --hashmap, -m        Run HashMap benchmarks only" << std::endl;
                std::cout << "  --hashset, -s        Run HashSet benchmarks only" << std::endl;
                std::cout << "  --priority-queue, -p Run PriorityQueue benchmarks only" << std::endl;
                std::cout << "  --pathfinding, -f    Run pathfinding simulation only" << std::endl;
                std::cout << "  --quick, -q          Run quick benchmark (small dataset)" << std::endl;
                std::cout << "  --help, -h           Show this help message" << std::endl;
                std::cout << "  (no args)            Run all benchmarks" << std::endl;
                return 0;
            }
            else {
                std::cout << "Unknown option: " << arg << std::endl;
                std::cout << "Use --help for available options" << std::endl;
                return 1;
            }
        } else {
            std::cout << "Running complete benchmark suite..." << std::endl;
            suite.run_all_benchmarks();
        }
        
        std::cout << "\nBenchmark completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Benchmark failed with error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
