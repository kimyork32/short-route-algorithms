#pragma once
#include <SFML/Graphics.hpp>
#include "Malla.hpp"

class Game {
public:
    Game();
    void run();

private:
    bool processEvents();
    void update();
    void render();

    // const int width = 1920;
    // const int height = 1050;
    const int windowWidth = 900;
    const int windowHeight = 900;

    const int mapWidth = 6000;
    const int mapHeight = 6000;

    const int pointSize = 1;
    const int cellSize = 1;

    sf::RenderWindow window;
    StaticDisplayMap grid;
    bool waitSecordClick = false;
    sf::Vector2i firstClick;
    sf::View view;
    const float cameraSpeed = 10.0f;
};
