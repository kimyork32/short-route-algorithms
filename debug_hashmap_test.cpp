#include "include/data_structures/DataStructures.hpp"
#include "include/Point.hpp"
#include <iostream>

int main() {
    std::cout << "=== TEST HASHMAP PERSONALIZADO ===" << std::endl;
    
    // Test 1: Crear HashMap y insertar elementos
    ds::HashMap<Point, Point> parentNodes(16);
    
    Point parent(672, 316);
    Point child(690, 288);
    
    std::cout << "Insertando: (" << child.x << "," << child.y << ") -> (" << parent.x << "," << parent.y << ")" << std::endl;
    parentNodes[child] = parent;
    
    std::cout << "Tamaño después de insertar: " << parentNodes.size() << std::endl;
    
    // Test 2: Buscar el elemento
    std::cout << "\n=== BUSCANDO ELEMENTO ===" << std::endl;
    auto iterator = parentNodes.find(child);
    
    if (iterator != parentNodes.end()) {
        Point foundParent = iterator->second;
        std::cout << "✅ ENCONTRADO: (" << child.x << "," << child.y << ") -> (" 
                  << foundParent.x << "," << foundParent.y << ")" << std::endl;
    } else {
        std::cout << "❌ NO ENCONTRADO: (" << child.x << "," << child.y << ")" << std::endl;
    }
    
    // Test 3: Verificar hash functions
    std::cout << "\n=== TEST HASH FUNCTIONS ===" << std::endl;
    ds::Hash<Point> hasher;
    size_t hash1 = hasher(child);
    size_t hash2 = hasher(Point(690, 288));
    
    std::cout << "Hash de Point(690,288) original: " << hash1 << std::endl;
    std::cout << "Hash de Point(690,288) nuevo:    " << hash2 << std::endl;
    std::cout << "¿Hashes iguales? " << (hash1 == hash2 ? "SÍ" : "NO") << std::endl;
    
    // Test 4: Verificar operador ==
    std::cout << "\n=== TEST OPERADOR == ===" << std::endl;
    Point test1(690, 288);
    Point test2(690, 288);
    std::cout << "Point(690,288) == Point(690,288): " << (test1 == test2 ? "SÍ" : "NO") << std::endl;
    std::cout << "child == test1: " << (child == test1 ? "SÍ" : "NO") << std::endl;
    
    // Test 5: Iterar sobre todos los elementos
    std::cout << "\n=== ELEMENTOS EN EL HASHMAP ===" << std::endl;
    for (const auto& [key, value] : parentNodes) {
        std::cout << "Clave: (" << key.x << "," << key.y << ") -> Valor: (" << value.x << "," << value.y << ")" << std::endl;
    }
    
    return 0;
}
