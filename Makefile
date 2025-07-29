# Compilador y flags para Windows
CXX = g++
CXXFLAGS = -Iinclude -std=c++17 -Wall -Wextra -g
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lgmp -lgmpxx

# Detectar si estamos en Windows
ifeq ($(OS),Windows_NT)
    TARGET = game.exe
    RM = del /Q
    MKDIR = mkdir
    PATH_SEP = \\
else
    TARGET = game
    RM = rm -rf
    MKDIR = mkdir -p
    PATH_SEP = /
endif

# Directorio de fuentes
SRC_DIR = src

# Archivos fuente y objeto
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=build/%.o)

# Regla principal
all: $(TARGET)

# Vincula todos los objetos al ejecutable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compila cada archivo fuente en build/
build/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) build 2>nul || echo.
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ejecutar
run: $(TARGET)
	./$(TARGET)

# Limpiar archivos
clean:
	$(RM) build $(TARGET)

.PHONY: all run clean