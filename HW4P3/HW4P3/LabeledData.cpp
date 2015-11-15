//
//  LabeledData.cpp
//  HW4P3
//
//  Created by Matt on 11/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "LabeledData.h"

// Parameterized constructor
LabeledData::LabeledData(unsigned int label, unsigned int rows, unsigned int cols, std::vector<std::vector<unsigned char>> imageData)
    : label(label), rows(rows), cols(cols), imageData(imageData) {
    this->features = calculateFeatures();
    this->binaryLabel = false;
}

// Get the label for the data
unsigned int LabeledData::getLabel() {
    return this->label;
}

bool LabeledData::getBinaryLabel() {
    return this->binaryLabel;
}

// Set the label for the data
void LabeledData::setLabel(unsigned int newlabel) {
    this->label = newlabel;
}

// Set binary label
void LabeledData::setBinaryLabel(bool newLabel) {
    this->binaryLabel = newLabel;
}

// Get the number of rows for the data
unsigned int LabeledData::getRows() {
    return this->rows;
}

// Set the number of rows for the data
void LabeledData::setRows(unsigned int newrows) {
    this->rows = newrows;
}

// Get the number of columns for the data
unsigned int LabeledData::getCols() {
    return this->cols;
}

// Set the number of columns for the data
void LabeledData::setCols(unsigned int newcols) {
    this->cols = newcols;
}

// Get the image data
std::vector<std::vector<unsigned char>> LabeledData::getData() {
    return this->imageData;
}

// Set the image data
void LabeledData::setData(std::vector<std::vector<unsigned char>>& newdata) {
    this->imageData = newdata;
}

// Return the features vector for this data
std::vector<double> LabeledData::getFeaturesVector() {
    return this->features;
}

// Calculate the features
// 0 = density
// 1 = symmetry
// 2 = min intersections (horizontal + vertical)
// 3 = max intersections (horizontal + vertical)
std::vector<double> LabeledData::calculateFeatures() {
    std::vector<double> result;
    result.resize(5);
    
    result[0] = calcDensity();
    result[1] = calcSymmetry();
    std::pair<double, double> horiz = calcHorizontalIntersections();
    std::pair<double, double> vert = calcVerticalIntersections();
    result[2] = transform(std::get<0>(horiz) + std::get<0>(vert), std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), -1.0f, 1.0f);
    result[3] = transform(std::get<1>(horiz) + std::get<1>(vert), std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), -1.0f, 1.0f);
    result[4] = -1.0f;  // I still haven't figured this out
    
    return result;
}

// Calculate the image density
double LabeledData::calcDensity() {
    double density = 0.0f;
    
    for (auto& i : imageData) {
        for (auto& j : i) {
            density += j;
        }
    }
    
    density /= rows * cols;
    
    density = transform(density, std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), -1.0f, 1.0f);
    
    return density;
}

// Horizontal image flip with xor and count
double LabeledData::calcSymmetry() {
    double symmetry = 0.0f;
    
    std::vector<std::vector<unsigned char>> flipImage;
    flipImage.resize(rows);
    for (size_t i=0; i<rows; i++) {
        flipImage[i].resize(cols);
    }
    
    // Flip image
    for (size_t row=0; row<rows; row++) {
        for (size_t col=0, flipcol = cols-1; col<cols; col++, flipcol--) {
            flipImage[row][flipcol] = imageData[row][col];
        }
    }
    
    // XOR
    for (size_t row=0; row<rows; row++) {
        for (size_t col=0; col<cols; col++) {
            flipImage[row][col] ^= imageData[row][col];
        }
    }
    
    // Count
    for (auto& i : flipImage) {
        for (auto& j : i) {
            symmetry += j;
        }
    }
    
    symmetry /= rows * cols;
    
    symmetry = transform(symmetry, std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), -1.0f, 1.0f);
    
    return symmetry;
}

// Calculate max and min horizontal intersections
std::pair<double, double> LabeledData::calcHorizontalIntersections() {
    double minimum = std::numeric_limits<double>::max();
    double maximum = 0.0f;
    std::pair<double, double> result;
    
    for (size_t row=0; row<rows; row++) {
        unsigned char lastpixel = imageData[row][0];
        size_t intersectionCount = 0;
        if (lastpixel > 0) {
            intersectionCount++;
        }
        for (size_t col=1; col<cols; col++) {
            if ((lastpixel > 0) && (imageData[row][col] == 0)) {
                intersectionCount++;
            }
            else if ((lastpixel == 0) && (imageData[row][col] > 0)) {
                intersectionCount++;
            }
            lastpixel = imageData[row][col];
        }
        maximum = intersectionCount > maximum ? intersectionCount : maximum;
        minimum = (intersectionCount > 0) && (intersectionCount < minimum) ? intersectionCount : minimum;
    }
    
    result = std::make_pair(minimum, maximum);
    
    return result;
}

// Calculate max and min vertical intersections
std::pair<double, double> LabeledData::calcVerticalIntersections() {
    double minimum = std::numeric_limits<double>::max();
    double maximum = 0.0f;
    std::pair<double, double> result;
    
    for (size_t col=0; col<cols; col++) {
        unsigned char lastpixel = imageData[0][col];
        size_t intersectionCount = 0;
        if (lastpixel > 0) {
            intersectionCount++;
        }
        for (size_t row=1; row<rows; row++) {
            if ((lastpixel > 0) && (imageData[row][col] == 0)) {
                intersectionCount++;
            }
            else if ((lastpixel == 0) && (imageData[row][col] > 0)) {
                intersectionCount++;
            }
            lastpixel = imageData[row][col];
        }
        maximum = intersectionCount > maximum ? intersectionCount : maximum;
        minimum = (intersectionCount > 0) && (intersectionCount < minimum) ? intersectionCount : minimum;
    }
    
    result = std::make_pair(minimum, maximum);
    
    return result;
}

double LabeledData::transform(double input, double inputlow, double inputhigh, double outputlow, double outputhigh) {
    return ((input - inputlow) / (inputhigh - inputlow)) * (outputhigh - outputlow) + outputlow;
}