//
//  UniformCostSearch.h
//  HW2P2
//
//  Created by Matt on 9/21/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

/* UCS Interface - Required T functions:
    // For copying T
    T(const T&)
    // Returns the cost of T
    size_t getCost() const;
    // Sets the cost of T
    void setCost(size_t);
    // Returns some arbitrary state type for T
    X getState();
    // Sets the same arbitrary state type for T
    void setState(X);
    // Returns a shared_ptr to the previous node
    std::shared_ptr<T> getPrevious();
    // Sets a shared_ptr to the previous node
    void setPrevious(std::shared_ptr<T>&);
    // Returns a vector of shared_ptr of neighbors of T
    std::vector<std::shared_ptr<T>> neighbors();
    // Compares State equality for T with another T
    bool isStateEqual(std::shared_ptr<T>&);
    // Tests if T is a goal node, optionally with input T Goal
    bool isGoal(T&);
    // Computes a hash for T's state only (no cost)
    size_t hashState();
    
    A* Additions
    // setHeuristic for node
    void setHeuristic(size_t);
    // getHeuristic cost for node
    size_t getHeuristic() const;
*/

#ifndef UniformCostSearch_h
#define UniformCostSearch_h
#include <vector>           // std::vector, size_t
#include <algorithm>        // make_heap, push_heap, pop_heap, find
#include <unordered_set>    // hash table
#include <memory>           // std::shared_ptr
#include <limits>           // std::numeric_limits<size_t>
#include <stack>            // std::queue

// unordered_set hash override call to
// T's hashState()
template<class S>
class THash {
    public:
        std::size_t operator()(std::shared_ptr<S> s) const {
            return s->hashState();
        }
};

// unordered_set equality override call to
// T's isStateEqual()
template<class R>
class TEqual {
    public:
        bool operator()(std::shared_ptr<R> a, std::shared_ptr<R> b) const {
            return a->isStateEqual(b);
        }
};

// UniformCostSearch Template Class
template <class T>
class UniformCostSearch {
    private:
        // std::heap cost comparison override call to
        // T's getCost()
        static bool costComparator(const std::shared_ptr<T> a, const std::shared_ptr<T> b) {
            return a->getCost() > b->getCost();
        }
    
        static bool heuristicComparator(const std::shared_ptr<T> a, const std::shared_ptr<T> b) {
            return (a->getCost() + a->getHeuristic()) > (b->getCost() + b->getHeuristic());
        }
    
        // Count of explored nodes for most recently ran search
        size_t exploredCount;
    
    public:
        // Default Constructor
        UniformCostSearch() : exploredCount(0) {
        }
    
        // Getter for nodes explored
        size_t getLastExploredCount() {
            return exploredCount;
        }
    
        // Standard Uniform Cost Search
        std::vector<T> UniformSearch(T start, T goal) {
            // Eventual solution
            std::vector<T> solution;
            
            // Copy start node
            std::shared_ptr<T> node(new T(start));
            node->setCost(0);
            
            // Initialize Data Structures
            std::vector<std::shared_ptr<T>> frontier;
            std::make_heap(frontier.begin(), frontier.end(), costComparator);
            std::unordered_set<std::shared_ptr<T>, THash<T>, TEqual<T>> explored;
            exploredCount = 0;
            
            // Enqueue start
            frontier.push_back(node);
            std::push_heap(frontier.begin(), frontier.end(), costComparator);
            
            for (;;) {
                if (frontier.empty()) {
                    throw std::runtime_error("No solution was found.");
                }
                
                // Dequeue top node from frontier
                node = frontier.front();
                std::pop_heap(frontier.begin(), frontier.end(), costComparator);
                frontier.pop_back();
                
                // Place in explored
                explored.emplace(node);
                
                // Is this the goal state?
                if (node->isGoal(goal)) {
                    break;
                }
                
                // Find neighbors
                std::vector<std::shared_ptr<T>> candidates = node->neighbors();
                for (auto& n : candidates) {
                    // Update previous pointer
                    n->setPrevious(node);
                    // Check if already explored
                    if (explored.find(n) == explored.end()) {
                        // Check if already in frontier
                        auto it = std::find(frontier.begin(), frontier.end(), n);
                        if (it == frontier.end()) {
                            // Add to frontier
                            n->setCost(n->getCost() + node->getCost());
                            n->setPrevious(node);
                            frontier.push_back(n);
                            std::push_heap(frontier.begin(), frontier.end(), costComparator);
                        }
                    }
                }
            }
            
            // node is the solution
            // Find path
            while (node->getPrevious() != nullptr) {
                T temp = (*node);
                solution.insert(solution.begin(), temp);
                node = node->getPrevious();
            }
            
            exploredCount = explored.size();
            return solution;
        }
    
