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

template <class T> class NeuralNetwork {
    private:
        // The NxN network of weights
        std::vector<std::vector<double>> weights;
    
        // Temporary Input Array
        std::vector<std::vector<double>> tempInput;
    
        double beta = 1.0f; //  what is this?
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
            
            // Compute the output layer
            double output = 0.0f;
            for (size_t neuron=0; neuron<weights[weights.size()-1].size(); neuron++) {
                output += tempInput[weights.size()-1][neuron] * weights[weights.size()-1][neuron];
            }
            
            // Compute the activation
            output = activation(beta, output);
            return output;
        }
    
        // Backwards phase calculation
        void propagateBackward(std::vector<double>& input, double label, double output) {
            // Reset temp input matrix
            updateInput(input);
            
            // Compute output error
            double outputError = (label - output) * output * (1 - output);
            
            // Update hidden layers
            // For each hidden layer
            for (long long layer=weights.size()-2; layer>0; layer--) {
                // For each neuron
                for (size_t neuron=0; neuron<weights[layer].size(); neuron++) {
                    
                }
            }
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
    
    public:
        // Default Constructor
        // Input size is how large the input vector will be
        // Layers is how many hidden layers to have
        NeuralNetwork(size_t inputSize, size_t layers, double learningRate) : learningRate(learningRate) {
            // Each row is a neural network path.
            // The extra row is for the -1 input bias
            weights.resize(inputSize + 1);
            
            // Each col is a neural network layer
            // There are a minimum of 2 (input and output layers)
            for (auto& i : weights) {
                i.resize(2 + layers);
            }
            
            // Set the -1 bias for each layer
            //for (size_t layer=0; layer<weights.size(); layer++) {
                //weights[layer][weights[layer].size()-1] = 1.0f;
            //}
            
            // Each row is a neural network path.
            // The extra row is for the -1 input bias
            tempInput.resize(inputSize + 1);
            
            // Each col is a neural network layer
            // There are a minimum of 2 (input and output layers)
            for (auto& i : tempInput) {
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
