//
//  MNISTLoader.h
//  HW4P3
//
//  Created by Matt on 11/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef __HW4P3__MNISTLoader__
#define __HW4P3__MNISTLoader__

#include <vector>
#include <fstream>
#include <iostream>
#include "LabeledData.h"

// Loader for the real MNIST dataset
// You posted your .txt files right after I finished this...
class MNISTLoader {
    private:
        // Union for swapping between big and little endian
        union BEByteInt {
            unsigned char bytes[4];
            unsigned int val;
        };
    
        // MNIST Magic Numbers
        const unsigned int LABEL_MAGIC_NUMBER = 0x00000801;
        const unsigned int IMAGE_MAGIC_NUMBER = 0x00000803;
    
        // Header readers
        bool readLabelHeader(std::fstream& labelfile);
        bool readImageHeader(std::fstream& imagefile);
    
        // Label reading functions
        unsigned int readLabelNumItems(std::fstream& labelfile);
        std::vector<unsigned char> readLabels(std::fstream& labelfile, unsigned int numItems);
    
        // Image data reading functions
        unsigned int readImageNumImages(std::fstream& imagefile);
        unsigned int readImageNumRows(std::fstream& imagefile);
        unsigned int readImageNumCols(std::fstream& imagefile);
        std::vector<unsigned char> readPixels(std::fstream& imagefile, unsigned int numItems);
    
        // Labeled Data Generator
        std::vector<LabeledData> createLabeledSet(std::vector<unsigned char>& labels, std::vector<unsigned char>& pixels, unsigned int imageNumRows, unsigned int imageNumCols);
    
    public:
        MNISTLoader();
    
        // Read the dataset from a label file and image file and return a set of LabeledData
        std::vector<LabeledData> readDataSet(std::string labelFilePath, std::string imageFilePath);
};

#endif /* defined(__HW4P3__MNISTLoader__) */
