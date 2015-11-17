//
//  Header.h
//  HW4P4
//
//  Created by Matt on 11/15/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef HW4P4_NeuralNetwork_h
#define HW4P4_NeuralNetwork_h

#include <vector>
#include <cmath>    // For exp
#include <random>   // For rand
#include <chrono>   // For time

template <class T> class NeuralNetwork {
    private:
        // The NxN network of weights
        std::vector<std::vector<double>> weights;
    
        // Temporary Input Array
        std::vector<std::vector<double>> tempInput;
    
        // Temporary Error Array
        std::vector<std::vector<double>> tempError;
    
        double beta = 1.0f; //  what is this?
        // Learning rate
        double learningRate;
    
        // Forward phase calculation
        double propagateForward(std::vector<double>& input) {
            // Update the NxN input matrix
            updateInput(input);
            
            // For each layer
            for (size_t layer=0; layer<weights.size()-1; layer++) {
                // For each neuron in the layer compute and save it in the next input
                for (size_t neuron=0; neuron<weights[layer].size(); neuron++) {
                    // For each input dot product
                    for (size_t inputIdx=0; inputIdx<weights[layer].size(); inputIdx++) {
                        tempInput[layer+1][neuron] += tempInput[layer][inputIdx] * weights[layer][neuron];
                    }
                }
                // Compute the activation
                for (size_t neuron=0; neuron<weights[layer].size(); neuron++) {
                    tempInput[layer+1][neuron] = activation(beta, tempInput[layer+1][neuron]);
                }
            }
            
            // Compute the output
            double output = 0.0f;
            for (size_t neuron=0; neuron<weights[weights.size()-1].size(); neuron++) {
                output += tempInput[weights.size()-1][neuron] * weights[weights.size()-1][neuron];
            }
            
            // Compute the final activation
            output = activation(beta, output);
            return output;
        }
    
        // Backwards phase calculation
        void propagateBackward(std::vector<double>& input, double label, double output) {
            // Reset temp errors matrix
            updateTempError();
            
            // Compute final output error
            double outputError = (label - output) * output * (1.0f - output);
            
            // Update output layer error
            // Dot product
            for (size_t neuron=0; neuron<weights[weights.size()-1].size(); neuron++) {
                tempError[weights.size()-1][neuron] += weights[weights.size()-1][neuron] * outputError;
            }
            // Final multiply
            for (size_t neuron=0; neuron<weights[weights.size()-1].size(); neuron++) {
                tempError[weights.size()-1][neuron] = tempError[weights.size()-1][neuron] * (tempInput[weights.size()-1][neuron] * (1.0f - tempInput[weights.size()-1][neuron]));
            }
            
            
            // Compute output in hidden layers
            // For each layer
            for (long long layer=weights.size()-2; layer>0; layer--) {
                // For each neuron in the layer
                // Compute dot product of neuron weight and output error
                for (size_t neuron=0; neuron<weights[layer].size(); neuron++) {
                    tempError[layer][neuron] += weights[layer][neuron] * tempError[layer+1][neuron];
                }
                // Final multiply
                for (size_t neuron=0; neuron<weights[weights.size()-1].size(); neuron++) {
                    tempError[layer][neuron] = tempError[layer][neuron] * (tempInput[layer][neuron] * (1.0f - tempInput[layer][neuron]));
                }
            }
            
            // Update output layer weights
            for (size_t neuron=0; neuron<weights[weights.size()-1].size(); neuron++) {
                weights[weights.size()-1][neuron] = weights[weights.size()-1][neuron] - (learningRate * tempError[weights.size()-1][neuron] * tempInput[weights.size()-1][neuron]);
            }
            
            // Update hidden layer inputs
            for (long long layer=weights.size()-2; layer>0; layer--) {
                // For each neuron
                for (size_t neuron=0; neuron<weights[weights.size()-1].size(); neuron++) {
                    weights[layer][neuron] = weights[layer][neuron] - (learningRate * tempError[layer][neuron] * tempInput[layer][neuron]);
                }
            }
            0;
        }
    
        // Activation Function
        double activation(double beta, double h) {
            return 1.0f/(1.0f + exp(-beta*h));
        }
    
        void updateInput(std::vector<double>& input) {
            // Set first level of input to the specified input
            tempInput[0] = input;
            // Add bias input
            tempInput[0][tempInput[0].size()-1] = -1.0f;
            // Zero out other layers
            for (size_t i=1; i<tempInput.size(); i++) {
                for (size_t j=0; j<tempInput[i].size(); j++) {
                    tempInput[i][j] = 0.0f;
                }
            }
        }
    
        // Zero out the temporary errors array
        void updateTempError() {
            for (auto& i : tempError) {
                for (auto& j : i) {
                    j = 0.0f;
                }
            }
        }
    
    public:
        // Default Constructor
        // Input size is how large the input vector will be
        // Layers is how many hidden layers to have
        NeuralNetwork(size_t inputSize, size_t layers, double learningRate) : learningRate(learningRate) {
            // Start random number generator
            std::random_device rand_device;
            
            // Each row is a neural network path.
            // The extra row is for the -1 input bias
            weights.resize(inputSize + 1);
            
            // Each col is a neural network layer
            // There are a minimum of 2 (input and output layers)
            for (auto& i : weights) {
                i.resize(2 + layers);
            }
            
            // Set the -1 bias for each layer
            for (size_t layer=0; layer<weights.size(); layer++) {
                weights[layer][weights[layer].size()-1] = 1.0f;
            }
            
            // Set random weights for each layer
            for (size_t layer=0; layer<weights.size(); layer++) {
                for (size_t neuron=0; neuron<weights[layer].size()-1; neuron++) {
                    weights[layer][neuron] = (-1.0f + ((double)rand_device() / rand_device.max()) * (1.0f - (-1.0f)));
                }
            }
            
            // Each row is a neural network path.
            // The extra row is for the -1 input bias
            tempInput.resize(inputSize + 1);
            
            // Each col is a neural network layer
            // There are a minimum of 2 (input and output layers)
            for (auto& i : tempInput) {
                i.resize(2 + layers);
            }
            
            tempError.resize(inputSize + 1);
            
            for (auto& i : tempError) {
                i.resize(2 + layers);
            }
        }
    
        void train(T& input) {
            // Get the input's features
            std::vector<double> features = input.getFeaturesVector();
            
            // Propagate forward the input
            double output = propagateForward(features);
            
            // Propagate the output backward to compute the error and adjust weights
            propagateBackward(features, input.getLabel(), output);
        }
};

#endif
