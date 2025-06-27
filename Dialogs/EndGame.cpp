#include "EndGame.h"
#include <string>
#include <fstream>
#include <chrono>

#define FIGURETHICKNESS 0.09f
#define WINDOWPROPORTIONX1 2.f
#define WINDOWPROPORTIONX2 5.f
#define WINDOWPROPORTIONY1 2.f
#define WINDOWPROPORTIONY2 5.f

#define BUTTONPROPORTIONX1 2.f
#define BUTTONPROPORTIONX2 3.f
#define BUTTONPROPORTIONY1 1.f
#define BUTTONPROPORTIONY2 5.f

#define TEXTPROPORTIONX1 9.f
#define TEXTPROPORTIONY1 2.f
#define TEXTPROPORTIONX2 1.f
#define TEXTPROPORTIONY2 2.f

#define SCORERECORDSFILE "records.bin"
#define MAXRECORDSNUMBER 10


EndGame::EndGame() {
    if (!font.loadFromFile("../NotoSans-Bold.ttf")) {
        throw std::runtime_error("Font was not found!");
    }
    for (decltype(auto) iter : texts) {
        iter.setFont(font);
        iter.setFillColor(sf::Color::Black);
    }
    mainRect.setOutlineColor(sf::Color(230,153,0));
    button.text.setFont(font);
    button.text.setFillColor(sf::Color::White);
    button.text.setString("OK");
    button.rect.setFillColor(sf::Color(25,64,255));
    button.rect.setOutlineColor(sf::Color(0,191,230));
    texts[0].setString("You Lose");
    texts[1].setString("Your score: 12");
}

void EndGame::draw(sf::RenderWindow& window) {
    window.draw(mainRect);
    for (decltype(auto) iter : texts) { window.draw(iter); }
    window.draw(button.rect);
    window.draw(button.text);
}

void EndGame::resize(const sf::Vector2u& windowSizes) {
    mainRect.setSize({WINDOWPROPORTIONX1 * static_cast<float>(windowSizes.x) / WINDOWPROPORTIONX2,
                      WINDOWPROPORTIONY1 * static_cast<float>(windowSizes.y) / WINDOWPROPORTIONY2});
    mainRect.setPosition((static_cast<float>(windowSizes.x) - mainRect.getSize().x) / 2.f,
                         (static_cast<float>(windowSizes.y) - mainRect.getSize().y) / 2.f);
    mainRect.setOutlineThickness(mainRect.getSize().y * FIGURETHICKNESS);
    button.rect.setSize({BUTTONPROPORTIONX1 * mainRect.getSize().x / BUTTONPROPORTIONX2,
                         BUTTONPROPORTIONY1 * mainRect.getSize().y / BUTTONPROPORTIONY2});
    button.text.setCharacterSize(static_cast<unsigned int>(
            std::min(button.rect.getSize().x / TEXTPROPORTIONX2, button.rect.getSize().y / TEXTPROPORTIONY2)));
    auto space = mainRect.getSize().y - button.rect.getSize().y;
    auto textSize = static_cast<unsigned int>(std::min(mainRect.getSize().x / TEXTPROPORTIONX1, mainRect.getSize().y / TEXTPROPORTIONY1));
    for (decltype(auto) iter : texts) {
        iter.setCharacterSize(textSize);
        iter.setOrigin(iter.getLocalBounds().width / 2.f, iter.getLocalBounds().height / 2.f);
        space -= iter.getLocalBounds().height;
    }
    // Adding one for button:
    space /= static_cast<float>(texts.size()) + 2.f;
    for (auto i = 0; i < texts.size(); ++i) {
        texts[i].setPosition(mainRect.getPosition().x + mainRect.getSize().x / 2.f,
                             space + (i != 0 ? texts[i - 1].getPosition().y + texts[i - 1].getLocalBounds().height
                             : mainRect.getPosition().y));
    }
    button.rect.setPosition(mainRect.getPosition().x + (mainRect.getSize().x - button.rect.getSize().x) / 2.f,
                            texts.crbegin()->getPosition().y + texts.crbegin()->getLocalBounds().height + space);
    button.text.setOrigin(button.text.getLocalBounds().width / 2.f, button.text.getLocalBounds().height / 2.f);
    button.text.setPosition(button.rect.getPosition().x + button.rect.getSize().x / 2.f,
                            button.rect.getPosition().y + button.rect.getSize().y / 2.f);
    button.rect.setOutlineThickness(button.rect.getSize().y * FIGURETHICKNESS);
}

void EndGame::setResult(const bool& isWon, const bool& isRed, const uint16_t& score) {
    using namespace std::string_literals;
    mainRect.setFillColor(isRed ? sf::Color(204,0,34) : sf::Color(0,153,230));
    texts[0].setString("You "s + (isWon ? "Win"s : "Lose"s));
    texts[1].setString("Your score: " + std::to_string(score));
    endScore = score;
}

bool EndGame::eventManager(const sf::Event& event, sf::RenderWindow& window) const noexcept {
    return event.type == sf::Event::MouseButtonPressed && isClicked(
            button.rect, sf::Mouse::getPosition(window));
}

bool EndGame::isClicked(const sf::RectangleShape& button, const sf::Vector2i& mousePos) noexcept {
    return static_cast<float>(mousePos.x) >= button.getPosition().x
           && static_cast<float>(mousePos.x) <= button.getPosition().x + button.getSize().x
           && static_cast<float>(mousePos.y) >= button.getPosition().y
           && static_cast<float>(mousePos.y) <= button.getPosition().y + button.getSize().y;
}

void EndGame::saveScoreToFile() const {
    /*
     Package: sizeof(dateT) + 1 bytes;
     sizeof(dateT) bytes - space for date;
     1 byte - space for score;
    */
    using namespace std::chrono;
    using dateT = unsigned int;

    std::ifstream fileRead{SCORERECORDSFILE, std::ios::binary};
    auto tempBuffer = std::vector(std::istreambuf_iterator(fileRead), {});
    fileRead.close();

    auto epoch = static_cast<dateT>(duration_cast<days>(system_clock::now().time_since_epoch()).count());
    auto arr = reinterpret_cast<const char*>(&epoch);
    auto iter = tempBuffer.size();
    if (tempBuffer.size() / (sizeof(dateT) + 1) == MAXRECORDSNUMBER) {
        if (endScore <= *(tempBuffer.crbegin() + sizeof(dateT))) { return; }
        tempBuffer.erase(tempBuffer.end() - (sizeof(dateT) + 1), tempBuffer.end());
        iter -= (sizeof(dateT) + 1) * 2;
    }
    while (iter != 0 && tempBuffer[iter - 1] < endScore) { iter -= sizeof(dateT) + 1; }
    tempBuffer.insert(tempBuffer.begin() + static_cast<long>(iter), static_cast<char>(endScore));
    tempBuffer.insert(tempBuffer.begin() + static_cast<long>(iter), arr, arr + sizeof(dateT));

    std::ofstream fileWrite{SCORERECORDSFILE, std::ios::binary | std::ios::trunc};
    fileWrite.write(&tempBuffer[0], static_cast<long>(tempBuffer.size()));
    fileWrite.close();
}
