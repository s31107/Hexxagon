#include "SaveLoadDialog.h"

#define FIGURETHICKNESS 0.05f
#define WINDOWPROPORTIONX1 9.f
#define WINDOWPROPORTIONX2 10.f
#define WINDOWPROPORTIONY1 7.f
#define WINDOWPROPORTIONY2 10.f

#define TEXTPROPORTIONX1 1.f
#define TEXTPROPORTIONY1 1.f
#define TEXTPROPORTIONX2 20.f
#define TEXTPROPORTIONY2 20.f

#define LINEEDITPROPORTIONX1 9.f
#define LINEEDITPROPORTIONX2 10.f
#define LINEEDITPROPORTIONY1 2.f
#define LINEEDITPROPORTIONY2 15.f

#define LINEEDITTEXTPROPORTIONX1 8.f
#define LINEEDITTEXTPROPORTIONX2 10.f
#define LINEEDITTEXTPROPORTIONY1 5.f
#define LINEEDITTEXTPROPORTIONY2 10.f

#define BUTTONPROPORTIONX1 2.f
#define BUTTONPROPORTIONX2 10.f
#define BUTTONPROPORTIONY1 1.f
#define BUTTONPROPORTIONY2 10.f

#define BUTTONTEXTPROPORTIONX1 1.f
#define BUTTONTEXTPROPORTIONY1 1.f
#define BUTTONTEXTPROPORTIONX2 2.f
#define BUTTONTEXTPROPORTIONY2 2.f

#define MAXCHARACTERSNUMBER 100


SaveLoadDialog::SaveLoadDialog() {
    if (!font.loadFromFile("../NotoSans-Bold.ttf")) {
        throw std::runtime_error("Font was not found!");
    }
    title.setFont(font);
    lineEdit.text.setFont(font);
    for (decltype(auto) iter : buttons) {
        iter.text.setFont(font);
        iter.rect.setFillColor(sf::Color(25,64,255));
        iter.rect.setOutlineColor(sf::Color(0,191,230));
    }
    mainRect.setFillColor(sf::Color(255,179,102));
    mainRect.setOutlineColor(sf::Color(230,153,0));
    lineEdit.rect.setFillColor(sf::Color::White);
    lineEdit.rect.setOutlineColor(sf::Color(102,255,25));
    lineEdit.text.setFillColor(sf::Color::Black);
    buttons[0].text.setString("Close");
    buttons[1].text.setString("OK");
    title.setString("Insert Path:");
}

void SaveLoadDialog::draw(sf::RenderWindow& window) {
    window.draw(mainRect);
    window.draw(title);
    window.draw(lineEdit.rect);
    window.draw(lineEdit.text);
    for (decltype(auto) iter : buttons) {
        window.draw(iter.rect);
        window.draw(iter.text);
    }
}

void SaveLoadDialog::resize(const sf::Vector2u& windowSizes) {
    // Number of components + 1:
    constexpr static uint16_t numberOfSpaces = 4.f;
    // Setting sizes of widgets:
    mainRect.setSize({WINDOWPROPORTIONX1 * static_cast<float>(windowSizes.x) / WINDOWPROPORTIONX2,
                      WINDOWPROPORTIONY1 * static_cast<float>(windowSizes.y) / WINDOWPROPORTIONY2});
    mainRect.setPosition((static_cast<float>(windowSizes.x) - mainRect.getSize().x) / 2.f,
                         (static_cast<float>(windowSizes.y) - mainRect.getSize().y) / 2.f);
    mainRect.setOutlineThickness(mainRect.getSize().y * FIGURETHICKNESS);
    title.setCharacterSize(static_cast<unsigned int>(TEXTPROPORTIONX1 * std::min(mainRect.getSize().x
        / TEXTPROPORTIONX2, TEXTPROPORTIONY1 * mainRect.getSize().y / TEXTPROPORTIONY2)));
    lineEdit.rect.setSize({LINEEDITPROPORTIONX1 * mainRect.getSize().x / LINEEDITPROPORTIONX2,
                           LINEEDITPROPORTIONY1 * mainRect.getSize().y / LINEEDITPROPORTIONY2});
    lineEdit.rect.setOutlineThickness(lineEdit.rect.getSize().y * FIGURETHICKNESS);
    for (decltype(auto) button : buttons) {
        button.rect.setSize(sf::Vector2f{BUTTONPROPORTIONX1 * mainRect.getSize().x / BUTTONPROPORTIONX2,
                                         BUTTONPROPORTIONY1 * mainRect.getSize().y / BUTTONPROPORTIONY2});
        button.rect.setOutlineThickness(button.rect.getSize().y * FIGURETHICKNESS);
        button.text.setCharacterSize(static_cast<unsigned int>(std::min(BUTTONTEXTPROPORTIONX1
            * button.rect.getSize().x / BUTTONTEXTPROPORTIONX2, BUTTONTEXTPROPORTIONY1
            * button.rect.getSize().y / BUTTONTEXTPROPORTIONY2)));
    }
    // Positioning components:
    auto spaceBetweenWidgets = ((mainRect.getSize().y - (title.getLocalBounds().getSize().y
            + lineEdit.rect.getSize().y + buttons[0].rect.getSize().y + 2.f * (buttons[0].rect.getOutlineThickness()
            + lineEdit.rect.getOutlineThickness())))) / numberOfSpaces;
    title.setPosition(mainRect.getPosition().x + (mainRect.getSize().x - title.getLocalBounds().width) / 2.f,
                      mainRect.getPosition().y + spaceBetweenWidgets);
    lineEdit.rect.setPosition(mainRect.getPosition().x + (mainRect.getSize().x - lineEdit.rect.getSize().x) / 2.f,
                              title.getPosition().y + title.getLocalBounds().height + spaceBetweenWidgets);
    // Space between button and left edge of main rectangle (mainRect):
    const auto segmentSizeX = (mainRect.getSize().x / 2.f - buttons[0].rect.getSize().x) / 2.f;
    buttons[0].rect.setPosition(mainRect.getPosition().x + segmentSizeX, lineEdit.rect.getPosition().y
        + lineEdit.rect.getSize().y + spaceBetweenWidgets);
    buttons[1].rect.setPosition(mainRect.getPosition().x + mainRect.getSize().x / 2.f + segmentSizeX,
                                buttons[0].rect.getPosition().y);
    for (decltype(auto) button : buttons) {
        button.text.setOrigin(button.text.getLocalBounds().width / 2.f,
                              button.text.getLocalBounds().height / 2.f);
        button.text.setPosition(button.rect.getPosition().x + button.rect.getSize().x / 2.f,
                                button.rect.getPosition().y + button.rect.getSize().y / 2.f);
    }
    configureLineEdit();
}

