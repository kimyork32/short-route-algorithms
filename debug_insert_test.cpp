#include "include/data_structures/hash/HashMap.hpp"
#include <iostream>
#include <string>

int main() {
    ds::HashMap<int, std::string> map;
    
    std::cout << "=== Testing insert method directly ===\n";
    
    std::cout << "Step 1: Insert using insert method\n";
    auto [iter1, inserted1] = map.insert(1, "HELLO");
    std::cout << "Inserted: " << inserted1 << "\n";
    std::cout << "iter1->second: '" << iter1->second << "'\n";
    std::cout << "Size: " << map.size() << "\n\n";
    
    std::cout << "Step 2: Access using at()\n";
    try {
        std::cout << "map.at(1): '" << map.at(1) << "'\n";
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }
    
    std::cout << "\nStep 3: Access using operator[]\n";
    std::cout << "map[1]: '" << map[1] << "'\n";
    
    std::cout << "\nStep 4: Test another insert\n";
    auto [iter2, inserted2] = map.insert(2, "WORLD");
    std::cout << "Inserted: " << inserted2 << "\n";
    std::cout << "iter2->second: '" << iter2->second << "'\n";
    std::cout << "Size: " << map.size() << "\n";
    
    std::cout << "\nStep 5: Access both values\n";
    std::cout << "map.at(1): '" << map.at(1) << "'\n";
    std::cout << "map.at(2): '" << map.at(2) << "'\n";
    
    return 0;
}
