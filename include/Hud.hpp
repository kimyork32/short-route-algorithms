#pragma once
#ifndef HUD_CPP
#define HUD_CPP
#include <string>
#include <SFML/Graphics.hpp>
#include "Mode.hpp"

class Hud {
public:
    Hud(int widthWinFloat, int heightWinFloat);
    void render(sf::RenderWindow& window);
    void updateOpt(int x, int y);
    void updateShow(int sizeNodes);
    void updateMinimap(sf::Vector2f coordinates, sf::Sprite window);

    int getHeight() const;
    int getWidthShow() const;
    int getWidthOpt() const;
    Select getSl() const;
    Mode getMode() const;
    Algorithm getAlg() const;
    Map getMap()  const;

    void setPosition(int x, int y);
    void setMode(Mode mode);
    void nextAlg();
    void nextMap();

private:
    int posX;
    int posY;

    int widthWinFloat;
    int heightWinFloat;

    const int mapWidth = 10000;
    const int mapHeight = 10000;

    const float widthScaleMinimap = 15.f / 100;
    const float heightScaleMinimap = 15.f / 100;

    const int height = 25;
    const int widthShow = 480 + 4;
    const int widthOpt = 480 + 4;

    sf::RenderTexture renderTextureOpt;
    sf::Sprite spriteOpt;

    sf::RenderTexture renderTextureShow;
    sf::Sprite spriteShow;

    sf::RenderTexture renderTextureMinimap;
    sf::Sprite spriteMinimap;

    // CONDICIONALES
    Select sl;
    Mode mode;
    Algorithm alg;
    Map map;
};

#endif
