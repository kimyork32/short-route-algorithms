#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    Player();
    void handleInput();
    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float speed = 200.f;
};
