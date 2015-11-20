//
//  MinMaxSearch.h
//  Midterm1
//
//  Created by Matt on 10/24/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef Midterm1_MinMaxSearch_h
#define Midterm1_MinMaxSearch_h

#include <algorithm>    // For std::min, std::max
#include <limits>       // For std::numeric_limits
#include <vector>       // For std::vector
#include <memory>       // For std::shared_ptr
#include "neuralNetwork.h"  // For neuralNetwork

/* MinMax Algorithm Interface - Required T Functions:
    // For copying T
    T(const T&);
    // Returns a static evaluation of T for MinMax
    long long Eval();
    // Returns whether T is a leaf node
    bool isLeaf();
    // Returns whether T is a Max node
    bool isMaxNode();
    // Returns a vector of shared_ptr of neighbors of T
    std::vector<std::shared_ptr<T>> neighbors();
    
    Alpha-Beta Additions
    // Returns a static evaluation of T for MinMax Alpha-Beta
    long long EvalAB();
    
    Neural Network Training Additions zzz
    void setLabel(long long);
*/

// MinMaxSearch Template Class
template <class T>
class MinMaxSearch {
    private:
        // Count of explored nodes since last run
        size_t exploredCount;
    
        // The suggested node to move to for Minmax Alpha-Beta
        std::shared_ptr<T> moveNode;
    
        // The max depth that Minmax Alpha-Beta will run to
        size_t abMaxDepth;
    
        // Private recursive Minmax Algorithm
        long long minmax(std::shared_ptr<T> u) {
            exploredCount++;
            // No successor, static evaluation
            if (u->isLeaf()) {
                return u->Eval();
            }
            
            // Find out if this is a max or min node
            bool maxNode = u->isMaxNode();
            
            // Initialize return value for MAX node
            long long val = maxNode ? std::numeric_limits<long long>::min() : std::numeric_limits<long long>::max();
            
            // Traverse successor list
            std::vector<std::shared_ptr<T>> candidates = u->neighbors();
            for (auto& v : candidates) {
                val = maxNode ? std::max(val, minmax(v)) : std::min(val, minmax(v));
            }
            
            return val;
        }
    
        // Private recursive Minmax Algorithm with Alpha-Beta Pruning
        long long minmaxab(std::shared_ptr<T> u, long long alpha, long long beta, long long depth) {
            exploredCount++;
            
            // No successor, static evaluation
            if (u->isLeaf() || (depth == abMaxDepth)) {
                return u->EvalAB();
            }
            
            // Find out if this is a max or min node
            bool maxNode = u->isMaxNode();
            long long res = 0;
            
            if (maxNode) {
                res = alpha;
                // Traverse Successor List
                std::vector<std::shared_ptr<T>> candidates = u->neighbors();
                for (auto& v : candidates) {
                    long long val = minmaxab(v, res, beta, depth+1);
                    // Set v's nn label and save
                    v->setLabel(val);
                    labeledData.push_back(v);
                    
                    res = std::max(res, val);
                    
                    if (res >= beta) {
                        moveNode = u;
                        return res;
                    }
                }
            }
            else {
                res = beta;
                // Traverse Successor List
                std::vector<std::shared_ptr<T>> candidates = u->neighbors();
                for (auto& v : candidates) {
                    long long val = minmaxab(v, alpha, res, depth+1);
                    // Set v's nn label and save
                    v->setLabel(val);
                    labeledData.push_back(v);
                    
                    res = std::min(res, val);
                    
                    if (res <= alpha) {
                        moveNode = u;
                        return res;
                    }
                }
            }
            
            return res;
        }
    
        // Sort comparator using labels
        static bool neuralSort(std::shared_ptr<T>& a, std::shared_ptr<T>& b) {
            std::vector<double> alabel = a->getLabel();
            std::vector<double> blabel = b->getLabel();
            return binaryToInt(alabel) > binaryToInt(blabel);
        }
    
