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
        // The network of perceptrons
        std::vector<std::vector<std::vector<double>>> network;
    
        // Temporary Input Array
        std::vector<std::vector<double>> tempInput;
    
        // Temporary Error Array
        std::vector<std::vector<double>> tempError;
    
        size_t inputSize;
        size_t outputSize;
        size_t layersSize;
        size_t neuronSize;
        size_t weightSize;
    
        double beta = 1.0f; //  what is this?
        
        // Learning rate
        double learningRate;
    
        // Forward phase calculation
        std::vector<double> propagateForward(std::vector<double>& userinput) {
            // Reset temp input matrix
            updateInput(userinput);
            
            // For every layer
            for (size_t layer=0; layer<network.size(); layer++) {
                // For every neuron
                for (size_t neuron=0; neuron<network[layer].size(); neuron++) {
                    // For every input
                    for (size_t input=0; input<tempInput[layer].size(); input++) {
                        // Compute the sum of squares
                        tempInput[layer+1][neuron] += tempInput[layer][input] * network[layer][neuron][input];
                    }
                    // Compute the neuron activation and store in input+1
                    tempInput[layer+1][neuron] = activation(tempInput[layer+1][neuron]);
                }
            }
            
            // Create an output vector
            std::vector<double> output;
            for (size_t i=0; i<tempInput[network.size()].size(); i++) {
                output.push_back(tempInput[network.size()][i]);
            }
            
            return output;
        }
    
        // Backwards phase calculation
        void propagateBackward(std::vector<double>& input, std::vector<double>& labels) {
            // Clear tempError matrix
            updateTempError();
            
            // Compute output errors
            // For each neuron in the output layer
            for (size_t neuron=0; neuron<network[network.size()-1].size(); neuron++) {
                // Compute error and store in tempError
                // (output - target) * output * (1 - output);
                tempError[network.size()-1][neuron] = (tempInput[network.size()][neuron] - labels[neuron]) * tempInput[network.size()][neuron] * (1.0f - tempInput[network.size()][neuron]);
            }
            
            // Compute outputs in the hidden layers
            for (size_t layer=network.size()-2; layer != 0; layer--) {
                // For each neuron in the hidden layer
                for (size_t neuron=0; neuron<network[layer].size(); neuron++) {
                    // Compute sum weight * previous error
                    for (size_t weight=0; weight<network[layer][neuron].size(); weight++) {
                        tempError[layer][neuron] += network[layer][neuron][weight] * tempError[layer+1][neuron];
                    }
                    // Compute output*(1-output)*sum
                    tempError[layer][neuron] *= tempInput[layer+1][neuron] * (1.0f - tempInput[layer+1][neuron]);
                }
            }
            
            // Update weights
            // weight = weight - rate * error * input
            for (size_t layer=network.size()-1; layer != 0; layer--) {
                for (size_t neuron=0; neuron<network[layer].size(); neuron++) {
                    for (size_t weight=0; weight<network[layer][neuron].size(); weight++) {
                        network[layer][neuron][weight] = network[layer][neuron][weight] - learningRate * tempError[layer][neuron] * tempInput[layer][neuron];
                    }
                }
            }
        }
    
        // Neuron activation function
        double activation(double h) {
            return 1.0f/(1.0f + exp(-h));
        }
    
        // Resets the temporary input vector before the next propagation
        void updateInput(std::vector<double>& input) {
            // Set first level of input to the specified input
            tempInput[0] = input;
            // Add bias input
            tempInput[0].push_back(-1.0f);
            // Other layers have same size input vector
            // Zero out other layers
            for (size_t i=1; i<tempInput.size(); i++) {
                for (size_t j=0; j<tempInput[i].size()-1; j++) {
                    tempInput[i][j] = 0.0f;
                }
                tempInput[i][tempInput[i].size()-1] = -1.0f;    // Bias input
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
        // neuronSize is how many neurons per layer
        // layersSize is how many hidden layers to have
        // inputSize is how many inputs to have in the input layer
        // outputSize is how many outputs to have in the output layer
        NeuralNetwork(size_t newLayersSize, size_t newNeuronSize, size_t newInputSize, size_t newOutputSize, double newLearningRate)
        : layersSize(newLayersSize), neuronSize(newNeuronSize), inputSize(newInputSize), outputSize(newOutputSize), learningRate(newLearningRate) {
            // Start random number generator
            std::random_device rand_device;
            
            // Init Neural Network Matrix
            this->inputSize += 1;   // Bias input
            this->layersSize += 1;   // Input and Output Layer
            this->weightSize = neuronSize + 1;  // # of neuron outputs + bias
            
            // Resize number of network layers
            network.resize(layersSize);
            
            // Input Layer
            network[0].resize(neuronSize);
            // For each neuron
            for (size_t neuron=0; neuron<network[0].size(); neuron++) {
                // There are inputSize # of weights per neuron
                network[0][neuron].resize(inputSize);
                // Initialize weights
                for (size_t weight=0; weight<network[0][neuron].size(); weight++) {
                    network[0][neuron][weight] = (-1.0f + ((double)rand_device() / rand_device.max()) * (1.0f - (-1.0f)));
                }
            }
            
            // Hidden Layers
            for (size_t layer=1; layer<network.size()-1; layer++) {
                // There are neuronSize # of neurons in the hidden layers
                network[layer].resize(neuronSize);
                // For each neuron in the layer
                for (size_t neuron=0; neuron<network[layer].size(); neuron++) {
                    // Add weightSize # of weights
                    network[layer][neuron].resize(weightSize);
                    // Initialize weights
                    for (size_t weight=0; weight<network[layer][neuron].size(); weight++) {
                        network[layer][neuron][weight] = (-1.0f + ((double)rand_device() / rand_device.max()) * (1.0f - (-1.0f)));
                    }
                }
            }
            
            // Output Layer
            network[network.size()-1].resize(outputSize);
            // For each neuron
            for (size_t neuron=0; neuron<network[network.size()-1].size(); neuron++) {
                // There are weightSize # of inputs
                network[network.size()-1][neuron].resize(weightSize);
                // Initialize weights
                for (size_t weight=0; weight<network[network.size()-1][neuron].size(); weight++) {
                    network[network.size()-1][neuron][weight] = (-1.0f + ((double)rand_device() / rand_device.max()) * (1.0f - (-1.0f)));
                }
            }
            
            // Initialize Temporary Structures
            // tempInput is a jagged array for propagating neuron activation between layers
            tempInput.resize(layersSize + 1);   // +1 for activation record of the output layer
            
            // Input layer's input
            tempInput[0].resize(inputSize);
            tempInput[0][inputSize-1] = -1.0f;   // Bias input
            
            // Hidden and output layer's input
            for (size_t layer=1; layer<tempInput.size()-1; layer++) {
                tempInput[layer].resize(weightSize);
                // Initialize input layer to 0
                for (size_t input=0; input<tempInput[layer].size()-1; input++) {
                    tempInput[layer][input] = 0.0f;
                }
                // Initialize bias input
                tempInput[layer][tempInput[layer].size()-1] = -1.0f;
            }
            
            // Output Layer's output
            tempInput[layersSize].resize(outputSize);
            tempInput[layersSize][outputSize-1] = 0.0f;
            
            // Error Propagation Matrix
            tempError.resize(layersSize);
            for (size_t layer=0; layer<tempError.size(); layer++) {
                tempError[layer].resize(neuronSize);
                // Zero out array
                for (size_t error=0; error<tempError[layer].size(); error++) {
                    tempError[layer][error] = 0.0f;
                }
            }
        }
    
        void train(T& input) {
            // Get the input's features
            std::vector<double> features = input.getFeaturesVector();
            std::vector<double> labels = input.getLabelsVector();
            // Propagate forward the input
            propagateForward(features);
            
            // Propagate the output backward to compute the error and adjust weights
            propagateBackward(features, labels);
        }
    
        std::vector<double> identify(T& input) {
            // Get the input's features
            std::vector<double> features = input.getFeaturesVector();
            
            // Propagate forward the input
            return propagateForward(features);
        }
};

#endif
