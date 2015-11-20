//
//  main.cpp
//  Midterm1
//
//  Created by Matt on 10/24/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

// Neural network from: takinginitiative.wordpress.co.za by Bobby Anguelov (banguelov@cs.up.ac.za)
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "MinMaxSearch.h"
#include "TicTacToe.h"
#include "neuralNetwork.h"

std::vector<std::shared_ptr<TicTacToe>> runSampleCases();
void runMenu();
void runFileMenu();
TicTacToe::GameBoard readFile();
TicTacToe::XO raviToSmithConverter(long long val);
void runTicTacToe(TicTacToe::GameBoard& board);
std::vector<std::shared_ptr<TicTacToe>> runTicTacToeAB(TicTacToe::GameBoard& board);
std::vector<std::vector<size_t>> generatePermutations(TicTacToe::GameBoard& board);
std::vector<dataEntry*> convertToDataEntry(std::vector<std::shared_ptr<TicTacToe>>& boards);

class DummyData {
    public:
        std::vector<double> features;
        std::vector<double> labels;
        DummyData(std::vector<double> labels, std::vector<double> features) : labels(labels), features(features) {
        
        }
    
        std::vector<double> getLabelsVector() {
            return this->labels;
        }
    
        std::vector<double> getFeaturesVector() {
            return this->features;
        }
};

void printVector(std::vector<double> vctr) {
    std::cout << std::fixed << std::setprecision(10);
    for (auto& i : vctr) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}

int main(int argc, const char * argv[]) {
    //runMenu();
    
    // Create the nn
    // input size = 16
    // neuron size = ??
    // output size = 1
    neuralNetwork nn(16, 25, 1);
    // Learning rate = 0.25
    // Momentum = 0.8
    nn.setLearningParameters(0.05, 0.8);
    // Desired 90% accuracy
    nn.setDesiredAccuracy(90);
    // 2000 iterations?
    nn.setMaxEpochs(2000);

    std::vector<std::shared_ptr<TicTacToe>> boards = runSampleCases();
    std::vector<dataEntry*> data = convertToDataEntry(boards);
    
    // Train network, probably need different datasets for each of these
    nn.trainNetwork(data, data, data);
    
    // See how we did
    for (size_t i=0; i<data.size(); i++) {
        double* out = nn.feedInput(data[i]->pattern);
        std::cout << "i = " << i << ", input = " << data[i]->target[0] << std::endl;
        std::cout << "i = " << i << ", output = " << nn.getRoundedOutputValue(*out) << std::endl;
    }
    
    return 0;
}

// Convert my awesome enumerated board types to a dataEntry* vector
std::vector<dataEntry*> convertToDataEntry(std::vector<std::shared_ptr<TicTacToe>>& boards) {
    std::vector<dataEntry*> result;
    for (auto& i : boards) {
        std::vector<std::vector<TicTacToe::XO>> board = i->getGameBoard();
        double* pattern = new double[board.size()*board[0].size()];
        double* target = new double[1];
        target[0] = i->getLabel();
        size_t count=0;
        for (auto& x : board) {
            for (auto& y : x) {
                pattern[count] = y;
                count++;
            }
        }
        dataEntry* entry = new dataEntry(pattern, target);
        result.push_back(entry);
    }
    
    return result;
}

// Run TicTacToe minmax with alpha beta for given board
std::vector<std::shared_ptr<TicTacToe>> runTicTacToeAB(TicTacToe::GameBoard& board) {
    MinMaxSearch<TicTacToe> search(6);
    std::vector<std::vector<size_t>> permBoard = generatePermutations(board);
    TicTacToe game(board, permBoard);
    std::cout << "-- Alpha Beta Pruning -- " << std::endl;
    std::cout << "Initial Board:" << std::endl;
    game.printBoard(std::cout);
    std::cout << "Min Max AB Result = " << search.runMinMaxABSearch(game) << std::endl;
    std::cout << "Explored = " << search.getLastExploredCount() << std::endl;
    std::cout << "Move Made:" << std::endl;
    search.getMoveNode()->printBoard(std::cout);
    std::cout << std::endl;
    
    return search.labeledData;
}

