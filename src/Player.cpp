#include "../include/Player.hpp"
#include <iostream>

Player::Player() {
    shape.setSize({50.f, 50.f});
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(375.f, 275.f);
}

void Player::handleInput() {
    velocity = {0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) velocity.y -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) velocity.y += speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) velocity.x -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) velocity.x += speed;
}

void Player::update(sf::Time deltaTime) {
    shape.move(velocity * deltaTime.asSeconds());
}

void Player::render(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Player::getBounds() const {
    return shape.getGlobalBounds();
}
