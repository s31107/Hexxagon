#pragma once
#include <SFML/Graphics.hpp>
#include <array>


class EndGame {
    uint16_t endScore = 0;
    std::array<sf::Text, 2> texts{};
    sf::RectangleShape mainRect{};
    struct Button {
        sf::RectangleShape rect{};
        sf::Text text{};
    } button{};
    sf::Font font{};
    static bool isClicked(const sf::RectangleShape& button, const sf::Vector2i& mousePos) noexcept;
public:
    EndGame();
    void saveScoreToFile() const;
    void setResult(const bool& isWon, const bool& isRed, const uint16_t& score);
    void draw(sf::RenderWindow& window);
    void resize(const sf::Vector2u& windowSizes);
    bool eventManager(const sf::Event& event, sf::RenderWindow& window) const noexcept;
};