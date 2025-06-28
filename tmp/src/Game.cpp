#include "../include/Game.hpp"
#include <iostream>

Game::Game() : window(sf::VideoMode(800, 600), "SFML Game") {
    window.setFramerateLimit(60);
}

void Game::run() {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;


    while (window.isOpen()) {
        processEvents();
        timeSinceLastUpdate += clock.restart();

        while (timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }

        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void Game::update(sf::Time deltaTime) {
    player.handleInput();
    player.update(deltaTime);
}

void Game::render() {
    window.clear();
    player.render(window);
    window.display();
}
