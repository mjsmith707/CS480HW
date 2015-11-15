//
//  MNISTLoader.cpp
//  HW4P3
//
//  Created by Matt on 11/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "MNISTLoader.h"

MNISTLoader::MNISTLoader() {

}

std::vector<LabeledData> MNISTLoader::readDataSet(std::string labelFilePath, std::string imageFilePath) {
    std::fstream labelFile;
    labelFile.open(labelFilePath, std::fstream::in | std::fstream::binary);
    if (!labelFile.is_open()) {
        throw std::runtime_error("Failed to open label data file.");
    }
    
    std::fstream imageFile;
    imageFile.open(imageFilePath, std::fstream::in | std::fstream::binary);
    if (!imageFile.is_open()) {
        throw std::runtime_error("Failed to open image data file.");
    }
    
    if (!readLabelHeader(labelFile)) {
        throw std::runtime_error("Failed to read label file header.");
    }
    
    if (!readImageHeader(imageFile)) {
        throw std::runtime_error("Failed to read image file header.");
    }
    
    // Read Label File
    unsigned int labelNumItems = readLabelNumItems(labelFile);
    std::vector<unsigned char> labels = readLabels(labelFile, labelNumItems);
    
    // Read Image File
    unsigned int imageNumImages = readImageNumImages(imageFile);
    unsigned int imageNumRows = readImageNumRows(imageFile);
    unsigned int imageNumCols = readImageNumCols(imageFile);
    std::vector<unsigned char> pixels = readPixels(imageFile, (imageNumImages * imageNumRows * imageNumCols));
    
    // Create Labeled Set
    std::vector<LabeledData> result = createLabeledSet(labels, pixels, imageNumRows, imageNumCols);
    
    return result;
}

// Read the magic number from the label file
bool MNISTLoader::readLabelHeader(std::fstream& labelfile) {
    char* header = (char*)malloc(sizeof(char) * 4);
    if (!labelfile.read(header, 4)) {
        throw std::runtime_error("Failed to read 4 bytes for label file header.");
    }
    BEByteInt leheader;
    leheader.bytes[0] = header[3];
    leheader.bytes[1] = header[2];
    leheader.bytes[2] = header[1];
    leheader.bytes[3] = header[0];
    
    free(header);
    return leheader.val == LABEL_MAGIC_NUMBER;
}

// Read the magic number from the image file
bool MNISTLoader::readImageHeader(std::fstream& imagefile) {
    char* header = (char*)malloc(sizeof(char) * 4);
    if (!imagefile.read(header, 4)) {
        throw std::runtime_error("Failed to read 4 bytes for label file header.");
    }
    BEByteInt leheader;
    leheader.bytes[0] = header[3];
    leheader.bytes[1] = header[2];
    leheader.bytes[2] = header[1];
    leheader.bytes[3] = header[0];
    
    free(header);
    return leheader.val == IMAGE_MAGIC_NUMBER;
}

// Read the number of items in the label file
unsigned int MNISTLoader::readLabelNumItems(std::fstream& labelfile) {
    char* buff = (char*)malloc(sizeof(char) * 4);
    if (!labelfile.read(buff, 4)) {
        throw std::runtime_error("Failed to read 4 bytes for label file number of items.");
    }
    
    BEByteInt lebytes;
    lebytes.bytes[0] = buff[3];
    lebytes.bytes[1] = buff[2];
    lebytes.bytes[2] = buff[1];
    lebytes.bytes[3] = buff[0];
    
    free(buff);
    return lebytes.val;
}

// Read the labels into a vector
std::vector<unsigned char> MNISTLoader::readLabels(std::fstream& labelfile, unsigned int numItems) {
    std::vector<unsigned char> labels;
    labels.resize(numItems);
    char* buff = (char*)malloc(sizeof(char));
    
    for (unsigned int i=0; i<numItems; i++) {
        if (!labelfile.read(buff, 1)) {
            throw std::runtime_error("Failed to read 1 byte from label file.");
        }
        labels[i] = buff[0];
    }

    free(buff);
    return labels;
}

// Read number of images from image file
unsigned int MNISTLoader::readImageNumImages(std::fstream& imagefile) {
    char* buff = (char*)malloc(sizeof(char) * 4);
    if (!imagefile.read(buff, 4)) {
        throw std::runtime_error("Failed to read 4 bytes for image file number of images.");
    }
    
    BEByteInt lebytes;
    lebytes.bytes[0] = buff[3];
    lebytes.bytes[1] = buff[2];
    lebytes.bytes[2] = buff[1];
    lebytes.bytes[3] = buff[0];
    
    free(buff);
    return lebytes.val;
}

// Read number of rows from image file
unsigned int MNISTLoader::readImageNumRows(std::fstream& imagefile) {
    char* buff = (char*)malloc(sizeof(char) * 4);
    if (!imagefile.read(buff, 4)) {
        throw std::runtime_error("Failed to read 4 bytes for image file number of rows.");
    }
    
    BEByteInt lebytes;
    lebytes.bytes[0] = buff[3];
    lebytes.bytes[1] = buff[2];
    lebytes.bytes[2] = buff[1];
    lebytes.bytes[3] = buff[0];
    
    free(buff);
    return lebytes.val;
}

// Read number of cols from image file
unsigned int MNISTLoader::readImageNumCols(std::fstream& imagefile) {
    char* buff = (char*)malloc(sizeof(char) * 4);
    if (!imagefile.read(buff, 4)) {
        throw std::runtime_error("Failed to read 4 bytes for image file number of cols.");
    }
    
    BEByteInt lebytes;
    lebytes.bytes[0] = buff[3];
    lebytes.bytes[1] = buff[2];
    lebytes.bytes[2] = buff[1];
    lebytes.bytes[3] = buff[0];
    
    free(buff);
    return lebytes.val;
}

// Read pixels from image file
std::vector<unsigned char> MNISTLoader::readPixels(std::fstream& imagefile, unsigned int numItems) {
    std::vector<unsigned char> pixels;
    pixels.resize(numItems);
    
    char* buff = (char*)malloc(sizeof(char));
    
    for (size_t i=0; i<numItems; i++) {
        if (!imagefile.read(buff, 1)) {
            throw std::runtime_error("Failed to read 1 byte for image file pixels.");
        }
        
        pixels[i] = buff[0];
    }
    
    free(buff);
    return pixels;
}

// Create LabeledData set from labels and pixels
std::vector<LabeledData> MNISTLoader::createLabeledSet(std::vector<unsigned char>& labels, std::vector<unsigned char>& pixels, unsigned int imageNumRows, unsigned int imageNumCols) {
    std::vector<LabeledData> result;
    size_t pixelidx = 0;
    for (size_t i=0; i<labels.size(); i++) {
        unsigned char label = labels[i];
        std::vector<std::vector<unsigned char>> image;
        image.resize(imageNumRows);
        for (auto& img : image) {
            img.resize(imageNumCols);
        }
        
        for (size_t row=0; row<imageNumRows; row++) {
            for (size_t col=0; col<imageNumCols; col++) {
                image[row][col] = pixels[pixelidx];
                pixelidx++;
            }
        }
        
        result.push_back(LabeledData(label, imageNumRows, imageNumCols, image));
    }
    
    return result;
}