#include "LeaderBoard.h"
#include <fstream>
#include <chrono>
#include <sstream>

#define SCORERECORDSFILE "records.bin"
#define MAXRECORDSNUMBER 10lu
#define DATESEPARATOR '/'
#define DATESCORESEP ": "

#define WIDGETSSPACEPROPORTION 0.05f
#define FIGURETHICKNESS 0.03f
#define WINDOWPROPORTIONX1 9.f
#define WINDOWPROPORTIONX2 10.f
#define WINDOWPROPORTIONY1 9.f
#define WINDOWPROPORTIONY2 10.f

#define BUTTONPROPORTIONX1 2.f
#define BUTTONPROPORTIONX2 10.f
#define BUTTONPROPORTIONY1 1.f
#define BUTTONPROPORTIONY2 10.f

#define TEXTPROPORTIONX1 1.f
#define TEXTPROPORTIONY1 1.f
#define TEXTPROPORTIONX2 20.f
#define TEXTPROPORTIONY2 20.f

#define BUTTONTEXTPROPORTIONX1 1.f
#define BUTTONTEXTPROPORTIONY1 1.f
#define BUTTONTEXTPROPORTIONX2 2.f
#define BUTTONTEXTPROPORTIONY2 2.f

#define RECORDTEXTPROPORTIONX1 1.f
#define RECORDTEXTPROPORTIONY1 1.f
#define RECORDTEXTPROPORTIONX2 10.f
#define RECORDTEXTPROPORTIONY2 2.f


LeaderBoard::LeaderBoard() {
    if (!font.loadFromFile("../NotoSans-Bold.ttf")) {
        throw std::runtime_error("Font was not found!");
    }
    button.label.setFont(font);
    button.label.setString("OK");
    title.setFont(font);
    button.rect.setFillColor(sf::Color(25,64,255));
    button.rect.setOutlineColor(sf::Color(0,191,230));
    button.label.setFillColor(sf::Color::White);
    mainRect.setFillColor(sf::Color(255,179,102));
    mainRect.setOutlineColor(sf::Color(230,153,0));
    title.setString("LeaderBoards:");
}

void LeaderBoard::updateRecords(const sf::Vector2u& windowSizes) {
    /*
     Package: sizeof(dateT) + 1 bytes;
     sizeof(dateT) bytes - space for date;
     1 byte - space for score;
    */

    using namespace std::chrono;
    using dateT = unsigned int;

    scoreRecords.clear();
    std::ifstream fileRead{SCORERECORDSFILE, std::ios::binary};
    auto tempBuffer = std::vector(std::istreambuf_iterator(fileRead), {});
    for (auto i = 0; i < std::min(tempBuffer.size(), MAXRECORDSNUMBER * (sizeof(dateT) + 1)); i += sizeof(dateT) + 1) {
        auto record = Record{};
        record.rect.setFillColor(sf::Color(230,230,0));
        record.rect.setOutlineColor(sf::Color(0,191,230));
        record.score.setFont(font);
        record.score.setFillColor(sf::Color::Black);
        auto recordData = year_month_day{time_point_cast<days>(system_clock::time_point{
            days{*reinterpret_cast<unsigned int*>(&tempBuffer[i])}})};
        auto tempStream = std::stringstream{};
        tempStream << static_cast<int>(recordData.year()) << DATESEPARATOR
            << static_cast<unsigned int>(recordData.month()) << DATESEPARATOR
            << static_cast<unsigned int>(recordData.day()) << DATESCORESEP
            << std::to_string(tempBuffer[i + sizeof(dateT)]);
        record.score.setString(tempStream.str());
        scoreRecords.push_back(std::move(record));
    } resize(windowSizes);
    fileRead.close();
}

void LeaderBoard::draw(sf::RenderWindow& window) {
    window.draw(mainRect);
    window.draw(button.rect);
    window.draw(button.label);
    window.draw(title);
    for (decltype(auto) iter : scoreRecords) {
        window.draw(iter.rect);
        window.draw(iter.score);
    }
}

