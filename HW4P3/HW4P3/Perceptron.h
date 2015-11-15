//
//  Perceptron.h
//  HW4P3
//
//  Created by Matt on 11/14/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef __HW4P3__Perceptron__
#define __HW4P3__Perceptron__

#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>   // Debugging

template <class T> class Perceptron {
    private:
        std::vector<double> weights;
        double errorEstimate;
        double bias;
        double learningRate;
    
        std::vector<double> bestWeights;
        double bestErrorEstimate;
    
        // Convenience Wrapper for double -> bool
        inline bool binaryClassifier(double y) {
            return y > 0.0f;
        }
    
    public:
        Perceptron(size_t weightsSize, double biasAmount, double learningRate) : errorEstimate(0.0f), bias(biasAmount), learningRate(learningRate) {
            this->weights.resize(weightsSize);
            this->bestWeights.resize(weightsSize);
            for (size_t i=0; i<weights.size(); i++) {
                this->weights[i] = 0.0f;
                this->bestWeights[i] = 0.0f;
            }
            
        }
    
        // Run the perceptron on a piece of data
        // with weight adjustment
        void runTrainingData(size_t iterations, std::vector<T> dataset) {
            // Shuffle training set
            unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle(dataset.begin(), dataset.end(), std::default_random_engine(seed));
            
            size_t dataidx = 0;
            
            // Run Iterations
            for (size_t itr = 0; itr<iterations; itr++) {
                // Shuffle training set
                if (dataidx >= dataset.size()) {
                    seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
                    std::shuffle(dataset.begin(), dataset.end(), std::default_random_engine(seed));
                    dataidx = 0;
                }
                
                T data = dataset[dataidx];
                
                if (data.getFeaturesVector().size() != weights.size()) {
                    throw std::runtime_error("Features vector size does not match perceptron weights size.");
                }
                
                // Compute dot product of weights and features
                double y = 0.0f;
                for (size_t i=0; i<weights.size(); i++) {
                    y += weights[i]*data.getFeaturesVector()[i] + bias;
                }
                
                bool output = binaryClassifier(y);
                
                // Update weights
                if (output != data.getBinaryLabel()) {
                    for (size_t i=0; i<weights.size(); i++) {
                        double temp = weights[i] + learningRate * (data.getBinaryLabel() - y)*data.getFeaturesVector()[i];
                        weights[i] = weights[i] + learningRate * (data.getBinaryLabel() - y)*data.getFeaturesVector()[i];
                    }
                }
                
                // Update Error Estimate
                
                dataidx++;
            }
        }
    
        // Run the perceptron on a piece of data
        // without weight adjustment or labels
        bool runTestData(T& data) {
            if (data.getFeaturesVector().size() != weights.size()) {
                throw std::runtime_error("Features vector size does not match perceptron weights size.");
            }
            
            // Compute dot product of weights and features
            double y = 0.0f;
            for (size_t i=0; i<weights.size(); i++) {
                y += weights[i]*data.getFeaturesVector()[i] + bias;
            }
            
            return binaryClassifier(y);
        }
    
        std::vector<double> getWeights() {
            return this->weights;
        }
    
        double getErrorEstimate() {
            return this->errorEstimate;
        }
};

#endif /* defined(__HW4P3__Perceptron__) */
