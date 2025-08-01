#include "include/data_structures/hash/HashMap.hpp"
#include <iostream>
#include <string>

int main() {
    ds::HashMap<int, std::string> map;
    
    std::cout << "Test 1: Basic operator[] assignment\n";
    map[1] = "one";
    std::cout << "map[1] = '" << map[1] << "'\n";
    std::cout << "Size: " << map.size() << "\n\n";
    
    std::cout << "Test 2: Second operator[] assignment\n";
    map[2] = "two";
    std::cout << "map[1] = '" << map[1] << "'\n";
    std::cout << "map[2] = '" << map[2] << "'\n";
    std::cout << "Size: " << map.size() << "\n\n";
    
    std::cout << "Test 3: Accessing existing key\n";
    std::cout << "map[1] again = '" << map[1] << "'\n";
    std::cout << "map[2] again = '" << map[2] << "'\n\n";
    
    std::cout << "Test 4: Using at() method\n";
    try {
        std::cout << "map.at(1) = '" << map.at(1) << "'\n";
        std::cout << "map.at(2) = '" << map.at(2) << "'\n";
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}
