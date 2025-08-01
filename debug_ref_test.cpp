#include "include/data_structures/hash/HashMap.hpp"
#include <iostream>
#include <string>

int main() {
    ds::HashMap<int, std::string> map;
    
    std::cout << "=== Testing operator[] step by step ===\n";
    
    std::cout << "Step 1: Get reference to map[1]\n";
    std::string& ref = map[1];  // This should insert empty string
    std::cout << "Reference value: '" << ref << "'\n";
    std::cout << "Size after access: " << map.size() << "\n\n";
    
    std::cout << "Step 2: Assign to reference\n";
    ref = "HELLO";
    std::cout << "After assignment - ref: '" << ref << "'\n";
    std::cout << "After assignment - map[1]: '" << map[1] << "'\n";
    std::cout << "Size: " << map.size() << "\n\n";
    
    std::cout << "Step 3: Verify persistence\n";
    std::cout << "map[1] again: '" << map[1] << "'\n";
    std::cout << "ref again: '" << ref << "'\n\n";
    
    std::cout << "Step 4: Test at() method\n";
    try {
        std::cout << "map.at(1): '" << map.at(1) << "'\n";
    } catch (const std::exception& e) {
        std::cout << "Exception in at(): " << e.what() << "\n";
    }
    
    return 0;
}