void LeaderBoard::resize(const sf::Vector2u& windowSizes) {
    // Configuring main rectangle (mainRect):
    mainRect.setSize({WINDOWPROPORTIONX1 * static_cast<float>(windowSizes.x) / WINDOWPROPORTIONX2,
                      WINDOWPROPORTIONY1 * static_cast<float>(windowSizes.y) / WINDOWPROPORTIONY2});
    mainRect.setPosition((static_cast<float>(windowSizes.x) - mainRect.getSize().x) / 2.f,
                         (static_cast<float>(windowSizes.y) - mainRect.getSize().y) / 2.f);
    mainRect.setOutlineThickness(mainRect.getSize().y * FIGURETHICKNESS);
    mainRect.setOutlineThickness(mainRect.getSize().y * FIGURETHICKNESS);
    // Configuring title and button:
    auto spaceBetweenWidgets = mainRect.getSize().y * WIDGETSSPACEPROPORTION;
    title.setCharacterSize(static_cast<unsigned int>(TEXTPROPORTIONX1 * std::min(mainRect.getSize().x
        / TEXTPROPORTIONX2, TEXTPROPORTIONY1 * mainRect.getSize().y / TEXTPROPORTIONY2)));
    title.setPosition(mainRect.getPosition().x + (mainRect.getSize().x - title.getLocalBounds().width) / 2.f,
                      mainRect.getPosition().y + spaceBetweenWidgets);

    button.rect.setSize({BUTTONPROPORTIONX1 * mainRect.getSize().x / BUTTONPROPORTIONX2,
                         BUTTONPROPORTIONY1 * mainRect.getSize().y / BUTTONPROPORTIONY2});
    button.label.setCharacterSize(static_cast<unsigned int>(std::min(BUTTONTEXTPROPORTIONX1
        * button.rect.getSize().x / BUTTONTEXTPROPORTIONX2, BUTTONTEXTPROPORTIONY1 * button.rect.getSize().y
        / BUTTONTEXTPROPORTIONY2)));
    button.rect.setPosition(mainRect.getPosition().x + (mainRect.getSize().x - button.rect.getSize().x) / 2.f,
                            mainRect.getPosition().y + mainRect.getSize().y - button.rect.getSize().y
                            - spaceBetweenWidgets);
    button.label.setOrigin(button.label.getLocalBounds().width / 2.f, button.label.getLocalBounds().height / 2.f);
    button.label.setPosition(button.rect.getPosition().x + button.rect.getSize().x / 2.f,
                             button.rect.getPosition().y + button.rect.getSize().y / 2.f);
    button.rect.setOutlineThickness(button.rect.getSize().y * FIGURETHICKNESS);
    // Configuring records:
    auto rectThickness = mainRect.getSize().y * FIGURETHICKNESS;
    auto startPos = title.getPosition().y + title.getLocalBounds().height + spaceBetweenWidgets + rectThickness;
    auto size = ((button.rect.getPosition().y - startPos - spaceBetweenWidgets))
            / static_cast<float>(scoreRecords.size()) - rectThickness;
    for (decltype(auto) scoreRecordIter : scoreRecords) {
        scoreRecordIter.rect.setOutlineThickness(rectThickness);
        scoreRecordIter.rect.setSize({mainRect.getSize().x - 2 * rectThickness, size});
        scoreRecordIter.rect.setPosition(mainRect.getPosition().x + rectThickness, startPos);
        startPos += size + rectThickness;
        scoreRecordIter.score.setCharacterSize(static_cast<unsigned int>(std::min(RECORDTEXTPROPORTIONX1
            * scoreRecordIter.rect.getSize().x / RECORDTEXTPROPORTIONX2, RECORDTEXTPROPORTIONY1
            * scoreRecordIter.rect.getSize().y / RECORDTEXTPROPORTIONY2)));
        scoreRecordIter.score.setOrigin(scoreRecordIter.score.getLocalBounds().width / 2.f,
                                        scoreRecordIter.score.getLocalBounds().height / 2.f);
        scoreRecordIter.score.setPosition(scoreRecordIter.rect.getPosition().x + scoreRecordIter.rect.getSize().x
            / 2.f, scoreRecordIter.rect.getPosition().y + scoreRecordIter.rect.getSize().y / 2.f);
    }
}

bool LeaderBoard::eventManager(const sf::Event& event, sf::RenderWindow& window) const noexcept {
    if (event.type == sf::Event::MouseButtonPressed) {
        auto mousePos = sf::Mouse::getPosition(window);
        return static_cast<float>(mousePos.x) >= button.rect.getPosition().x
        && static_cast<float>(mousePos.x) <= button.rect.getPosition().x + button.rect.getSize().x
        && static_cast<float>(mousePos.y) >= button.rect.getPosition().y
        && static_cast<float>(mousePos.y) <= button.rect.getPosition().y + button.rect.getSize().y;
    } return false;
}
