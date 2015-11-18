//
//  main.cpp
//  Midterm1
//
//  Created by Matt on 10/24/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "MinMaxSearch.h"
#include "TicTacToe.h"
#include "NeuralNetwork.h"

void runSampleCases();
void runMenu();
void runFileMenu();
TicTacToe::GameBoard readFile();
TicTacToe::XO raviToSmithConverter(long long val);
void runTicTacToe(TicTacToe::GameBoard& board);
void runTicTacToeAB(TicTacToe::GameBoard& board);
std::vector<std::vector<size_t>> generatePermutations(TicTacToe::GameBoard& board);


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
    std::vector<std::vector<std::vector<double>>> temp;
    temp.resize(4);
    temp[0].resize(4);
    temp[0][0].resize(4);
    temp[0][0][0] = 4.0f;
    
    
    //runMenu();
    // size_t newLayersSize, size_t newNeuronSize, size_t newInputSize, size_t newOutputSize, double newLearningRate
    NeuralNetwork<DummyData> testnn(1,2,2,1,0.25);
    
    std::vector<double> features = {0.0f, 0.0f};
    std::vector<double> labels = {0.0f};
    DummyData test(labels, features);
    
    std::vector<double> features2 = {0.0f, 1.0f};
    std::vector<double> labels2 = {1.0f};
    DummyData test2(labels2, features2);
    
    std::vector<double> features3 = {1.0f, 0.0f};
    std::vector<double> labels3 = {1.0f};
    DummyData test3(labels3, features3);
    
    std::vector<double> features4 = {1.0f, 1.0f};
    std::vector<double> labels4 = {1.0f};
    DummyData test4(labels4, features4);
    
    std::vector<DummyData> testData;
    testData.push_back(test);
    testData.push_back(test2);
    testData.push_back(test3);
    testData.push_back(test4);
    
    for (size_t i=0; i<100000; i++) {
        // Shuffle
        std::random_shuffle(testData.begin(), testData.end());
        for (auto& i : testData) {
            testnn.train(i);
        }
    }
    
    std::cout << "Test output2: "; printVector(testnn.identify(test));
    std::cout << "Test output2: "; printVector(testnn.identify(test2));
    return 0;
}



// Root Level Menu
void runMenu() {
    do {
        std::cout << "Enter a choice:" << std::endl;
        std::cout << "[1] Run Test Cases, [2] Load and run file, [q] Quit" << std::endl;
        std::cout << "> ";
        char input = 0;
        if (std::cin >> input) {
            switch (input) {
                case '1': {
                    runSampleCases();
                    break;
                }
                case '2': {
                    try {
                        runFileMenu();
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
void runFileMenu() {
    TicTacToe::GameBoard loadedboard = readFile();
    std::cout << std::endl << "File Loaded." << std::endl;
    
    do {
        std::cout << "[1] Run Minmax, [2] Run Minmax AlphaBeta, [q] Return" << std::endl;
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
void runSampleCases() {
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
    
    for (size_t i=0; i<testCases.size(); i++) {
        std::cout << "=== Test Case " << i << " ===" << std::endl;
        runTicTacToe(testCases[i]);
        runTicTacToeAB(testCases[i]);
    }
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