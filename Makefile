# Compilador y flags para Windows
CXX = g++
CXXFLAGS = -Iinclude -Iinclude/data_structures -std=c++17 -Wall -Wextra -g -O2 -DUSE_CUSTOM_CONTAINERS
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lgmp -lgmpxx

# Detectar si estamos en Windows
ifeq ($(OS),Windows_NT)
    TARGET = game.exe
    BENCHMARK_TARGET = benchmark.exe
    TEST_TARGET = test.exe
    RM = del /Q
    MKDIR = mkdir
    PATH_SEP = \\
else
    TARGET = game
    BENCHMARK_TARGET = benchmark
    TEST_TARGET = test
    RM = rm -rf
    MKDIR = mkdir -p
    PATH_SEP = /
endif

# Directorios
SRC_DIR = src
INCLUDE_DIR = include
DS_INCLUDE_DIR = include/data_structures
BUILD_DIR = build

# Archivos fuente y objeto
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Benchmark y test sources
BENCHMARK_SRC = $(DS_INCLUDE_DIR)/benchmarks/benchmark_main.cpp
TEST_SRC = $(DS_INCLUDE_DIR)/tests/test_main.cpp

# Regla principal
all: $(TARGET)

# Target específicos
game: $(TARGET)
benchmark: $(BENCHMARK_TARGET)
test: $(TEST_TARGET)
custom: CXXFLAGS += -DUSE_CUSTOM_CONTAINERS
custom: $(TARGET)
stl: CXXFLAGS += -DUSE_STD_CONTAINERS
stl: $(TARGET)
hybrid: CXXFLAGS += -DUSE_HYBRID_CONTAINERS
hybrid: $(TARGET)

# Vincula todos los objetos al ejecutable principal
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Ejecutable de benchmarks
$(BENCHMARK_TARGET): $(BENCHMARK_SRC)
	@$(MKDIR) $(BUILD_DIR) 2>nul || echo.
	$(CXX) $(CXXFLAGS) $< -o $@

# Ejecutable de tests
$(TEST_TARGET): $(TEST_SRC)
	@$(MKDIR) $(BUILD_DIR) 2>nul || echo.
	$(CXX) $(CXXFLAGS) $< -o $@

# Compila cada archivo fuente en build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(BUILD_DIR) 2>nul || echo.
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ejecutar diferentes versiones
run: $(TARGET)
	./$(TARGET)

run-custom: custom
	./$(TARGET)

run-stl: stl
	./$(TARGET)

run-hybrid: hybrid
	./$(TARGET)

run-benchmark: $(BENCHMARK_TARGET)
	./$(BENCHMARK_TARGET)

run-test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Análisis de rendimiento
perf: custom
	@echo "Running performance analysis..."
	time ./$(TARGET)

# Limpiar archivos
clean:
	$(RM) $(BUILD_DIR) $(TARGET) $(BENCHMARK_TARGET) $(TEST_TARGET)

# Ayuda
help:
	@echo "Available targets:"
	@echo "  all          - Build main game executable"
	@echo "  game         - Build main game executable"
	@echo "  benchmark    - Build benchmark executable"
	@echo "  test         - Build test executable"
	@echo "  custom       - Build with custom data structures"
	@echo "  stl          - Build with STL data structures"
	@echo "  hybrid       - Build with hybrid data structures"
	@echo "  run          - Run main executable"
	@echo "  run-custom   - Build and run with custom structures"
	@echo "  run-stl      - Build and run with STL structures"
	@echo "  run-hybrid   - Build and run with hybrid structures"
	@echo "  run-benchmark- Run benchmarks"
	@echo "  run-test     - Run tests"
	@echo "  perf         - Performance analysis"
	@echo "  clean        - Clean build files"
	@echo "  help         - Show this help"

.PHONY: all game benchmark test custom stl hybrid run run-custom run-stl run-hybrid run-benchmark run-test perf clean help