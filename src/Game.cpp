#include "../include/Game.hpp"
#include <iostream>
#include <algorithm>

Game::Game()
    : window(sf::VideoMode(windowWidth, windowHeight), "Malla de Puntos"),
      grid(mapWidth, mapHeight, cellSize) {

    window.setFramerateLimit(60);

    view.reset(sf::FloatRect(0, 0, windowWidth, windowHeight));
    window.setView(view);
}

void Game::run() {
    while (window.isOpen()) {
        if (processEvents()) {
            update();
            render();
        }
    }
}

bool Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed) {
            // Traducir a coordenadas del mundo
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            int x = static_cast<int>(worldPos.x);
            int y = static_cast<int>(worldPos.y);

            if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
                sf::Vector2i mousePos(x, y);
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (waitSecordClick) {
                        grid.insertEdge(firstClick, mousePos);
                        waitSecordClick = false;
                    } else {
                        grid.insertPoint(mousePos);
                    }
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    firstClick = mousePos;
                    waitSecordClick = true;
                }
            }
            return true;
        }
    }

    sf::Vector2f offset(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        offset.y -= cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        offset.y += cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        offset.x -= cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        offset.x += cameraSpeed;

    if (offset.x != 0.f || offset.y != 0.f) {
        sf::Vector2f newCenter = view.getCenter() + offset;

        float halfWidth = view.getSize().x / 2.f;
        newCenter.x = std::clamp(newCenter.x, halfWidth, static_cast<float>(mapWidth) - halfWidth);

        float halfHeight = view.getSize().y / 2.f;
        newCenter.y = std::clamp(newCenter.y, halfHeight, static_cast<float>(mapHeight) - halfHeight);

        view.setCenter(newCenter);
    }

    window.setView(view);

    return true;
}

void Game::update() {
    // nada
}

void Game::render() {
    window.clear();
    grid.render(window);
    window.display();
}
