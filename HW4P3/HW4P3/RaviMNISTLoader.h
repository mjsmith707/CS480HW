//
//  RaviMNISTLoader.h
//  HW4P3
//
//  Created by Matt on 11/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef __HW4P3__RaviMNISTLoader__
#define __HW4P3__RaviMNISTLoader__

#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include "LabeledData.h"

class RaviMNISTLoader {
    private:
        // Constant width and height from Ravi's dataset
        static const size_t WIDTH = 20;
        static const size_t HEIGHT = 20;
        
        std::vector<unsigned int> readLabels(std::fstream& labelFile);
        std::vector<double> readPixels(std::fstream& imageFile);
        std::vector<LabeledData> createLabeledData(std::vector<unsigned int>& labels, std::vector<double>& rawPixels);
        unsigned char pixelTransform(double oldmin, double oldmax, double newmin, double newmax, double pixel);
    
    public:
        RaviMNISTLoader();
    
        // Read the dataset from two files and return a set of LabeledData
        std::vector<LabeledData> readDataSet(std::string labelFile, std::string imageFile);
};

#endif /* defined(__HW4P3__RaviMNISTLoader__) */
