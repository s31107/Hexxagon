#pragma once
#include "../Board/Board.h"
#include <cstdint>


struct scoreContainer {
    uint16_t from;
    uint16_t to;
    int score;
};

[[nodiscard]] scoreContainer calculateMove(Board board, const uint16_t& step = 0) noexcept;
