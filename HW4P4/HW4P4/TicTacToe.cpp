//
//  TicTacToe.cpp
//  Midterm1
//
//  Created by Matt on 10/24/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "TicTacToe.h"

// Parameterized Constructor
TicTacToe::TicTacToe(GameBoard newboard, std::vector<std::vector<size_t>> newperm) : board(newboard), permutationsBoard(newperm) {
    if (!isValidBoard(newboard)) {
        throw std::invalid_argument("Invalid board file for TicTacToe.");
    }
    maxNode = calcMaxNode();
}

// Copy Constructor
TicTacToe::TicTacToe(const TicTacToe& other) {
    this->board = other.board;
    this->permutationsBoard = other.permutationsBoard;
    maxNode = calcMaxNode();
}

// Print board to ostream
void TicTacToe::printBoard(std::ostream&) {
    for (auto& i : board) {
        for (auto j : i) {
            char temp;
            if (j == EMPTY) {
                temp = '_';
            }
            else if (j == X) {
                temp = 'X';
            }
            else if (j == O) {
                temp = 'O';
            }
            std::cout << temp << " ";
        }
        std::cout << std::endl;
    }
}

// Calculate whether this is a max or min node
bool TicTacToe::calcMaxNode() {
    long long xcount = 0;
    long long ocount = 0;
    for (size_t i=0; i<board.size(); i++) {
        for (size_t j=0; j<board[i].size(); j++) {
            if (board[i][j] == X) {
                xcount++;
            }
            else if (board[i][j] == O) {
                ocount++;
            }
        }
    }
    return xcount <= ocount;
}

// MinMaxSearch Functions
// Return if this is a max or min node
bool TicTacToe::isMaxNode() {
    return this->maxNode;
}

// Return if this is a leaf node
bool TicTacToe::isLeaf() {
    if (Eval() != EMPTY) {
        return true;
    }
    else {
        return isBoardFull();
    }
}

// Return a win evaluation for this node
long long TicTacToe::Eval() {
    bool maxSolution = false;
    bool minSolution = false;
    // Check the board against the permutations board
    // to find solutions
    for (auto& i : permutationsBoard) {
        bool foundMax = true;
        bool foundMin = true;
        for (size_t row=0, col=i[row]; row<board.size(); row++, col=i[row]) {
            foundMax &= board[row][col] == X ? true : false;
            foundMin &= board[row][col] == O ? true : false;
        }
        if (foundMax) {
            maxSolution = true;
        }
        else if (foundMin) {
            minSolution = true;
        }
    }
    
    if (maxSolution) {
        return static_cast<long long>(X);
    }
    else if (minSolution) {
        return static_cast<long long>(O);
    }
    else {
        return static_cast<long long>(EMPTY);
    }
}

// Alpha-Beta Static Evaluation Function
long long TicTacToe::EvalAB() {
    long long evalres = Eval();
    if (evalres == X) {
        return 100;
    }
    else if (evalres == O) {
        return -100;
    }
    else {
        return countWinWays(X) - countWinWays(O);
    }
}

// Count the number of ways a player can win
long long TicTacToe::countWinWays(XO player) {
    size_t winCount = 0;
    for (auto& i : permutationsBoard) {
        bool foundMax = true;
        for (size_t row=0, col=i[row]; row<board.size(); row++, col=i[row]) {
            foundMax &= (board[row][col] == player) || (board[row][col] == EMPTY) ? true : false;
        }
        if (foundMax) {
            winCount++;
        }
    }
    return winCount;
}

// Generate the neighbors of this TicTacToe node
std::vector<std::shared_ptr<TicTacToe::TicTacToe>> TicTacToe::neighbors() {
    XO player = isMaxNode() ? X : O;
    std::vector<std::shared_ptr<TicTacToe>> candidates;
    
    for (size_t i=0; i<board.size(); i++) {
        for (size_t j=0; j<board[i].size(); j++) {
            if (board[i][j] == EMPTY) {
                GameBoard newboard = this->board;
                newboard[i][j] = player;
                std::shared_ptr<TicTacToe> temp(new TicTacToe(newboard, permutationsBoard));
                candidates.push_back(temp);
            }
        }
    }
    
    return candidates;
}

// Test whether a given board is valid or not
bool TicTacToe::isValidBoard(GameBoard& board) {
    long long xcount = 0;
    long long ocount = 0;
    for (size_t i=0; i<board.size(); i++) {
        for (size_t j=0; j<board[i].size(); j++) {
            if (board[i][j] == X) {
                xcount++;
            }
            else if (board[i][j] == O) {
                ocount++;
            }
        }
    }
    
    return std::abs(xcount - ocount) <= 1;
}

// Test whether a given board is full or not
bool TicTacToe::isBoardFull() {
    for (auto& i : board) {
        for (auto j : i) {
            if (j == EMPTY) {
                return false;
            }
        }
    }
    return true;
}

void TicTacToe::setLabel(long long newlabel) {
    // Transform to binary
    if (newlabel >= 0.9f) {
        //this->label = 1.0f;
        label.push_back(0.0f);
        label.push_back(1.0f);
    }
    else if (newlabel <= -0.9f) {
        //this->label = -1.0f;
        label.push_back(1.0f);
        label.push_back(1.0f);
    }
    else {
        //this->label = 0.0f;
        label.push_back(0.0f);
        label.push_back(0.0f);
    }
}

void TicTacToe::setLabel(std::vector<double> newlabel) {
    this->label = newlabel;
}

std::vector<double> TicTacToe::getLabel() {
    return this->label;
}

std::vector<std::vector<TicTacToe::XO>> TicTacToe::getGameBoard() {
    return this->board;
}