#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <filesystem>


class SaveLoadDialog {
    struct Button {
        sf::RectangleShape rect{};
        sf::Text text{};
    };
    struct LineEdit {
        sf::RectangleShape rect{};
        sf::Text text{};
    };
    sf::Font font{};
    sf::RectangleShape mainRect{};
    sf::Text title{};
    std::array<Button, 2> buttons{};
    LineEdit lineEdit{};
    bool isSaving = false;
    static bool isClicked(const sf::RectangleShape& button, const sf::Vector2i& mousePos) noexcept;
    bool validatePath() const;
    void configureLineEdit();
public:
    enum class BUTTONSIGNAL { NONE, CLOSE, PROCESS };
    SaveLoadDialog();
    void draw(sf::RenderWindow& window);
    void resize(const sf::Vector2u& windowSizes);
    BUTTONSIGNAL eventManager(const sf::Event& event, sf::RenderWindow& window);
    void setIsSaving(const bool& sis) noexcept;
    [[nodiscard]] std::filesystem::path getPath() const;
    void setPathError(const std::string& description);
};
