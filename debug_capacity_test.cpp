#include "include/data_structures/hash/HashMap.hpp"
#include <iostream>
#include <string>

int main() {
    ds::HashMap<int, std::string> map;
    
    std::cout << "Initial capacity: " << map.bucket_count() << "\n\n";
    
    std::cout << "Step 1: map[1] = \"one\"\n";
    map[1] = "one";
    std::cout << "map[1] = '" << map[1] << "'\n";
    std::cout << "Size: " << map.size() << ", Capacity: " << map.bucket_count() << "\n\n";
    
    std::cout << "Step 2: Check map[1] again\n";
    std::cout << "map[1] = '" << map[1] << "'\n\n";
    
    std::cout << "Step 3: map[2] = \"two\"\n";
    map[2] = "two";
    std::cout << "After inserting [2]:\n";
    std::cout << "map[1] = '" << map[1] << "'\n";
    std::cout << "map[2] = '" << map[2] << "'\n";
    std::cout << "Size: " << map.size() << ", Capacity: " << map.bucket_count() << "\n\n";
    
    std::cout << "Step 4: Using at() method\n";
    try {
        std::cout << "map.at(1) = '" << map.at(1) << "'\n";
        std::cout << "map.at(2) = '" << map.at(2) << "'\n";
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}
