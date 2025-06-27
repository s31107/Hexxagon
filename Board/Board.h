#pragma once
#include <vector>
#include <array>
#include <cstdint>
#include <set>

#define SIDELENGTH 5


constexpr uint16_t getBoardSize() {
    uint16_t sum = 0;
    for (uint16_t i = 0; i < SIDELENGTH; ++i) {
        if (i == SIDELENGTH - 1) { sum *= 2; }
        sum += SIDELENGTH + i;
    }
    return sum;
}

struct Segment {
    uint16_t index{};
    static constexpr auto enumSize = 5;
    enum class State {NONUSED, FREE, RED, BLUE, HIGHLITED};
    State segmentState = State::FREE;
};

using SegmentP = std::array<Segment, getBoardSize()>::iterator;
using SegmentPC = std::array<Segment, getBoardSize()>::const_iterator;

class Board {
    std::array<Segment, getBoardSize()> board{};
    std::array<uint16_t , SIDELENGTH * 2> newRows{};
    std::vector<SegmentP> getAllAdjacentFields(SegmentP actualField) noexcept;
    bool playingWithComputer = false;
    Segment::State currentColor = Segment::State::RED;
public:
    static constexpr auto nonUsedSegments = {29, 22, 39};
    Board();
    [[nodiscard]] SegmentP get(const uint16_t& index) noexcept;
    [[nodiscard]] SegmentPC get(const uint16_t& index) const noexcept;
    [[nodiscard]] std::vector<SegmentP> getAdjacentFields(SegmentP actualField) noexcept;
    [[nodiscard]] std::set<SegmentP> getPossibleMoves(SegmentP actualField) noexcept;
    [[nodiscard]] std::vector<SegmentP> getAll(const Segment::State& state) noexcept;
    void clear() noexcept;
    void clearHighlightedElements() noexcept;
    [[nodiscard]] bool getPlayingWithComputer() const noexcept;
    void setPlayingWithComputer(const bool& pwc) noexcept;
    [[nodiscard]] Segment::State getCurrentColor() const noexcept;
    void setCurrentColor(const Segment::State& cc) noexcept;
};
