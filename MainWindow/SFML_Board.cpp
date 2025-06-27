#include "SFML_Board.h"
#include "../Bot/Bot.h"
#include <cmath>

#define BOARDPROPOTIONX1 6.f
#define BOARDPROPOTIONX2 7.f
#define BOARDPROPOTIONY1 8.f
#define BOARDPROPOTIONY2 9.f
#define FIGURESTHICKNESS 0.09f
#define SCOREFIGUREPROPORTIONY 7.f
#define TEXTPROPORTIONY1 2.f


SFML_Board::SFML_Board() {
    for (uint16_t i = 0; i < getBoardSize(); ++i) {
        auto hexagon = sf::CircleShape{};
        hexagon.setPointCount(6);
        hexagon.setOutlineColor(sf::Color(255,179,25));
        hexagon.setRotation(30);
        sfmlBoard.push_back(std::move(hexagon));
    }
    if (!font.loadFromFile("../NotoSans-Bold.ttf")) {
        throw std::runtime_error("Font was not found!");
    }
    for (decltype(auto) iter : scoreObjects) {
        iter.rect.setOutlineColor(sf::Color(sf::Color(102,255,25)));
        iter.text.setFont(font);
        iter.text.setString("0");
    }
    scoreObjects[0].rect.setFillColor(sf::Color(204,0,34));
    scoreObjects[1].rect.setFillColor(sf::Color(0,153,230));
    setFillColors();
}

void SFML_Board::draw(sf::RenderWindow& window) {
    for (decltype(auto) iter : sfmlBoard) {
        window.draw(iter);
    }
    for (decltype(auto) iter : scoreObjects) {
        window.draw(iter.rect);
        window.draw(iter.text);
    }
}

void SFML_Board::resize(const sf::Vector2u& windowSizes) {
    auto castWindowSizes = static_cast<sf::Vector2f>(windowSizes);
    resizeBoard(castWindowSizes);
    resizeScore(castWindowSizes);
}

void SFML_Board::resizeScore(const sf::Vector2f& windowSizes) {
    auto size = std::min(windowSizes.x / SCOREFIGUREPROPORTIONY, windowSizes.y / SCOREFIGUREPROPORTIONY);
    for (decltype(auto) iter : scoreObjects) {
        iter.rect.setSize({size, size});
        iter.rect.setOutlineThickness(size * FIGURESTHICKNESS);
    }
    scoreObjects[0].rect.setPosition(scoreObjects[0].rect.getOutlineThickness(),
                                     scoreObjects[0].rect.getOutlineThickness());
    scoreObjects[1].rect.setPosition(windowSizes.x - (size + scoreObjects[1].rect.getOutlineThickness()),
                                     scoreObjects[1].rect.getOutlineThickness());

    for (decltype(auto) iter : scoreObjects) {
        iter.text.setCharacterSize(static_cast<uint32_t>(size / TEXTPROPORTIONY1));
        iter.text.setFillColor(sf::Color::White);
        iter.text.setOrigin(iter.text.getLocalBounds().width / 2.f, iter.text.getLocalBounds().height / 2.f);
        iter.text.setPosition(iter.rect.getPosition().x + size / 2.f, iter.rect.getPosition().y + size / 2.f);
    }
}

void SFML_Board::resizeBoard(const sf::Vector2f& windowSizes) {
    // radius * 14.f, 9.f - width, height of board:
    auto hexagonRadius = std::min(BOARDPROPOTIONX1 * windowSizes.x / (BOARDPROPOTIONX2 * 14.f),
                                  BOARDPROPOTIONY1 * windowSizes.y / (BOARDPROPOTIONY2 * 9.f * std::sqrt(3.f)));
    uint16_t elementsInRow = 0;
    // radius * 6.f(from max left to max right hexagons centre point) width of board divided by 2:
    auto startX = windowSizes.x / 2.f - hexagonRadius * 6.f;
    // radius * 4.f(from max high to max low hexagons centre point) height of board divided by 2:
    // + irregular shape distance:
    auto startY = windowSizes.y / 2.f - (hexagonRadius * 4.f - hexagonRadius * (SIDELENGTH - 1) / 2)
            * std::sqrt(3.f);
    auto hexagon = sfmlBoard.begin();

    for (auto row = 0; row < SIDELENGTH * 2 - 1; ++row) {
        if (row) { elementsInRow += row < SIDELENGTH ? 1 : -1; }
        for (uint16_t seg = 0; seg < elementsInRow + SIDELENGTH; ++seg, ++hexagon) {
            hexagon->setOrigin(hexagonRadius, hexagonRadius);
            hexagon->setPosition(startX + (hexagonRadius * 3.f / 2.f) * static_cast<float>(row),
                                 startY + (hexagonRadius * std::sqrt(3.f)) * (static_cast<float>(seg)
                                 - static_cast<float>(elementsInRow) / 2.f));
            hexagon->setRadius(hexagonRadius);
            hexagon->setOutlineThickness(hexagonRadius * FIGURESTHICKNESS);
        }
    }
}

