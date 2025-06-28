#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    sf::RenderWindow window;
    Player player;
    const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);
};
