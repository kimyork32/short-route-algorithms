#include "include/Point.hpp"
#include "include/data_structures/hash/HashMap.hpp"
#include <iostream>

int main() {
    // Test simple del HashMap personalizado
    ds::HashMap<Point, Point> parentNodes;
    
    Point key1(715, 314);
    Point value1(696, 319);
    
    std::cout << "=== TEST HASHMAP PERSONALIZADO ===" << std::endl;
    std::cout << "Insertando: (" << key1.x << "," << key1.y << ") -> (" << value1.x << "," << value1.y << ")" << std::endl;
    
    // Test del operador []
    parentNodes[key1] = value1;
    
    std::cout << "HashMap size después de insertar: " << parentNodes.size() << std::endl;
    
    // Test del find()
    auto iterator = parentNodes.find(key1);
    if (iterator != parentNodes.end()) {
        Point foundValue = iterator->second;
        std::cout << "Valor encontrado con find(): (" << foundValue.x << "," << foundValue.y << ")" << std::endl;
        
        if (foundValue.x == value1.x && foundValue.y == value1.y) {
            std::cout << "✅ CORRECTO: Los valores coinciden" << std::endl;
        } else {
            std::cout << "❌ ERROR: Los valores NO coinciden" << std::endl;
            std::cout << "  Esperado: (" << value1.x << "," << value1.y << ")" << std::endl;
            std::cout << "  Encontrado: (" << foundValue.x << "," << foundValue.y << ")" << std::endl;
        }
    } else {
        std::cout << "❌ ERROR CRÍTICO: find() no encontró la clave que acabamos de insertar" << std::endl;
    }
    
    // Test del operador [] para lectura
    Point readValue = parentNodes[key1];
    std::cout << "Valor leído con operator[]: (" << readValue.x << "," << readValue.y << ")" << std::endl;
    
    if (readValue.x == value1.x && readValue.y == value1.y) {
        std::cout << "✅ CORRECTO: operator[] funciona correctamente" << std::endl;
    } else {
        std::cout << "❌ ERROR: operator[] devuelve valor incorrecto" << std::endl;
        std::cout << "  Esperado: (" << value1.x << "," << value1.y << ")" << std::endl;
        std::cout << "  Encontrado: (" << readValue.x << "," << readValue.y << ")" << std::endl;
    }
    
    // Test múltiples inserciones
    std::cout << "\n=== TEST MÚLTIPLES INSERCIONES ===" << std::endl;
    
    Point keys[] = {
        Point(330, 190),
        Point(353, 204),
        Point(390, 171),
        Point(410, 195)
    };
    
    Point values[] = {
        Point(330, 190),  // startNode es padre de sí mismo
        Point(330, 190),
        Point(353, 204),
        Point(390, 171)
    };
    
    for (int i = 0; i < 4; i++) {
        std::cout << "Insertando: (" << keys[i].x << "," << keys[i].y << ") -> (" << values[i].x << "," << values[i].y << ")" << std::endl;
        parentNodes[keys[i]] = values[i];
        
        // Verificar inmediatamente
        auto found = parentNodes.find(keys[i]);
        if (found != parentNodes.end()) {
            Point foundVal = found->second;
            if (foundVal.x == values[i].x && foundVal.y == values[i].y) {
                std::cout << "  ✅ Verificación exitosa" << std::endl;
            } else {
                std::cout << "  ❌ ERROR: Valor incorrecto después de insertar" << std::endl;
                std::cout << "    Esperado: (" << values[i].x << "," << values[i].y << ")" << std::endl;
                std::cout << "    Encontrado: (" << foundVal.x << "," << foundVal.y << ")" << std::endl;
            }
        } else {
            std::cout << "  ❌ ERROR CRÍTICO: No se pudo encontrar la clave recién insertada" << std::endl;
        }
    }
    
    std::cout << "\nHashMap final size: " << parentNodes.size() << std::endl;
    
    return 0;
}
