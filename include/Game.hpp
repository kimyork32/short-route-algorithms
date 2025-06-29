#pragma once
#include <SFML/Graphics.hpp>
#include "Malla.hpp"
#include "Hud.hpp"
#include "Mode.hpp"


class Game {
public:
    Game();
    void run();

private:

    sf::Vector2f getViewTopLeft() const;
    void processEvents();
    void update();
    void render();
    void calculateRoute();
    void generateMap();

    // VARAIBLES
    const int windowWidth = 1200;
    const int windowHeight = 900;

    const int mapWidth = 10000;
    const int mapHeight = 10000;

    const int pointSize = 3;
    const int sizeNodes = 7000;
    const float cameraSpeed = 10.0f;

    sf::Vector2i firstClick;

    // CONDICIONALES
    Mode mode;
    Algorithm alg;
    Map map;

    bool waitSecordClick = false;
    bool endModeRemoveArist = false;

    // ATRIBUTOS PRINCIPALES
    sf::RenderWindow window;
    StaticDisplayMap grid;
    Hud hud;
    sf::View view;
};
