#include "include/data_structures/hash/HashMap.hpp"
#include <iostream>
#include <string>

int main() {
    ds::HashMap<int, std::string> map;
    
    std::cout << "=== Debug HashMap Test ===\n";
    
    std::cout << "Step 1: Insert first element\n";
    map[1] = "one";
    std::cout << "map[1] = '" << map[1] << "'\n";
    std::cout << "Size: " << map.size() << "\n";
    std::cout << "Empty: " << (map.empty() ? "true" : "false") << "\n\n";
    
    std::cout << "Step 2: Insert second element\n";
    map[2] = "two";
    std::cout << "map[1] = '" << map[1] << "'\n";
    std::cout << "map[2] = '" << map[2] << "'\n";
    std::cout << "Size: " << map.size() << "\n\n";
    
    std::cout << "Step 3: Test at() method\n";
    try {
        std::cout << "map.at(1) = '" << map.at(1) << "'\n";
        std::cout << "map.at(2) = '" << map.at(2) << "'\n";
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }
    
    std::cout << "\nStep 4: Test find() method\n";
    auto it1 = map.find(1);
    auto it2 = map.find(2);
    std::cout << "find(1) " << (it1 != map.end() ? "found" : "not found") << "\n";
    std::cout << "find(2) " << (it2 != map.end() ? "found" : "not found") << "\n";
    
    if (it1 != map.end()) {
        std::cout << "it1->second = '" << it1->second << "'\n";
    }
    if (it2 != map.end()) {
        std::cout << "it2->second = '" << it2->second << "'\n";
    }
    
    return 0;
}
