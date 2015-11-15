//
//  LabeledData.h
//  HW4P3
//
//  Created by Matt on 11/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef __HW4P3__LabeledData__
#define __HW4P3__LabeledData__

#include "LabeledData.h"
#include <vector>
#include <utility>  // For std::pair
#include <limits>   // For std::numeric_limits

// MNIST Labeled Data
class LabeledData {
    private:
        // All of these were originally const until I needed std::swap for std::shuffle welp
        unsigned int label;
        bool binaryLabel;   // 1 = 0, 5 = 1
        unsigned int rows;
        unsigned int cols;
        std::vector<std::vector<unsigned char>> imageData;
        std::vector<double> features;
    
        // Feature Calculation Functions
        std::vector<double> calculateFeatures();
        double calcDensity();
        double calcSymmetry();
        std::pair<double, double> calcHorizontalIntersections();
        std::pair<double, double> calcVerticalIntersections();
        double transform(double input, double inputlow, double inputhigh, double outputlow, double outputhigh);
    
    public:
        // Parameterized Constructor
        LabeledData(unsigned int label, unsigned int rows, unsigned int cols, std::vector<std::vector<unsigned char>> imageData);
    
        // Get the label
        unsigned int getLabel();
    
        // Perceptron interface
        bool getBinaryLabel();
    
        // Set the label
        void setLabel(unsigned int newlabel);
    
        // Set binary label
        void setBinaryLabel(bool newLabel);
    
        // Get the number of rows in image
        unsigned int getRows();
    
        // Set the number of rows in image
        void setRows(unsigned int newrows);
    
        // Get the number of columns in image
        unsigned int getCols();
    
        // Set the number of columns in image
        void setCols(unsigned int newcols);
    
        // Get the image data
        std::vector<std::vector<unsigned char>> getData();
    
        // Set the image data
        void setData(std::vector<std::vector<unsigned char>>& newdata);
    
        // Get the features vector
        std::vector<double> getFeaturesVector();
};

#endif /* defined(__HW4P3__LabeledData__) */
