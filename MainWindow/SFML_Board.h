#pragma once
#include "../Board/Board.h"
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>


class SFML_Board : public Board {
    struct ScoreObject {
        sf::RectangleShape rect{};
        sf::Text text{};
    };
    sf::Font font;
    std::vector<sf::CircleShape> sfmlBoard{};
    std::array<ScoreObject, 2> scoreObjects{};
    void resizeBoard(const sf::Vector2f& windowSizes);
    void resizeScore(const sf::Vector2f& windowSizes);
    bool makeMove(SegmentP newField, SegmentP oldField);
    void updateScore(const int16_t& points, const Segment::State& color);
public:
    using Board::operator=;
    uint16_t getScore(const Segment::State& color) const noexcept;
    SFML_Board();
    void draw(sf::RenderWindow& window);
    void resize(const sf::Vector2u& windowSizes);
    bool eventManager(const sf::Event& event, sf::RenderWindow& window);
    void calculateScores();
    void prepareBoard();
    void setFillColors();
};
