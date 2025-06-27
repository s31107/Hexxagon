#pragma once
#include "../Dialogs/LeaderBoard.h"
#include "../Dialogs/SaveLoadDialog.h"
#include "../Board/Board.h"
#include <SFML/Graphics.hpp>
#include <array>


struct Menu {
    enum class MenuType { NOMENU, MAINMENU, GAMEMENU, SAVEMENU, LOADMENU, LEADERBOARDS };
    enum class Signal { PLAYAI, PLAYHOTSEAT, SAVEGAME, LOADGAME, NONE };
    Menu();
    void draw(sf::RenderWindow& window);
    void resize(const sf::Vector2u& windowSizes) noexcept;
    void saveGame(const Board& board) const;
    Board loadGame() const;
    static void resizeSegments(
            sf::Vector2f windowSizes, std::vector<sf::RectangleShape>& buttons, std::vector<sf::Text>& texts) noexcept;
    Signal eventManager(const sf::Event& event, sf::RenderWindow& window);
    void setCurrentMenu(const MenuType& cm) noexcept;
    [[nodiscard]] MenuType getCurrentMenu() const noexcept;
    void setPathError(const std::string& description);
private:
    static bool isClicked(const sf::RectangleShape& button, const sf::Vector2i& mousePos) noexcept;
    sf::Font font{};
    std::vector<sf::RectangleShape> mainMenuSegments{};
    std::vector<sf::Text> mainMenuTexts{};
    std::vector<sf::RectangleShape> gameMenuSegments{};
    std::vector<sf::Text> gameMenuTexts{};
    LeaderBoard leaderBoard{};
    SaveLoadDialog saveLoadDialog{};
    MenuType currentMenu = MenuType::MAINMENU;
};
