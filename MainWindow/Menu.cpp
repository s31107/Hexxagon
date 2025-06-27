#include "Menu.h"
#include <cstdint>
#include <fstream>
#include <stdexcept>

#define FIGURESTHICKNESS 0.09f
#define BUTTONPROPORTIONX 3
#define BUTTONPROPORTIONX2 4.f
#define BUTTONPROPORTIONY 13.f
#define TEXTPROPORTION1 7.f
#define TEXTPROPORTION2 2.f


Menu::Menu() {
    if (!font.loadFromFile("../NotoSans-Bold.ttf")) {
        throw std::runtime_error("Font was not found!");
    }
    for (auto i = 0; i < 7; ++i) {
        auto segment = sf::RectangleShape{};
        segment.setFillColor(sf::Color(25,64,255));
        segment.setOutlineColor(sf::Color(0,191,230));
        if (i < 5) { mainMenuSegments.push_back(std::move(segment)); }
        else { gameMenuSegments.push_back(std::move(segment)); }
    }
    uint16_t iter = 0;
    for (auto text : {"New Game", "Save Game", "Load Game", "Leaderboard", "Exit", "AI", "Hot Seat"}) {
        auto tObject = sf::Text{};
        tObject.setString(text);
        tObject.setFont(font);
        if (iter < 5) { mainMenuTexts.push_back(std::move(tObject)); }
        else { gameMenuTexts.push_back(std::move(tObject)); }
        ++iter;
    }
}

void Menu::resize(const sf::Vector2u& windowSizes) noexcept {
    auto wS = sf::Vector2f{static_cast<float>(windowSizes.x), static_cast<float>(windowSizes.y)};
    resizeSegments(wS, mainMenuSegments, mainMenuTexts);
    resizeSegments(wS, gameMenuSegments, gameMenuTexts);
    saveLoadDialog.resize(windowSizes);
    if (currentMenu == MenuType::LEADERBOARDS) { leaderBoard.resize(windowSizes); }
}

void Menu::resizeSegments(
        sf::Vector2f windowSizes, std::vector<sf::RectangleShape>& buttons, std::vector<sf::Text>& texts) noexcept {
    auto sizeX = BUTTONPROPORTIONX * windowSizes.x / BUTTONPROPORTIONX2;
    auto sizeY = windowSizes.y / BUTTONPROPORTIONY;
    auto rectDistance = windowSizes.y / static_cast<float>(buttons.size()) - sizeY;
    for (auto i = 0; i < buttons.size(); ++i) {
        buttons.at(i).setSize({sizeX, sizeY});
        buttons.at(i).setPosition((windowSizes.x - sizeX) / 2.f,
                                  (i != 0 ? buttons.at(i - 1).getPosition().y + sizeY + rectDistance
                                  : rectDistance / 2.f));
        buttons.at(i).setOutlineThickness(sizeY * FIGURESTHICKNESS);
    }
    auto iter = buttons.begin();
    for (decltype(auto) text : texts) {
        text.setCharacterSize(static_cast<uint32_t>(std::min(sizeX / TEXTPROPORTION1, sizeY / TEXTPROPORTION2)));
        text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height / 2.f);
        text.setPosition(iter->getPosition().x + sizeX / 2.f, iter->getPosition().y + sizeY / 2.f);
        ++iter;
    }
}

void Menu::draw(sf::RenderWindow& window) {
    auto drawOperation = [&window](const auto& vec) -> void {
        for (decltype(auto) iter : vec) {
            window.draw(iter);
        }
    };
    switch(currentMenu) {
        case MenuType::MAINMENU:
            drawOperation(mainMenuSegments);
            drawOperation(mainMenuTexts);
            break;
        case MenuType::GAMEMENU:
            drawOperation(gameMenuSegments);
            drawOperation(gameMenuTexts);
            break;
        case MenuType::LEADERBOARDS:
            leaderBoard.draw(window);
            break;
        case MenuType::SAVEMENU:
        case MenuType::LOADMENU:
            saveLoadDialog.draw(window);
        default:
            break;
    }
}