        // Convert double binary vector to int
        static int binaryToInt(std::vector<double>& labels) {
            if ((labels[0] == 0.0f) && (labels[1] == 1.0f)) {
                return 1;
            }
            else if ((labels[0] == 1.0f) && (labels[1] == 1.0f)) {
                return -1;
            }
            else {
                return 0;
            }
        }
    
        // Function to label candidate nodes via the neural network
        void labelCandidates(neuralNetwork& nn, std::vector<std::shared_ptr<T>>& candidates) {
            for (auto& i : candidates) {
                auto board = i->getGameBoard();
                size_t inputSize = board.size() * board[0].size();
                double* input = new double[inputSize];
                size_t count = 0;
                for (auto& x : board) {
                    for (auto& y : x) {
                        input[count] = y;
                        count++;
                    }
                }
                double* result = nn.feedInput(input);
                std::vector<double> newlabel;
                for (size_t i=0; i<2; i++) {
                    newlabel.push_back(nn.getRoundedOutputValue(result[i]));
                }
                
                i->setLabel(newlabel);
                delete[] input;
            }
        }

        // Private recursive Minmax Algorithm with Alpha-Beta Pruning and Neural Network ranking
        long long minmaxabnn(neuralNetwork& nn, std::shared_ptr<T> u, long long alpha, long long beta, long long depth) {
            exploredCount++;
            
            // No successor, static evaluation
            if (u->isLeaf() || (depth == abMaxDepth)) {
                return u->EvalAB();
            }
            
            // Find out if this is a max or min node
            bool maxNode = u->isMaxNode();
            long long res = 0;
            
            if (maxNode) {
                res = alpha;
                // Traverse Successor List
                std::vector<std::shared_ptr<T>> candidates = u->neighbors();
                // Use neural network to label and sort the list
                labelCandidates(nn, candidates);
                // Sort
                std::sort(candidates.begin(), candidates.end(), neuralSort);
                
                for (auto& v : candidates) {
                    long long val = minmaxab(v, res, beta, depth+1);
                    
                    res = std::max(res, val);
                    
                    if (res >= beta) {
                        moveNode = u;
                        return res;
                    }
                }
            }
            else {
                res = beta;
                // Traverse Successor List
                std::vector<std::shared_ptr<T>> candidates = u->neighbors();
                // Use neural network to label and sort the list
                labelCandidates(nn, candidates);
                // Sort
                std::sort(candidates.begin(), candidates.end(), neuralSort);
                for (auto& v : candidates) {
                    long long val = minmaxab(v, alpha, res, depth+1);
                    
                    res = std::min(res, val);
                    
                    if (res <= alpha) {
                        moveNode = u;
                        return res;
                    }
                }
            }
            
            return res;
        }
    
    public:
        // Vector of labeled T objects for neural network training
        std::vector<std::shared_ptr<T>> labeledData;
    
        // Parameterized Constructor
        // Sets the Alpha-Beta Max Depth
        MinMaxSearch(size_t depth) : exploredCount(1), abMaxDepth(depth) {
        }
    
        // Return how many nodes were explored since last run
        size_t getLastExploredCount() {
            return this->exploredCount;
        }
    
        // Return the recommended move from the last Minmax Alpha-Beta run
        std::shared_ptr<T> getMoveNode() {
            return this->moveNode;
        }
    
        // Run the MinMax Algorithm for T
        long long runMinMaxSearch(T u) {
            exploredCount = 0;
            std::shared_ptr<T> start(new T(u));
            return minmax(start);
        }
    
        // Run the MinMax Algorithm with Alpha-Beta Pruning for T
        long long runMinMaxABSearch(T u) {
            labeledData.clear();
            exploredCount = 0;
            std::shared_ptr<T> start(new T(u));
            return minmaxab(start, std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max(), 0);
        }
    
        // Run the MinMax Algorithm with Alpha-Beta Pruning and Neural Network ranking for T
        long long runMinMaxABSearchNN(neuralNetwork& nn, T u) {
            labeledData.clear();
            exploredCount = 0;
            std::shared_ptr<T> start(new T(u));
            return minmaxabnn(nn, start, std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max(), 0);
        }
};

#endif