        // Uniform Cost Search with Branch and Bound
        std::vector<T> UniformSearchBnB(T start, T goal) {
            // Eventual solution
            std::vector<T> solution;
            std::shared_ptr<T> best;
            
            // Copy start node
            std::shared_ptr<T> node(new T(start));
            node->setCost(0);
            
            // Initialize Data Structures
            std::vector<std::shared_ptr<T>> frontier;
            std::make_heap(frontier.begin(), frontier.end(), costComparator);
            std::unordered_set<std::shared_ptr<T>, THash<T>, TEqual<T>> explored;
            exploredCount = 0;
            size_t upperBound = std::numeric_limits<size_t>::max();
            
            // Enqueue start
            frontier.push_back(node);
            std::push_heap(frontier.begin(), frontier.end(), costComparator);
            
            for (;;) {
                if (frontier.empty()) {
                    throw std::runtime_error("No solution was found.");
                }
                
                // Dequeue top node from frontier
                node = frontier.front();
                std::pop_heap(frontier.begin(), frontier.end(), costComparator);
                frontier.pop_back();
                
                // Place in explored
                explored.emplace(node);
                
                // Is this the goal state?
                if (node->isGoal(goal)) {
                    best = node;
                    upperBound = node->getCost();
                    break;
                }
                
                // Is this above the upper bound?
                if (node->getCost() >= upperBound) {
                    continue;
                }

                // Find neighbors
                std::vector<std::shared_ptr<T>> candidates = node->neighbors();
                for (auto& n : candidates) {
                    // Skip if >= upperBound
                    if ((n->getCost() + node->getCost()) >= upperBound) {
                        continue;
                    }
                    // If its the goal and lower, update best
                    if (n->isGoal(goal)) {
                        best = n;
                        upperBound = n->getCost() + node->getCost();
                    }
                    // Update previous pointer
                    n->setPrevious(node);
                    // Check if already explored
                    if (explored.find(n) == explored.end()) {
                        // Check if already in frontier
                        auto it = std::find(frontier.begin(), frontier.end(), n);
                        if (it == frontier.end()) {
                            // Add to frontier
                            n->setCost(n->getCost() + node->getCost());
                            n->setPrevious(node);
                            frontier.push_back(n);
                            std::push_heap(frontier.begin(), frontier.end(), costComparator);
                        }
                    }
                }
            }
            
            // best is the solution
            // Find path
            while (best->getPrevious() != nullptr) {
                T temp = (*best);
                solution.insert(solution.begin(), temp);
                best = best->getPrevious();
            }
            
            exploredCount = explored.size();
            return solution;
        }
    
        // A* Heuristic Search
        std::vector<T> AStarSearch(T start, T goal) {
            // Eventual solution
            std::vector<T> solution;
            std::shared_ptr<T> best;
            
            // Copy start node
            std::shared_ptr<T> node(new T(start));
            node->setCost(0);
            
            // Initialize Data Structures
            std::vector<std::shared_ptr<T>> frontier;
            std::make_heap(frontier.begin(), frontier.end(), heuristicComparator);
            std::unordered_set<std::shared_ptr<T>, THash<T>, TEqual<T>> explored;
            exploredCount = 0;
            size_t upperBound = std::numeric_limits<size_t>::max();
            
            // Enqueue start
            frontier.push_back(node);
            std::push_heap(frontier.begin(), frontier.end(), heuristicComparator);
            
            for (;;) {
                if (frontier.empty()) {
                    throw std::runtime_error("No solution was found.");
                }
                
                // Dequeue top node from frontier
                node = frontier.front();
                std::pop_heap(frontier.begin(), frontier.end(), heuristicComparator);
                frontier.pop_back();
                
                // Place in explored
                explored.emplace(node);
                
                // Is this the goal state?
                if (node->isGoal(goal)) {
                    best = node;
                    upperBound = node->getCost() + node->getHeuristic();
                    break;
                }
                
                // Is this above the upper bound?
                if ((node->getCost() + node->getHeuristic()) >= upperBound) {
                    continue;
                }

                // Find neighbors
                std::vector<std::shared_ptr<T>> candidates = node->neighbors();
                for (auto& n : candidates) {
                    // Skip if >= upperBound
                    if ((n->getCost() + n->getHeuristic() + node->getCost() + node->getHeuristic()) >= upperBound) {
                        continue;
                    }
                    // If its the goal and lower, update best
                    if (n->isGoal(goal)) {
                        best = n;
                        upperBound = n->getCost() + n->getHeuristic() + node->getCost() + node->getHeuristic();
                    }
                    // Update previous pointer
                    n->setPrevious(node);
                    // Check if already explored
                    if (explored.find(n) == explored.end()) {
                        // Check if already in frontier
                        auto it = std::find(frontier.begin(), frontier.end(), n);
                        if (it == frontier.end()) {
                            // Add to frontier
                            n->setCost(n->getCost() + node->getCost());
                            n->setPrevious(node);
                            frontier.push_back(n);
                            std::push_heap(frontier.begin(), frontier.end(), heuristicComparator);
                        }
                    }
                }
            }
            
            // best is the solution
            // Find path
            while (best->getPrevious() != nullptr) {
                T temp = (*best);
                solution.insert(solution.begin(), temp);
                best = best->getPrevious();
            }
            
            exploredCount = explored.size();
            return solution;
        }
};

#endif