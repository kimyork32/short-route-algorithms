#include "../include/Game.hpp"
#include <iostream>
#include <algorithm>

Game::Game()
    : window(sf::VideoMode(windowWidth, windowHeight), "Malla de Puntos"),
        grid(mapWidth, mapHeight, pointSize, sizeNodes), 
        hud(windowWidth, windowHeight) {

    window.setFramerateLimit(60);

    view.reset(sf::FloatRect(0, 0, windowWidth, windowHeight));
    window.setView(view);
}

sf::Vector2f Game::getViewTopLeft() const {
    sf::Vector2f center = view.getCenter();
    sf::Vector2f size = view.getSize();
    return sf::Vector2f(center.x - size.x / 2.f, center.y - size.y / 2.f);
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseButtonPressed) {

            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);
            int x = static_cast<int>(worldPos.x);
            int y = static_cast<int>(worldPos.y);

            sf::Vector2f posWin = getViewTopLeft();

            if ((x > posWin.x + windowWidth - hud.getWidthOpt()) && 
                (y < posWin.y + hud.getHeight())) {
                // std::cout << "update opt\n";
                hud.updateOpt(x - static_cast<int>(posWin.x), y - static_cast<int>(posWin.y));
                switch (hud.getSl()) {
                    case Select::NOTHING: 
                        // nada 
                        break;
                    case Select::CALCULATE_ROUTE: 
                        calculateRoute();
                        break;
                    case Select::CHANGE_ALG: 
                        hud.nextAlg();
                        break;
                    case Select::CREATE_MAP: 
                        generateMap();
                        break;
                    case Select::CHANGE_MAP: 
                        hud.nextMap();
                        break;
                }
            }
            // if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
            else {
                sf::Vector2i mousePos(x, y); 
                switch (hud.getMode()) {
                    case Mode::NOTHING: 
                        // std::cout << "nada" << std::endl;
                        break;
                    case Mode::INSERT_ARIST: 
                        // std::cout << "mode insert arist" << std::endl;
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            if (waitSecordClick) {
                                grid.insertEdgeWindow(firstClick, mousePos);
                                waitSecordClick = false;
                            }
                            else {
                                firstClick = mousePos;
                                waitSecordClick = true;
                            }
                        }
                        break;

                    case Mode::INSERT_NODES:
                        // std::cout << "mode insert nodes" << std::endl;
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            grid.insertPointWindow(mousePos);
                        }
                        break;
                    
                    case Mode::REMOVE_ARISTS: 
                        std::cout << "mode remove arists" << std::endl;
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            if (waitSecordClick) {
                                grid.removeEdgeWindow(firstClick, mousePos);
                                waitSecordClick = false;
                            }
                            else {
                                firstClick = mousePos;
                                waitSecordClick = true;
                            }
                        }
                        break;

                    case Mode::REMOVE_NODE:
                        // std::cout << "mode remove node" << std::endl;
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            grid.removeNodeWindow(mousePos);
                        }
                        break;

                    case Mode:: SELECT_START_END_NODE: 
                        std::cout << "mode insert start_end_node" << std::endl;
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            if (waitSecordClick) {
                                grid.insertStartEndNode(firstClick, mousePos);
                                waitSecordClick = false;
                            }
                            else {
                                firstClick = mousePos;
                                waitSecordClick = true;
                            }
                        }
                        break;
                }
            }

        }
        if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
                case sf::Keyboard::X:
                    std::cout << "remove arist" << std::endl;
                    if (hud.getMode() != Mode::REMOVE_ARISTS) {
                        hud.setMode(Mode::REMOVE_ARISTS);
                    }
                    else {
                        waitSecordClick = false;
                        hud.setMode(Mode::NOTHING);
                    }
                    break;

                case sf::Keyboard::C:
                    std::cout << "remove node" << std::endl;
                    if (hud.getMode() != Mode::REMOVE_NODE)
                        hud.setMode(Mode::REMOVE_NODE);
                    else
                        hud.setMode(Mode::NOTHING);
                    break;

                case sf::Keyboard::I:
                    if (hud.getMode() != Mode::INSERT_NODES)
                        hud.setMode(Mode::INSERT_NODES);
                    else
                        hud.setMode(Mode::NOTHING);
                    break;

                case sf::Keyboard::T:
                    if (hud.getMode() != Mode::INSERT_ARIST)
                        hud.setMode(Mode::INSERT_ARIST);
                    else
                        hud.setMode(Mode::NOTHING);
                    break;

                case sf::Keyboard::H:
                    if (hud.getMode() != Mode::SELECT_START_END_NODE)
                        hud.setMode(Mode::SELECT_START_END_NODE);
                    else
                        hud.setMode(Mode::NOTHING);
                    break;

                default:
                    break;
            }
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
    
}

void Game::update() {
    sf::Vector2f posWin = getViewTopLeft();
    hud.setPosition(posWin.x, posWin.y);
    hud.updateShow(grid.getSizeNodes());
    hud.updateOpt(-1, -1);
    hud.updateMinimap(posWin, grid.getMapSprite());
}

void Game::render() {
    window.clear();
    // window.setView(view);
    grid.render(window);
    // window.setView(window.getDefaultView());
    hud.render(window);
    window.display();
}

void Game::calculateRoute() {
    switch (hud.getAlg()) {
        case Algorithm::DIJKSTRA: 
            grid.dijkstra();
            break;
        case Algorithm::A_STAR: 
            grid.aStar();
            break;
    }
}
void Game::generateMap() {
    switch (hud.getMap()) {
        case Map::RANDOM: 
            grid.genRandGraph();
            // hud.updateMinimap(posWin, grid.getMapSprite());
            break;
        case Map::LIMA: 
            grid.genLima(mapWidth, mapHeight);
            break;
        case Map::AREQUIPA: 
            grid.genArequipa(mapWidth, mapHeight);
            // hud.updateMinimap(posWin, grid.getMapSprite());
            break;
        case Map::NEW_YORK: 
            grid.genNewYork(mapWidth, mapHeight);
            break;
    }
}
