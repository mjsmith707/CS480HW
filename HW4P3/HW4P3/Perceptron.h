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
#include <algorithm>    // for std::shuffle
#include <random>       // for std::default_random_engine
#include <chrono>       // for chrono::system_clock
#include <cmath>        // for abs

/* Binary Perceptron Interface:

// Returns a 0 or 1 label
bool T.getBinaryLabel();

// Returns a features vector for the data
std::vector<double> T.getFeaturesVector();

*/
template <class T> class Perceptron {
    private:
        // Vector containing the best weights in the perceptron
        std::vector<double> weights;
    
        // The best error estimate found
        double errorEstimate;
    
        // The perceptron bias amount
        double bias;
    
        // The perceptron learning rate
        double learningRate;
    
        // Convenience Wrapper for double -> bool
        inline bool binaryClassifier(double y) {
            return y > 0.0f;
        }
    
    public:
        // Parameterized Constructor
        Perceptron(size_t weightsSize, double biasAmount, double learningRate) : errorEstimate(1.0f), bias(biasAmount), learningRate(learningRate) {
            // Initialize weights to 0
            this->weights.resize(weightsSize);
            for (size_t i=0; i<weights.size(); i++) {
                this->weights[i] = 0.0f;
            }
        }
    
        // Run the perceptron on a piece of data with weight adjustment
        void runTrainingData(size_t iterations, std::vector<T> dataset) {
            // Vector for holding ouput for error estimation
            std::vector<bool> savedOutput;
            savedOutput.resize(dataset.size());
            
            // Shuffle training set
            unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle(dataset.begin(), dataset.end(), std::default_random_engine(seed));
            
            // Run Iterations
            for (size_t itr = 0; itr<iterations; itr++) {
                // Temporaries for pocket training
                std::vector<double> tempweights = weights;
                
                // Shuffle training set
                seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
                std::shuffle(dataset.begin(), dataset.end(), std::default_random_engine(seed));
                for (size_t dataidx=0; dataidx<dataset.size(); dataidx++) {
                    T data = dataset[dataidx];
                    
                    if (data.getFeaturesVector().size() != weights.size()) {
                        throw std::runtime_error("Features vector size does not match perceptron weights size.");
                    }
                    
                    // Compute dot product of weights and features
                    double y = 0.0f;
                    for (size_t i=0; i<tempweights.size(); i++) {
                        y += tempweights[i]*data.getFeaturesVector()[i] + bias;
                    }
                    
                    bool output = binaryClassifier(y);
                    savedOutput[dataidx] = output;
                    
                    // Update weights
                    if (output != data.getBinaryLabel()) {
                        for (size_t i=0; i<tempweights.size(); i++) {
                            tempweights[i] = tempweights[i] + learningRate * (data.getBinaryLabel() - y)*data.getFeaturesVector()[i];
                        }
                    }
                }
                
                // Compute Error Estimate
                double tempError = 0.0f;
                for (size_t i=0; i<dataset.size(); i++) {
                    tempError += abs(dataset[i].getBinaryLabel() - savedOutput[i]);
                }
                tempError *= 1.0f/(double)dataset.size();
                
                // Update error and weights if results were better (Pocket Algorithm)
                if (tempError < errorEstimate) {
                    weights = tempweights;
                    errorEstimate = tempError;
                }
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
    
        // Returns the best weight vector for the perceptron
        std::vector<double> getWeights() {
            return this->weights;
        }
    
        // Returns the best error estimate for the perceptron
        double getErrorEstimate() {
            return this->errorEstimate;
        }
};

#endif /* defined(__HW4P3__Perceptron__) */
