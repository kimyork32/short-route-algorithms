# Compilador y flags
CXX = g++
CXXFLAGS = -I/usr/local/include -Iinclude -std=c++17 -Wall -Wextra -g
LDFLAGS = -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system

# Directorio de fuentes
SRC_DIR =  src/

# Archivos fuente y objeto
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=build/%.o)

# Ejecutable
TARGET = game

# Regla principal
all: $(TARGET)

# Vincula todos los objetos al ejecutable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compila cada archivo fuente en build/
build/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ejecutar
run: $(TARGET)
	LD_LIBRARY_PATH=/usr/local/lib ./$(TARGET)

# Limpiar archivos
clean:
	rm -rf build $(TARGET)
