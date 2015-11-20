//
//  TicTacToe.h
//  Midterm1
//
//  Created by Matt on 10/24/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include <vector>
#include <memory>
#include <iostream>

class TicTacToe {
    public:
        // Enumeration for positions on a TicTacToe board
        enum XO {
            X = 1,
            O = -1,
            EMPTY = 0
        };
        typedef std::vector<std::vector<XO>> GameBoard;
    
    private:
        // Board holding the game state
        GameBoard board;
    
        // Board holding all the possible solutions for this
        // board size
        std::vector<std::vector<size_t>> permutationsBoard;
    
        // Boolean representing whether this is a max node or not
        bool maxNode;
    
        // Win/lose/draw label
        double label;
    
        // Function to calculate whether this is a max node or not
        bool calcMaxNode();
    
        // Function to test whether a given board is valid
        bool isValidBoard(GameBoard&);
    
        // Function to test whether this node's board is full
        bool isBoardFull();
    
        // Function to count how many ways a player can win at this node
        long long countWinWays(XO);
    
    public:
        // Parameterized Constructor, Board and Solution Board
        TicTacToe(std::vector<std::vector<XO>>, std::vector<std::vector<size_t>>);
    
        // Copy Constructor
        TicTacToe(const TicTacToe&);
    
        // Function to print this node's board to an ostream
        void printBoard(std::ostream&);
    
        // MinMaxSearch Functions
        // Returns whether this node is a max node or not
        bool isMaxNode();
    
        // Returns whether this node is a leaf node or not
        bool isLeaf();
    
        // Returns a static evaluation of this node
        long long Eval();
    
        // Returns a static evaluation of this node for Alpha-Beta pruning
        long long EvalAB();
    
        // Returns a vector of neighbors of this node
        std::vector<std::shared_ptr<TicTacToe>> neighbors();
    
        // Set labeled data entry
        void setLabel(long long newlabel);
    
        double getLabel();
    
        std::vector<std::vector<XO>> getGameBoard();
};