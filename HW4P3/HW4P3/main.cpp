//
//  main.cpp
//  HW4P3
//
//  Created by Matt on 11/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>
#include "MNISTLoader.h"
#include "RaviMNISTLoader.h"
#include "Perceptron.h"

// Highly advanced ASCII Drawing routine. Good enough to be a VLC output plugin
void asciiDrawer(LabeledData& image);

// Function to generate a set of LabeledData containing only 1 and 5
std::vector<LabeledData> generateOnesFivesSet(size_t limit, std::vector<LabeledData>& labeledSet);

// Calculate the standardized features across the entire set
void calcStandardizedFeatures(std::vector<LabeledData>& labeledSet);

int main(int argc, const char * argv[]) {
    MNISTLoader loader;
    if (argc != 5) {
        std::cerr << "Usage: ./a.out trainingLabelFile trainingImageFile testLabelFile testImageFile" << std::endl;
        std::cerr << "E.x.: ./a.out train-labels.idx1-ubyte train-images.idx3-ubyte t10k-labels.idx1-ubyte t10k-images.idx3-ubyte" << std::endl;
        exit(-1);
    }

    // Read the training and test sets in
    std::string trainingLabelPath = argv[1];
    std::string trainingImagePath = argv[2];
    std::string testLabelPath = argv[3];
    std::string testImagePath = argv[4];
    std::cout << "Loading the MNIST Dataset... This could take a minute..." << std::endl;
    std::vector<LabeledData> trainingset = loader.readDataSet(trainingLabelPath, trainingImagePath);
    std::vector<LabeledData> testset = loader.readDataSet(testLabelPath, testImagePath);
    
    // Shuffle full training set
    unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(trainingset.begin(), trainingset.end(), std::default_random_engine(seed));
    
    // Shuffle full test set
    seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(testset.begin(), testset.end(), std::default_random_engine(seed));
    
    // Generate a subset of each containing only ones and fives
    std::vector<LabeledData> trainingOnesFivesSet = generateOnesFivesSet(500, trainingset);
    std::vector<LabeledData> testOnesFivesSet = generateOnesFivesSet(500, testset);
    
    // Erase the training and test sets to save memory
    trainingset.clear();
    testset.clear();
    
    // Calculate standardized features
    calcStandardizedFeatures(trainingOnesFivesSet);
    calcStandardizedFeatures(testOnesFivesSet);
    
    // Create Single Perceptron
    Perceptron<LabeledData> mnistPerceptron(trainingOnesFivesSet[0].getFeaturesVector().size(), 0.0f, 0.5f);
    
    // Run Training Data
    mnistPerceptron.runTrainingData(1000, trainingOnesFivesSet);
    
    // Compare Results
    for (auto& i : trainingOnesFivesSet) {
        //asciiDrawer(i);
        std::cout << "Binary Label:      " << i.getBinaryLabel() << std::endl;
        std::cout << "Perceptron Output: " << mnistPerceptron.runTestData(i) << std::endl;
        std::cout << "===================================" << std::endl;
    }
    
    return 0;
}

// Industry standard proprietary image viewing algorithm patent pending by Matt Smith
void asciiDrawer(LabeledData& image) {
    auto features = image.getFeaturesVector();
    std::cout << "Label = " << (image.getLabel() == 10 ? 0 : image.getLabel()) << std::endl;
    std::cout << "Density = " << features[0] << std::endl;
    std::cout << "Symmetry = " << features[1] << std::endl;
    std::cout << "Min Intersections = " << features[2] << std::endl;
    std::cout << "Max Intersections = " << features[3] << std::endl;
    
    for (size_t i=0; i<image.getRows(); i++) {
        for (size_t j=0; j<image.getCols(); j++) {
            if (image.getData()[i][j] >= 127) {
                std::cout << "#";
            }
            else if ((image.getData()[i][j] < 127) && (image.getData()[i][j] > 0)) {
                std::cout << ".";
            }
            else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

// Generate a set containing only ones and fives with normalized labels
std::vector<LabeledData> generateOnesFivesSet(size_t limit, std::vector<LabeledData>& labeledSet) {
    std::vector<LabeledData> OnesFivesSet;
    OnesFivesSet.reserve(limit*2);
    size_t ones = 0;
    size_t fives = 0;
    for (size_t i=0; i<labeledSet.size(); i++) {
        if ((ones < limit) && (labeledSet[i].getLabel() == 1)) {
            LabeledData temp = labeledSet[i];
            temp.setBinaryLabel(0);   // 1 = 0b0, 5 = 0b1
            OnesFivesSet.push_back(temp);
            ones++;
        }
        if ((fives < limit) && (labeledSet[i].getLabel() == 5)) {
            LabeledData temp = labeledSet[i];
            temp.setBinaryLabel(1);   // 1 = 0b0, 5 = 0b1
            OnesFivesSet.push_back(temp);
            fives++;
        }
        if ((ones >= limit) && (fives >= limit)) {
            break;
        }
    }
    return OnesFivesSet;
}

// Calculate the standardized features across the entire set
// This is a very expensive operation on a large set
void calcStandardizedFeatures(std::vector<LabeledData>& labeledSet) {
    double densityMean = 0.0f;
    double symmetryMean = 0.0f;
    double minIntMean = 0.0f;
    double maxIntMean = 0.0f;
    
    for (auto& i : labeledSet) {
        auto features = i.getFeaturesVector();
        densityMean += features[0];
        symmetryMean += features[1];
        minIntMean += features[2];
        maxIntMean += features[3];
    }
    
    densityMean /= labeledSet.size();
    symmetryMean /= labeledSet.size();
    minIntMean /= labeledSet.size();
    maxIntMean /= labeledSet.size();
    
    // Calculate Standard Deviation
    double densitySD = 0.0f;
    double symmetrySD = 0.0f;
    double minIntSD = 0.0f;
    double maxIntSD = 0.0f;
    
    // Sum
    for (auto& i : labeledSet) {
        auto features = i.getFeaturesVector();
        densitySD += pow(densityMean - features[0], 2);
        symmetrySD += pow(symmetryMean - features[1], 2);
        minIntSD += pow(minIntMean - features[2], 2);
        maxIntSD += pow(maxIntMean - features[3], 2);
    }
    
    // Variance
    densitySD = ((double)(1.0f/labeledSet.size())) * densitySD;
    symmetrySD = ((double)(1.0f/labeledSet.size())) * symmetrySD;
    minIntSD = ((double)(1.0f/labeledSet.size())) * minIntSD;
    maxIntSD = ((double)(1.0f/labeledSet.size())) * maxIntSD;
    
    // Standard Deviation
    densitySD = sqrt(densitySD);
    symmetrySD = sqrt(symmetrySD);
    minIntSD = sqrt(minIntSD);
    maxIntSD = sqrt(maxIntSD);
    
    // Writeback features
    for (auto& i : labeledSet) {
        auto features = i.getFeaturesVector();
        features[0] = (features[0] - densityMean) / densitySD;
        features[1] = (features[1] - symmetryMean) / symmetrySD;
        features[2] = (features[2] - minIntMean) / minIntSD;
        features[3] = (features[3] - maxIntMean) / maxIntSD;
        i.setFeaturesVector(features);
    }
}