#include <iostream>
#include "TestRunner.hpp"

/**
 * @file test_main.cpp
 * @brief Main executable for running unit tests
 */

void print_header() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════════╗
║                      Data Structures Test Suite                  ║
║                   Unit Tests for Custom Implementations          ║
╚═══════════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void print_system_info() {
    std::cout << "Test Configuration:" << std::endl;
    std::cout << "  Implementation: ";
    
    #ifdef USE_CUSTOM_CONTAINERS
        std::cout << "Custom Data Structures" << std::endl;
    #elif defined(USE_HYBRID_CONTAINERS)
        std::cout << "Hybrid (Custom + STL)" << std::endl;
    #else
        std::cout << "Standard Library (STL)" << std::endl;
    #endif
    
    std::cout << "  Build Type: ";
    #ifdef NDEBUG
        std::cout << "Release" << std::endl;
    #else
        std::cout << "Debug" << std::endl;
    #endif
    
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    print_header();
    print_system_info();
    
    try {
        ds::test::TestRunner runner;
        
        if (argc > 1) {
            std::string arg = argv[1];
            
            if (arg == "--help" || arg == "-h") {
                std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --help, -h           Show this help message" << std::endl;
                std::cout << "  (no args)            Run all tests" << std::endl;
                return 0;
            }
        }
        
        std::cout << "Running all unit tests..." << std::endl;
        runner.run_all_tests();
        runner.print_results();
        
        return runner.all_tests_passed() ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Test suite failed with error: " << e.what() << std::endl;
        return 1;
    }
}
