#include "MainWindow.h"

#define SCREENX 1600
#define SCREENY 900
#define SCREENX2 1680
#define SCREENY2 1050


MainWindow::MainWindow() : sf::RenderWindow(sf::VideoMode(
        sf::VideoMode::getDesktopMode().width * SCREENX / SCREENX2,
        sf::VideoMode::getDesktopMode().height * SCREENY / SCREENY2),"HEXXAGON",sf::Style::Default,
                                            sf::ContextSettings(0, 0, 8)) {}

void MainWindow::show() {
    clear(sf::Color::White);
    board.draw(*this);
    menu.draw(*this);
    if (isEndGame) { endGameDialog.draw(*this); }
    display();
}

void MainWindow::eventManagement() {
    sf::Event event{};
    while (pollEvent(event)) {
        if (event.type == sf::Event::Closed) { close(); }
        if (event.type == sf::Event::Resized) {
            board.resize(getSize());
            menu.resize(getSize());
            endGameDialog.resize(getSize());
            setView(sf::View(
                    sf::FloatRect(0, 0, static_cast<float>(getSize().x),
                                  static_cast<float>(getSize().y))));
        }
        if (!isEndGame) {
            if (menu.getCurrentMenu() == Menu::MenuType::NOMENU && board.eventManager(event, *this)) {
                const auto winner = (board.getScore(Segment::State::RED) > board.getScore(
                        Segment::State::BLUE) ? Segment::State::RED : Segment::State::BLUE);
                const auto isWinnerRed = (winner == Segment::State::RED);
                endGameDialog.setResult(!board.getPlayingWithComputer() || isWinnerRed, isWinnerRed,
                                        board.getScore(board.getPlayingWithComputer()
                                        ? Segment::State::RED : winner));
                if (board.getPlayingWithComputer() && isWinnerRed) { endGameDialog.saveScoreToFile(); }
                isEndGame = true;
            }
            switch (menu.eventManager(event, *this)) {
                case Menu::Signal::PLAYAI:
                    board.prepareBoard();
                    board.setPlayingWithComputer(true);
                    break;
                case Menu::Signal::PLAYHOTSEAT:
                    board.prepareBoard();
                    board.setPlayingWithComputer(false);
                    break;
                case Menu::Signal::SAVEGAME:
                    try {
                        menu.saveGame(board);
                        menu.setCurrentMenu(Menu::MenuType::NOMENU);
                    }
                    catch (const std::logic_error& exc) {
                        menu.setPathError(exc.what());
                    }
                    catch (...) { menu.setPathError("Path cannot be loaded!"); }
                    break;
                case Menu::Signal::LOADGAME:
                    try {
                        board = menu.loadGame();
                        board.setFillColors();
                        board.calculateScores();
                        menu.setCurrentMenu(Menu::MenuType::NOMENU);
                    }
                    catch (const std::logic_error& exc) {
                        menu.setPathError(exc.what());
                    }
                    catch (...) { menu.setPathError("Path cannot be loaded!"); }
                case Menu::Signal::NONE:
                    break;
            }
        } else if (endGameDialog.eventManager(event, *this)) { isEndGame = false; }
    }
}
