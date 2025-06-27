#include "Board.h"


Board::Board() {
    newRows.at(0) = 0;
    for (uint16_t i = 1; i < SIDELENGTH + 1; ++i) {
        newRows.at(i) = newRows.at(i - 1) + SIDELENGTH + i - 1;
    }
    for (uint16_t i = SIDELENGTH + 1, e = SIDELENGTH - 2; i < static_cast<uint16_t>(newRows.size()); ++i, --e) {
        newRows.at(i) = newRows.at(i - 1) + SIDELENGTH + e;
    }
    for (decltype(auto) iter : nonUsedSegments) {
        board.at(iter).segmentState = Segment::State::NONUSED;
    }
    for (uint16_t i = 0; i < static_cast<uint16_t>(board.size()); ++i) { board.at(i).index = i; }
}

SegmentP Board::get(const uint16_t& index) noexcept { return board.begin() + index; }

SegmentPC Board::get(const uint16_t& index) const noexcept { return board.cbegin() + index; }

std::vector<SegmentP> Board::getAll(const Segment::State& state) noexcept {
    auto rVec = std::vector<SegmentP>{};
    for (auto ptr = board.begin(); ptr != board.end(); ++ptr) {
        if (ptr -> segmentState == state) { rVec.push_back(ptr); }
    } return rVec;
}


std::set<SegmentP> Board::getPossibleMoves(SegmentP actualField) noexcept {
    auto rSet = std::set<SegmentP>{};
    for (auto iter : getAllAdjacentFields(actualField)) {
        for (auto possibleMoves : getAdjacentFields(iter)) {
            if ((possibleMoves->segmentState == Segment::State::FREE
                || possibleMoves->segmentState == Segment::State::HIGHLITED) && possibleMoves != actualField) {
                rSet.insert(possibleMoves);
            }
        }
    } return rSet;
}

std::vector<SegmentP> Board::getAdjacentFields(SegmentP actualField) noexcept {
    auto rVec = getAllAdjacentFields(actualField);
    std::erase_if(rVec, [](auto elem) -> bool {
        return elem->segmentState == Segment::State::NONUSED;
    });
    return rVec;
}

std::vector<SegmentP> Board::getAllAdjacentFields(SegmentP actualField) noexcept {
    uint16_t rowNum = 0, d, l2, l4;
    bool isMoreElementsInNextRow, isMoreElementsInPrevRow;
    auto rVec = std::vector<SegmentP>{};

    for (decltype(auto) iter : newRows) {
        if (iter <= actualField->index) { ++rowNum; }
        else { --rowNum; break; }
    }

    isMoreElementsInNextRow = rowNum + 1 <= SIDELENGTH - 1;
    isMoreElementsInPrevRow = rowNum - 1 >= SIDELENGTH - 1;
    d = actualField->index - newRows.at(rowNum);

    if (rowNum != SIDELENGTH * 2 - 2) {
        if (d - !isMoreElementsInNextRow != -1) {
            rVec.push_back(get(newRows.at(rowNum + 1) + d - !isMoreElementsInNextRow));
        }
        l2 = newRows.at(rowNum + 1) + d - !isMoreElementsInNextRow + 1;
        if (l2 != newRows.at(rowNum + 2)) {
            rVec.push_back(get(l2));
        }
    }
    if (rowNum != 0) {
        if (d - !isMoreElementsInPrevRow != -1) {
            rVec.push_back(get(newRows.at(rowNum - 1) + d - !isMoreElementsInPrevRow));
        }
        l4 = newRows.at(rowNum - 1) + d - !isMoreElementsInPrevRow + 1;
        if (l4 != newRows.at(rowNum)) {
            rVec.push_back(get(l4));
        }
    }
    if (actualField->index + 1 != newRows.at(rowNum + 1)) {
        rVec.push_back(get(actualField->index + 1));
    }
    if (actualField->index != 0 && (rowNum == 0 || actualField->index != newRows.at(rowNum))) {
        rVec.push_back(get(actualField->index - 1));
    }
    return rVec;
}

void Board::clear() noexcept {
    for (decltype(auto) iter : board) {
        if (iter.segmentState != Segment::State::NONUSED) {
            iter.segmentState = Segment::State::FREE;
        }
    }
}

void Board::clearHighlightedElements() noexcept {
    for (auto segment : getAll(Segment::State::HIGHLITED)) {
        segment->segmentState = Segment::State::FREE;
    }
}

bool Board::getPlayingWithComputer() const noexcept { return playingWithComputer; }

void Board::setPlayingWithComputer(const bool& pwc) noexcept { playingWithComputer = pwc; }

Segment::State Board::getCurrentColor() const noexcept { return currentColor; }

void Board::setCurrentColor(const Segment::State& cc) noexcept { currentColor = cc; }
