#include <iostream>
#include <string>
#include "include/data_structures/hash/HashMap.hpp"

int main() {
    ds::HashMap<int, std::string> map;
    
    std::cout << "Inserting (1, \"one\")..." << std::endl;
    auto [iter1, inserted1] = map.insert(1, "one");
    std::cout << "Inserted: " << inserted1 << ", Size: " << map.size() << std::endl;
    
    std::cout << "Trying to access map.at(1)..." << std::endl;
    try {
        std::string value = map.at(1);
        std::cout << "Value at key 1: '" << value << "'" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << "Inserting (1, \"ONE\") to update..." << std::endl;
    auto [iter2, inserted2] = map.insert(1, "ONE");
    std::cout << "Inserted: " << inserted2 << ", Size: " << map.size() << std::endl;
    
    std::cout << "Trying to access map.at(1) again..." << std::endl;
    try {
        std::string value = map.at(1);
        std::cout << "Value at key 1: '" << value << "'" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}
