#include "../include/Hud.hpp"
#include <iostream>

Hud::Hud(int widthWinFloat, int heightWinFloat) 
: 
    posX(0), 
    posY(0), 
    widthWinFloat(widthWinFloat), 
    heightWinFloat(heightWinFloat),
    alg(Algorithm::A_STAR),
    map(Map::RANDOM),
    mode(Mode::NOTHING)
{
    renderTextureOpt.create(widthOpt, height);
    renderTextureShow.create(widthShow, height);
    spriteShow.setPosition(0, 0);
    spriteOpt.setPosition(0, 0);
}

void Hud::render(sf::RenderWindow& window) {
    window.draw(spriteShow);
    window.draw(spriteOpt);
}

// calculateRoute | changeAlg | createMap | changeMap
void Hud::updateOpt(int x, int y) {

    const float space = 120.f;
    const int numColumns = 4;
    const float shiftRight = widthWinFloat - widthOpt;
    const int textSize = 12;
    const float sepTop = 5.f;
    const float sepLeft = 10.f;

    // int wo = widthOpt / 4;
    std::cout << x << " " << y << std::endl;
    if (x > space * 0 + shiftRight && x < space * 1 + shiftRight) {
        std::cout << "1\n";
        this->sl = Select::CALCULATE_ROUTE;
    }
    else if (x > space * 1 + shiftRight && x < space * 2 + shiftRight) {
        std::cout << "2\n";
        this->sl = Select::CHANGE_ALG;
    }
    else if (x > space * 2 + shiftRight && x < space * 3 + shiftRight) {
        std::cout << "3\n";
        this->sl = Select::CREATE_MAP;
    }
    else if (x > space * 3 + shiftRight && x < space * 4 + shiftRight) {
        std::cout << "4\n";
        this->sl = Select::CHANGE_MAP;
    }

    sf::RectangleShape bar(sf::Vector2f(space * numColumns, height));
    bar.setPosition(0, 0);
    bar.setFillColor(sf::Color::Black);
    renderTextureOpt.draw(bar);

    sf::Font font;
    if (!font.loadFromFile("fonts/Roboto-Black.ttf")) return;

    sf::RectangleShape rect(sf::Vector2f(space, height - 4.f));
    rect.setPosition(space * 0 + 2.f, 2.f);
    rect.setFillColor(sf::Color::Black);
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(sf::Color(175, 177, 219));
    renderTextureOpt.draw(rect);

    rect.setPosition(space * 1 + 2.f, 2.f);
    renderTextureOpt.draw(rect);

    rect.setPosition(space * 2 + 2.f, 2.f);
    renderTextureOpt.draw(rect);

    rect.setPosition(space * 3 + 2.f, 2.f);
    renderTextureOpt.draw(rect);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(textSize);
    text.setFillColor(sf::Color::White);


    text.setString("Calculate Route");
    text.setPosition(space * 0 + sepLeft, sepTop);
    renderTextureOpt.draw(text);


    text.setString("Change algorithm");
    text.setPosition(space * 1 + sepLeft, sepTop);
    renderTextureOpt.draw(text);

    text.setString("create map");
    text.setPosition(space * 2 + sepLeft, sepTop);
    renderTextureOpt.draw(text);

    text.setString("change map");
    text.setPosition(space * 3 + sepLeft, sepTop);
    renderTextureOpt.draw(text);


    renderTextureOpt.display();
    spriteOpt.setTexture(renderTextureOpt.getTexture(), true);
    spriteOpt.setPosition(posX + shiftRight, posY);
}

void Hud::updateShow(int sizeNodes) {
    renderTextureShow.clear(sf::Color{0x373737FF});

    const float space = 120.f;
    const int numColumns = 4;
    const int textSize = 12;
    const float sepTop = 5.f;
    const float sepLeft = 10.f;

    sf::RectangleShape bar(sf::Vector2f(space * numColumns, height));
    bar.setPosition(0, 0);
    bar.setFillColor(sf::Color::Black);
    renderTextureShow.draw(bar);

    sf::Font font;
    if (!font.loadFromFile("fonts/Roboto-Black.ttf")) return;

    sf::RectangleShape rect(sf::Vector2f(space, height - 4.f));
    rect.setPosition(space * 0 + 2.f, 2.f);
    rect.setFillColor(sf::Color::Black);
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(sf::Color(175, 177, 219));
    renderTextureShow.draw(rect);

    rect.setPosition(space * 1 + 2.f, 2.f);
    renderTextureShow.draw(rect);

    rect.setPosition(space * 2 + 2.f, 2.f);
    renderTextureShow.draw(rect);

    rect.setPosition(space * 3 + 2.f, 2.f);
    renderTextureShow.draw(rect);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(textSize);
    text.setFillColor(sf::Color::White);


    text.setString("Nodos: " + std::to_string(sizeNodes));
    text.setPosition(space * 0 + sepLeft, sepTop);
    renderTextureShow.draw(text);


    switch (alg) {
        case Algorithm::A_STAR: text.setString("Alg: A*"); break;
        case Algorithm::DIJKSTRA: text.setString("Alg: Dijkstra"); break;
    }
    text.setPosition(space * 1 + sepLeft, sepTop);
    renderTextureShow.draw(text);

    switch (map) {
        case Map::RANDOM: text.setString("Mapa: Random"); break;
        case Map::LIMA: text.setString("Mapa: Lima"); break;
        case Map::AREQUIPA: text.setString("Mapa: Arequipa"); break;
        case Map::NEW_YORK: text.setString("Mapa: New York"); break;
    }
    text.setPosition(space * 2 + sepLeft, sepTop);
    renderTextureShow.draw(text);

    switch (mode) {
        case Mode::NOTHING: text.setString("Modo: Nada"); break;
        case Mode::INSERT_NODES: text.setString("Insertar Nodos"); break;
        case Mode::INSERT_ARIST: text.setString("Insertar Aristas"); break;
        case Mode::REMOVE_ARISTS: text.setString("Remover Aristas"); break;
        case Mode::REMOVE_NODE: text.setString("Remover Nodo"); break;
    }
    text.setPosition(space * 3 + sepLeft, sepTop);
    renderTextureShow.draw(text);


    renderTextureShow.display();
    spriteShow.setTexture(renderTextureShow.getTexture(), true);
    spriteShow.setPosition(static_cast<float>(posX), static_cast<float>(posY));
}

int Hud::getHeight() const {
    return height;
}
int Hud::getWidthShow() const {
    return widthShow;
}
int Hud::getWidthOpt() const {
    return widthOpt;
}
Select Hud::getSl() const {
    return sl;
}
Mode Hud::getMode() const {
    return mode;
}
Algorithm Hud::getAlg() const {
    return alg;
}
Map Hud::getMap()  const {
    return map;
}

void Hud::setPosition(int x, int y) {
    this->posX = x;
    this->posY = y;
}

void Hud::setMode(Mode mode) {
    this->mode = mode;
}

void Hud::nextAlg() {
    alg = static_cast<Algorithm>(static_cast<int>(alg) + 1);
    if (alg == Algorithm::COUNT) {
        alg = static_cast<Algorithm>(0);
    }
}
void Hud::nextMap() {
    map = static_cast<Map>(static_cast<int>(map) + 1);
    if (map == Map::COUNT) {
        map = static_cast<Map>(0);
    }
}