SaveLoadDialog::BUTTONSIGNAL SaveLoadDialog::eventManager(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        auto mousePos = sf::Mouse::getPosition(window);
        if (isClicked(buttons[1].rect, mousePos)) {
            if (!validatePath()) {
                setPathError("Selected path isn't appropriate!");
                return BUTTONSIGNAL::NONE;
            }
            return BUTTONSIGNAL::PROCESS;
        } else if (isClicked(buttons[0].rect, mousePos)) {
            return BUTTONSIGNAL::CLOSE;
        }
    } else if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == '\b') {
            auto str = static_cast<std::string>(lineEdit.text.getString());
            if (!str.empty()) {
                str.pop_back();
                lineEdit.text.setString(str);
            } else { return BUTTONSIGNAL::NONE; }
        } else {
            if (lineEdit.text.getString().getSize() >= MAXCHARACTERSNUMBER) { return BUTTONSIGNAL::NONE; }
            lineEdit.text.setString(lineEdit.text.getString() + static_cast<char>(event.text.unicode));
        }
        if (title.getString() != "Insert Path:") {
            title.setString("Insert Path:");
            // Setting alignment of title:
            title.setPosition(mainRect.getPosition().x + (mainRect.getSize().x - title.getLocalBounds().width) / 2.f,
                              title.getPosition().y);
        }
        configureLineEdit();
    } return BUTTONSIGNAL::NONE;
}

bool SaveLoadDialog::isClicked(const sf::RectangleShape& button, const sf::Vector2i& mousePos) noexcept {
    return static_cast<float>(mousePos.x) >= button.getPosition().x
           && static_cast<float>(mousePos.x) <= button.getPosition().x + button.getSize().x
           && static_cast<float>(mousePos.y) >= button.getPosition().y
           && static_cast<float>(mousePos.y) <= button.getPosition().y + button.getSize().y;
}

void SaveLoadDialog::configureLineEdit() {
    lineEdit.rect.setOutlineColor(sf::Color(102,255,25));
    lineEdit.text.setCharacterSize(static_cast<unsigned int>(std::min(
            LINEEDITTEXTPROPORTIONX1 * lineEdit.rect.getSize().x / LINEEDITTEXTPROPORTIONX2,
            LINEEDITTEXTPROPORTIONY1 * lineEdit.rect.getSize().y / LINEEDITTEXTPROPORTIONY2)));
    if (lineEdit.text.getLocalBounds().width >= lineEdit.rect.getSize().x) {
        lineEdit.text.setCharacterSize(static_cast<unsigned int>(lineEdit.rect.getSize().x
        * static_cast<float>(lineEdit.text.getCharacterSize()) / lineEdit.text.getLocalBounds().width));
    }
    lineEdit.text.setOrigin(lineEdit.text.getLocalBounds().width / 2.f,
                            lineEdit.text.getLocalBounds().height / 2.f);
    lineEdit.text.setPosition(lineEdit.rect.getPosition().x + lineEdit.rect.getSize().x / 2.f,
                              lineEdit.rect.getPosition().y + lineEdit.rect.getSize().y / 2.f);
}

bool SaveLoadDialog::validatePath() const {
    using namespace std::filesystem;
    auto workingPath = getPath();
    if (isSaving) { return !exists(workingPath) && exists(workingPath.root_path()); }
    else { return is_regular_file(workingPath); }
}

void SaveLoadDialog::setPathError(const std::string& description) {
    title.setString(description);
    title.setPosition(mainRect.getPosition().x + (mainRect.getSize().x - title.getLocalBounds().width) / 2.f,
                      title.getPosition().y);
    lineEdit.rect.setOutlineColor(sf::Color::Red);
}

std::filesystem::path SaveLoadDialog::getPath() const {
    return std::filesystem::absolute(std::filesystem::path(lineEdit.text.getString()));
}

void SaveLoadDialog::setIsSaving(const bool& sis) noexcept { isSaving = sis; }