bool SFML_Board::eventManager(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type != sf::Event::MouseButtonPressed) { return false; }
    auto mousePos = sf::Mouse::getPosition(window);
    static auto lastPressed = -1;
    for (auto iter = 0; iter < sfmlBoard.size(); ++iter) {
        if (std::pow(static_cast<float>(mousePos.x) - sfmlBoard[iter].getPosition().x, 2)
            + std::pow(static_cast<float>(mousePos.y) - sfmlBoard[iter].getPosition().y, 2)
            > std::pow(sfmlBoard[iter].getRadius(), 2)) { continue; }
        if (lastPressed != iter) {
            switch (get(iter)->segmentState) {
                case Segment::State::RED:
                case Segment::State::BLUE:
                    clearHighlightedElements();
                    if (getCurrentColor() == get(iter)->segmentState) {
                        for (auto move: getPossibleMoves(get(iter))) {
                            move->segmentState = Segment::State::HIGHLITED;
                        }
                        lastPressed = iter;
                    }
                    setFillColors(); return false;
                case Segment::State::HIGHLITED:
                    clearHighlightedElements();
                    if (makeMove(get(iter), get(lastPressed))) { return true; }
                    if (getPlayingWithComputer()) {
                        auto botMove = calculateMove(*this);
                        if (makeMove(get(botMove.to), get(botMove.from))) { return true; }
                    } else {
                        setCurrentColor(get(iter)->segmentState == Segment::State::RED ? Segment::State::BLUE
                                : Segment::State::RED);
                    } break;
                default: clearHighlightedElements();
            }
        } else { clearHighlightedElements(); }
        setFillColors();
        lastPressed = -1; break;
    } return false;
}

void SFML_Board::setFillColors() {
    for (auto i = 0; i < getBoardSize(); ++i) {
        switch (get(i)->segmentState) {
            case Segment::State::NONUSED:
                sfmlBoard[i].setFillColor(sf::Color::Black);
                break;
            case Segment::State::FREE:
                sfmlBoard[i].setFillColor(sf::Color::White);
                break;
            case Segment::State::RED:
                sfmlBoard[i].setFillColor(sf::Color(204,0,34));
                break;
            case Segment::State::BLUE:
                sfmlBoard[i].setFillColor(sf::Color(0,153,230));
                break;
            case Segment::State::HIGHLITED:
                sfmlBoard[i].setFillColor(sf::Color(102,255,25));
                break;
        }
    }
}

bool SFML_Board::makeMove(SegmentP newField, SegmentP oldField) {
    newField->segmentState = getCurrentColor();
    bool isAdjanced = false;
    int16_t score1 = 0, score2 = 0;
    auto opponentColor = (getCurrentColor() == Segment::State::RED ? Segment::State::BLUE : Segment::State::RED);
    for (auto field : getAdjacentFields(newField)) {
        if (field == oldField) {
            isAdjanced = true;
            ++score1;
        } else if (field->segmentState == opponentColor) {
            field->segmentState = getCurrentColor();
            ++score1; --score2;
        }
    }
    if (!isAdjanced) { oldField->segmentState = Segment::State::FREE; }
    // Checking if match is over:
    if (getAll(Segment::State::FREE).empty() || getAll(opponentColor).empty() || std::ranges::all_of(
            getAll(opponentColor), [this](auto iter) -> bool {
        return getPossibleMoves(iter).empty();
    })) {
        // Ending match:
        const auto freeFields = getAll(Segment::State::FREE);
        for (auto field : freeFields) {
            field->segmentState = getCurrentColor();
        }
        updateScore(static_cast<int16_t>(score1 + freeFields.size()), getCurrentColor());
        updateScore(score2, opponentColor);
        setFillColors();
        return true;
    }
    updateScore(score1, getCurrentColor());
    updateScore(score2, opponentColor);
    setCurrentColor(opponentColor);
    return false;
}

void SFML_Board::prepareBoard() {
    clear();
    uint16_t index = 0;
    for (decltype(auto) iter : {0, 56, 34, 26, 4, 60}) {
        if (index > 2) { get(iter)->segmentState = Segment::State::RED; }
        else { get(iter)->segmentState = Segment::State::BLUE; }
        ++index;
    }
    for (decltype(auto) iter : scoreObjects) {
        iter.text.setString("3");
        // Aligning scores to center:
        iter.text.setOrigin(iter.text.getLocalBounds().width / 2.f, iter.text.getLocalBounds().height / 2.f);
        iter.text.setPosition(iter.rect.getPosition().x + iter.rect.getSize().x / 2.f, iter.text.getPosition().y);
    }
    setCurrentColor(Segment::State::RED);
    setFillColors();
}

void SFML_Board::updateScore(const int16_t& points, const Segment::State& color) {
    decltype(auto) scoreObj = scoreObjects[color == Segment::State::BLUE];
    scoreObj.text.setString(std::to_string(std::stoi(static_cast<const std::string&>(
            scoreObj.text.getString())) + points));
    // Aligning scores to center:
    scoreObj.text.setOrigin(
            scoreObj.text.getLocalBounds().width / 2.f, scoreObj.text.getLocalBounds().height / 2.f);
    scoreObj.text.setPosition(
            scoreObj.rect.getPosition().x + scoreObj.rect.getSize().x / 2.f, scoreObj.text.getPosition().y);
}

void SFML_Board::calculateScores() {
    scoreObjects[0].text.setString(std::to_string(getAll(Segment::State::RED).size()));
    scoreObjects[1].text.setString(std::to_string(getAll(Segment::State::BLUE).size()));
    // Aligning scores to center:
    for (decltype(auto) iter : scoreObjects) {
        iter.text.setOrigin(iter.text.getLocalBounds().width / 2.f, iter.text.getLocalBounds().height / 2.f);
        iter.text.setPosition(iter.rect.getPosition().x + iter.rect.getSize().x / 2.f, iter.text.getPosition().y);
    }
}

uint16_t SFML_Board::getScore(const Segment::State& color) const noexcept {
    return std::stoi(static_cast<const std::string&>(scoreObjects[color == Segment::State::BLUE].text.getString()));
}
