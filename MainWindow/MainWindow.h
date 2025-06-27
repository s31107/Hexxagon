#pragma once
#include "SFML_Board.h"
#include "Menu.h"
#include "../Dialogs/EndGame.h"
#include <SFML/Graphics.hpp>


class MainWindow : public sf::RenderWindow {
    bool isEndGame = false;
    SFML_Board board{};
    Menu menu{};
    EndGame endGameDialog{};
public:
    MainWindow();
    void show();
    void eventManagement();
};
