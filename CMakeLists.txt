cmake_minimum_required(VERSION 3.25)
project(Hexxagon)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS FALSE)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY  https://github.com/SFML/SFML
        GIT_TAG         2.6.1
)

FetchContent_MakeAvailable(SFML)

add_executable(Hexxagon
        main.cpp
        Board/Board.cpp
        Bot/Bot.cpp
        MainWindow/MainWindow.cpp
        MainWindow/SFML_Board.cpp
        MainWindow/Menu.cpp
        Dialogs/EndGame.cpp
        Dialogs/LeaderBoard.cpp
        Dialogs/SaveLoadDialog.cpp)
target_link_libraries(Hexxagon sfml-graphics)