Menu::Signal Menu::eventManager(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        currentMenu = (currentMenu == MenuType::MAINMENU ? MenuType::NOMENU : MenuType::MAINMENU);
    }
    auto mousePos = sf::Mouse::getPosition(window);
    if (currentMenu == MenuType::GAMEMENU && event.type == sf::Event::MouseButtonPressed) {
        if (isClicked(gameMenuSegments[0], mousePos)) {
            currentMenu = MenuType::NOMENU;
            return Signal::PLAYAI;
        }
        else if (isClicked(gameMenuSegments[1], mousePos)) {
            currentMenu = MenuType::NOMENU;
            return Signal::PLAYHOTSEAT;
        }
    } else if (currentMenu == MenuType::MAINMENU && event.type == sf::Event::MouseButtonPressed) {
        if (isClicked(mainMenuSegments[0], mousePos)) { currentMenu = MenuType::GAMEMENU; }
        else if (isClicked(mainMenuSegments[1], mousePos)) {
            currentMenu = MenuType::SAVEMENU;
            saveLoadDialog.setIsSaving(true);
        }
        else if (isClicked(mainMenuSegments[2], mousePos)) {
            currentMenu = MenuType::LOADMENU;
            saveLoadDialog.setIsSaving(false);
        }
        else if (isClicked(mainMenuSegments[3], mousePos)) {
            currentMenu = MenuType::LEADERBOARDS;
            leaderBoard.updateRecords(window.getSize());
        }
        else if (isClicked(mainMenuSegments[4], mousePos)) { window.close(); }
    } else if (currentMenu == MenuType::LEADERBOARDS) {
        if (leaderBoard.eventManager(event, window)) {
            currentMenu = MenuType::MAINMENU;
        }
    } else if (currentMenu == MenuType::SAVEMENU || currentMenu == MenuType::LOADMENU) {
        auto eventR = saveLoadDialog.eventManager(event, window);
        if (eventR == SaveLoadDialog::BUTTONSIGNAL::PROCESS) {
            return (currentMenu == MenuType::SAVEMENU ? Signal::SAVEGAME : Signal::LOADGAME);
        } else if (eventR == SaveLoadDialog::BUTTONSIGNAL::CLOSE) {
            currentMenu = MenuType::MAINMENU;
        }
    } return Signal::NONE;
}

bool Menu::isClicked(const sf::RectangleShape& button, const sf::Vector2i& mousePos) noexcept {
    return static_cast<float>(mousePos.x) >= button.getPosition().x
           && static_cast<float>(mousePos.x) <= button.getPosition().x + button.getSize().x
           && static_cast<float>(mousePos.y) >= button.getPosition().y
           && static_cast<float>(mousePos.y) <= button.getPosition().y + button.getSize().y;
}

void Menu::saveGame(const Board& board) const {
    auto fileBuffer = std::ofstream(saveLoadDialog.getPath(), std::ios::binary);
    if (!fileBuffer.good()) { throw std::exception(); }
    for (auto iter = board.get(0); iter < board.get(getBoardSize()); ++iter) {
        auto x = static_cast<char>(
                iter->segmentState == Segment::State::HIGHLITED ? Segment::State::FREE : iter -> segmentState);
        fileBuffer << x;
    } fileBuffer << static_cast<char>(board.getPlayingWithComputer()) << static_cast<char>(board.getCurrentColor());
    fileBuffer.close();
}

Board Menu::loadGame() const {
    auto fileBuffer = std::ifstream(saveLoadDialog.getPath(), std::ios::binary);
    if (!fileBuffer.good()) { throw std::exception(); }
    auto board = Board();
    char bufferGet;
    for (auto iter = board.get(0); iter < board.get(getBoardSize()); ++iter) {
        fileBuffer >> bufferGet;
        if (std::ranges::find(Board::nonUsedSegments, iter->index) != Board::nonUsedSegments.end()) {
            continue;
        }
        if ((bufferGet >= Segment::enumSize || bufferGet < 0)) {
            throw std::logic_error("File is not a HEXXAGON save!");
        }
        iter->segmentState = static_cast<Segment::State>(bufferGet);
        if (iter->segmentState == Segment::State::HIGHLITED || iter->segmentState == Segment::State::NONUSED) {
            throw std::logic_error("Save is corrupted!");
        }
    }
    board.setPlayingWithComputer(fileBuffer.get());
    board.setCurrentColor(static_cast<Segment::State>(fileBuffer.get()));
    if (board.getCurrentColor() != Segment::State::BLUE && board.getCurrentColor() != Segment::State::RED) {
        throw std::logic_error("Save's attributes are corrupted!");
    }
    fileBuffer.close();
    return board;
}

void Menu::setCurrentMenu(const Menu::MenuType& cm) noexcept { currentMenu = cm; }

void Menu::setPathError(const std::string& description) { saveLoadDialog.setPathError(description); }

Menu::MenuType Menu::getCurrentMenu() const noexcept { return currentMenu; }