// Generate the solutions for a given tic tac toe board
std::vector<std::vector<size_t>> generatePermutations(TicTacToe::GameBoard& board) {
    std::vector<std::vector<size_t>> permboard;
    std::vector<size_t> positions;
    positions.resize(board.size());
    for (size_t i=0; i<positions.size(); i++) {
        positions[i] = i;
    }
    
    do {
        std::vector<size_t> newrow;
        for (size_t j=0; j<board.size(); j++) {
            newrow.push_back(positions[j]);
        }
        permboard.push_back(newrow);
    } while (std::next_permutation(positions.begin(), positions.end()));

    return permboard;
}

// Test Cases from piazza PDF
std::vector<std::shared_ptr<TicTacToe>> runSampleCases() {
    TicTacToe::GameBoard test_case0 = {
        {TicTacToe::O, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::O},
        {TicTacToe::O, TicTacToe::X, TicTacToe::EMPTY, TicTacToe::X},
        {TicTacToe::O, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::X},
        {TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::X}
    };
    
    TicTacToe::GameBoard test_case1 = {
        {TicTacToe::X, TicTacToe::O, TicTacToe::EMPTY, TicTacToe::EMPTY},
        {TicTacToe::X, TicTacToe::EMPTY, TicTacToe::O, TicTacToe::EMPTY},
        {TicTacToe::O, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::X},
        {TicTacToe::O, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::X}
    };
    
    TicTacToe::GameBoard test_case2 = {
        {TicTacToe::X, TicTacToe::O, TicTacToe::O, TicTacToe::EMPTY},
        {TicTacToe::X, TicTacToe::X, TicTacToe::O, TicTacToe::EMPTY},
        {TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::EMPTY},
        {TicTacToe::O, TicTacToe::X, TicTacToe::EMPTY, TicTacToe::EMPTY}
    };
    
    TicTacToe::GameBoard test_case3 = {
        {TicTacToe::O, TicTacToe::EMPTY, TicTacToe::O, TicTacToe::EMPTY},
        {TicTacToe::X, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::X},
        {TicTacToe::X, TicTacToe::O, TicTacToe::EMPTY, TicTacToe::EMPTY},
        {TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::X}
    };
    
    // Sample from Midterm PDF. Unused
    TicTacToe::GameBoard test_case4 = {
        {TicTacToe::X, TicTacToe::O, TicTacToe::X, TicTacToe::O},
        {TicTacToe::O, TicTacToe::X, TicTacToe::EMPTY, TicTacToe::EMPTY},
        {TicTacToe::X, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::EMPTY},
        {TicTacToe::O, TicTacToe::EMPTY, TicTacToe::EMPTY, TicTacToe::EMPTY}
    };
    
    std::vector<TicTacToe::GameBoard> testCases;
    testCases.push_back(test_case0);
    testCases.push_back(test_case1);
    testCases.push_back(test_case2);
    testCases.push_back(test_case3);
    
    std::vector<std::shared_ptr<TicTacToe>> result;
    
    for (size_t i=0; i<testCases.size(); i++) {
        std::cout << "=== Test Case " << i << " ===" << std::endl;
        std::vector<std::shared_ptr<TicTacToe>> temp = runTicTacToeAB(testCases[i]);
        for (auto& i : temp) {
            result.push_back(i);
        }
    }
    return result;
}

// Run TicTacToe minmax for given board
void runTicTacToe(TicTacToe::GameBoard& board) {
    MinMaxSearch<TicTacToe> search(6);
    std::vector<std::vector<size_t>> permBoard = generatePermutations(board);
    TicTacToe game(board, permBoard);
    std::cout << "-- Minmax Algorithm -- " << std::endl;
    std::cout << "Initial Board:" << std::endl;
    game.printBoard(std::cout);
    std::cout << "Min Max Result = " << search.runMinMaxSearch(game) << std::endl;
    std::cout << "Explored = " << search.getLastExploredCount() << std::endl;
    std::cout << std::endl;
}

