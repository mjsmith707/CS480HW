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
#include <limits>
#include "MinMaxSearch.h"
#include "TicTacToe.h"
#include "neuralNetwork.h"

std::vector<std::shared_ptr<TicTacToe>> runSampleCases();
void runMenu(neuralNetwork& nn);
void runFileMenu(neuralNetwork& nn);
TicTacToe::GameBoard readFile();
TicTacToe::XO raviToSmithConverter(long long val);
void runTicTacToe(TicTacToe::GameBoard& board);
std::vector<std::shared_ptr<TicTacToe>> runTrainingBoard(TicTacToe::GameBoard& board);
void runTicTacToeAB(TicTacToe::GameBoard& board);
void runTicTacToeABNN(neuralNetwork& nn, TicTacToe::GameBoard& board);
std::vector<std::vector<size_t>> generatePermutations(TicTacToe::GameBoard& board);
std::vector<dataEntry*> convertToDataEntry(std::vector<std::shared_ptr<TicTacToe>>& boards);


int main(int argc, const char * argv[]) {
    // Create the neural network
    // input size = 16
    // neuron size = ??
    // output size = 2 binary coded
    neuralNetwork nn(16, 16, 2);
    // Learning rate = 0.25
    // Momentum = 0.8
    nn.setLearningParameters(0.05, 0.8);
    // Desired 90% accuracy
    nn.setDesiredAccuracy(90);
    // 2000 iterations?
    nn.setMaxEpochs(300);

    std::vector<std::shared_ptr<TicTacToe>> boards = runSampleCases();
    std::vector<dataEntry*> data = convertToDataEntry(boards);
    
    // Train network, probably need different datasets for each of these
    nn.trainNetwork(data, data, data);
    
    runMenu(nn);
    
    return 0;
}

// Root Level Menu
void runMenu(neuralNetwork& nn) {
    do {
        std::cout << "Enter a choice:" << std::endl;
        std::cout << "[1] Load and run file, [q] Quit" << std::endl;
        std::cout << "> ";
        char input = 0;
        if (std::cin >> input) {
            switch (input) {
                case '1': {
                    try {
                        runFileMenu(nn);
                    }
                    catch (std::runtime_error& e) {
                        std::cout << e.what() << std::endl;
                        break;
                    }
                    catch (std::invalid_argument& e) {
                        std::cout << e.what() << std::endl;
                        break;
                    }
                    
                    break;
                }
                case 'q': {
                case 'Q':
                    return;
                }
                default: {
                    std::cout << "Invalid choice selected!" << std::endl;
                    break;
                }
            }
        }
    } while (true);
}

// Run File Menu
void runFileMenu(neuralNetwork& nn) {
    TicTacToe::GameBoard loadedboard = readFile();
    std::cout << std::endl << "File Loaded." << std::endl;
    
    do {
        std::cout << "[1] Run Minmax, [2] Run Minmax AlphaBeta, [3] Run Minmax AB Neural Network [q] Return" << std::endl;
        std::cout << "> ";
        char input = 0;
        if (std::cin >> input) {
            switch (input) {
                case '1': {
                    runTicTacToe(loadedboard);
                    break;
                }
                case '2': {
                    runTicTacToeAB(loadedboard);
                    break;
                }
                case '3': {
                    runTicTacToeABNN(nn, loadedboard);
                    break;
                }
                case 'q': {
                case 'Q':
                    return;
                }
                default:
                    std::cout << "Invalid choice selected!" << std::endl;
                    break;
            }
        }
    } while (true);
}

// Read file from input
TicTacToe::GameBoard readFile() {
    std::cin.ignore();
    std::string input = "";
    std::cout << "Enter filename: ";
    if (std::getline(std::cin, input)) {
        std::fstream stream;
        stream.open(input);
        if (!stream.is_open()) {
            std::string error = "Failed to open file: ";
            error += input;
            throw std::runtime_error(error);
        }
        
        TicTacToe::GameBoard newboard;
        std::string line;
        while (std::getline(stream, line)) {
            std::stringstream sstream;
            sstream << line;
            long long val = 0;
            std::vector<TicTacToe::XO> newrow;
            while (sstream >> val) {
                newrow.push_back(raviToSmithConverter(val));
            }
            newboard.push_back(newrow);
        }
        return newboard;
    }
    else {
        throw std::runtime_error("Failed to read file name from stdin");
    }
}

// Convert from 0 = Empty, 1 = Player 1, 2 = Player 2 to
// TicTacToe::XO enumerations.
TicTacToe::XO raviToSmithConverter(long long val) {
    switch(val) {
        case 0: {
            return TicTacToe::EMPTY;
        }
        case 1: {
            return TicTacToe::X;
        }
        case 2: {
            return TicTacToe::O;
        }
        default:
            std::string error = "Invalid value in board: ";
            error += std::to_string(val);
            throw std::runtime_error(error);
    }
}

// Convert my awesome enumerated board types to a dataEntry* vector
std::vector<dataEntry*> convertToDataEntry(std::vector<std::shared_ptr<TicTacToe>>& boards) {
    std::vector<dataEntry*> result;
    for (auto& i : boards) {
        std::vector<std::vector<TicTacToe::XO>> board = i->getGameBoard();
        double* pattern = new double[board.size()*board[0].size()];
        double* target = new double[2];
        std::vector<double> labels = i->getLabel();
        target[0] = labels[0];
        target[1] = labels[1];
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
// Return a labeled set of training data to use
std::vector<std::shared_ptr<TicTacToe>> runTrainingBoard(TicTacToe::GameBoard& board) {
    MinMaxSearch<TicTacToe> search(std::numeric_limits<size_t>::max());
    std::vector<std::vector<size_t>> permBoard = generatePermutations(board);
    TicTacToe game(board, permBoard);
    search.runMinMaxABSearch(game);
    return search.labeledData;
}

// Run TicTacToe minmax with alpha beta for given board
void runTicTacToeAB(TicTacToe::GameBoard& board) {
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
}

// Run TicTacToe minmax with alpha beta neural network-backed for given board
void runTicTacToeABNN(neuralNetwork& nn, TicTacToe::GameBoard& board) {
    MinMaxSearch<TicTacToe> search(6);
    std::vector<std::vector<size_t>> permBoard = generatePermutations(board);
    TicTacToe game(board, permBoard);
    std::cout << "-- Alpha Beta Pruning Neural Network -- " << std::endl;
    std::cout << "Initial Board:" << std::endl;
    game.printBoard(std::cout);
    std::cout << "Min Max AB NN Result = " << search.runMinMaxABSearchNN(nn, game) << std::endl;
    std::cout << "Explored = " << search.getLastExploredCount() << std::endl;
    std::cout << "Move Made:" << std::endl;
    search.getMoveNode()->printBoard(std::cout);
    std::cout << std::endl;
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
    
    // Sample from Midterm PDF.
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
    testCases.push_back(test_case4);
    
    std::vector<std::shared_ptr<TicTacToe>> result;
    
    for (size_t i=0; i<testCases.size(); i++) {
        std::vector<std::shared_ptr<TicTacToe>> temp = runTrainingBoard(testCases[i]);
        for (auto& i : temp) {
            result.push_back(i);
        }
    }
    return result;
}
