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
            exploredCount = 0;
            std::shared_ptr<T> start(new T(u));
            return minmaxab(start, std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max(), 0);
        }
};

#endif
