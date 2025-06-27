#pragma once
#include <SFML/Graphics.hpp>


class LeaderBoard {
    struct Record {
        sf::RectangleShape rect{};
        sf::Text score{};
    };
    struct Button {
        sf::Text label{};
        sf::RectangleShape rect{};
    } button{};
    sf::RectangleShape mainRect{};
    std::vector<Record> scoreRecords{};
    sf::Text title{};
    sf::Font font{};
public:
    LeaderBoard();
    void updateRecords(const sf::Vector2u &windowSizes);
    void draw(sf::RenderWindow& window);
    void resize(const sf::Vector2u& windowSizes);
    bool eventManager(const sf::Event& event, sf::RenderWindow& window) const noexcept;
};
