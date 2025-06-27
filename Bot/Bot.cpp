#include "Bot.h"
#include <algorithm>

#define RECURSIONDEPTH 1


static scoreContainer makeMove(Board board, const uint16_t& index1, const uint16_t& index2,
                               const uint16_t& step) noexcept {
    auto figure = board.get(index1);
    auto move = board.get(index2);

    auto currentScore = scoreContainer{index1, index2, 0};
    auto tVec = board.getAdjacentFields(figure);
    auto playerColour = board.getCurrentColor();
    board.setCurrentColor(board.getCurrentColor() == Segment::State::BLUE ? Segment::State::RED : Segment::State::BLUE);

    if (std::ranges::find(tVec, move) != tVec.end()) { ++currentScore.score; }
    else { figure->segmentState = Segment::State::FREE; }
    move->segmentState = playerColour;
    for (auto field : board.getAdjacentFields(move)) {
        if (field->segmentState == board.getCurrentColor()) {
            field->segmentState = playerColour;
            ++currentScore.score;
        }
    }
    if (step != RECURSIONDEPTH) {
        currentScore.score -= calculateMove(board, step + 1).score;
    } return currentScore;
}

scoreContainer calculateMove(Board board, const uint16_t& step) noexcept {
    auto maxScore = scoreContainer{0, 0, 0};
    bool isEmpty = true;
    for (auto figure : board.getAll(board.getCurrentColor())) {
        for (auto move : board.getPossibleMoves(figure)) {
            auto rMove = makeMove(board, figure->index, move->index, step);
            if (isEmpty || maxScore.score < rMove.score) {
                isEmpty = false;
                maxScore = rMove;
            }
        }
    } return maxScore;
}
