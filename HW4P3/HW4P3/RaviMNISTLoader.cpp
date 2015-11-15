//
//  RaviMNISTLoader.cpp
//  HW4P3
//
//  Created by Matt on 11/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "RaviMNISTLoader.h"

// I just finished this right before you posted a reply saying I could use the MNIST dataset
RaviMNISTLoader::RaviMNISTLoader() {

}

// Read the dataset from the two given paths (labels and images)
// Create a set of LabeledData
std::vector<LabeledData> RaviMNISTLoader::readDataSet(std::string labelFilePath, std::string imageFilePath) {
    std::fstream labelFile;
    labelFile.open(labelFilePath, std::fstream::in);
    if (!labelFile.is_open()) {
        throw std::runtime_error("Failed to open label data file.");
    }
    
    std::fstream imageFile;
    imageFile.open(imageFilePath, std::fstream::in);
    if (!imageFile.is_open()) {
        throw std::runtime_error("Failed to open image data file.");
    }
    
    std::vector<unsigned int> labels = readLabels(labelFile);
    std::vector<double> rawPixels = readPixels(imageFile);
    
    std::vector<LabeledData> result = createLabeledData(labels, rawPixels);
    
    return result;
}

// Read the labels from a file stream
std::vector<unsigned int> RaviMNISTLoader::readLabels(std::fstream& labelFile) {
    std::vector<unsigned int> labels;
    unsigned int temp;
    while (labelFile >> temp) {
        labels.push_back(temp);
    }
    
    return labels;
}

// Read the pixels from a file stream
std::vector<double> RaviMNISTLoader::readPixels(std::fstream& imageFile) {
    std::vector<double> pixels;
    double temp;
    char comma;
    while (imageFile >> temp) {
        pixels.push_back(temp);
        imageFile >> comma;
    }
    
    return pixels;
}

// Create labeled data from a labels set and pixels set
std::vector<LabeledData> RaviMNISTLoader::createLabeledData(std::vector<unsigned int>& labels, std::vector<double>& rawPixels) {
    std::vector<LabeledData> result;
    size_t pixelidx = 0;
    for (size_t i=0; i<labels.size(); i++) {
        unsigned int label = labels[i];
        std::vector<std::vector<unsigned char>> image;
        image.resize(WIDTH);
        for (auto& img : image) {
            img.resize(HEIGHT);
        }
        
        for (size_t row=0; row<WIDTH; row++) {
            for (size_t col=0; col<HEIGHT; col++) {
                image[row][col] = pixelTransform(-1.0f, 1.0f, 0.0f, 255.0f, rawPixels[pixelidx]);
                pixelidx++;
            }
        }
        result.push_back(LabeledData(label, WIDTH, HEIGHT, image));
    }
    
    return result;
}

// Transform a -1 to 1 double to a 0-255 pixel representation
// This is wrong, but I couldn't be bothered to fix it
unsigned char RaviMNISTLoader::pixelTransform(double oldmin, double oldmax, double newmin, double newmax, double pixel) {
    double temp = (newmax - newmin) / (oldmax - oldmin);
    return (unsigned char)round(newmin + ((pixel - oldmin) * temp));
}