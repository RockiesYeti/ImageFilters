#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <string>
#include <iterator>
#include <map>
#include <filesystem>
#include <vector>
#include <fstream>

using namespace std;
using namespace std::filesystem;

struct ImageProcessor {
	string name;
	string description;
};


/**
 * Write the input image to a BMP file name specified
 * @param filename The BMP file name to save the image to
 * @param image    The input image to save
 * @return True if successful and false otherwise
 */
bool write_image(string filename, const vector<vector<vector<int> > >& image);

// Reads the BMP image specified and returns the resulting image
vector<vector<vector<int> > > read_image(string filename);

// Adds vignette effect to the input image and returns the resulting image
vector<vector<vector<int> > > process_1(const vector<vector<vector<int> > >& image);

// Adds claredon effect to the input image and returns the resulting image
vector<vector<vector<int> > > process_2(const vector<vector<vector<int> > >& image);

// Adds grayscale effect to the input image and returns the resulting image
vector<vector<vector<int> > > process_3(const vector<vector<vector<int> > >& image);

// Rotates the input image by 90 degrees clockwise and returns the resulting image
vector<vector<vector<int> > > process_4(const vector<vector<vector<int> > >& image);

// Rotates image by the specified multiple of 90 degrees clockwise and returns the resulting image
vector<vector<vector<int> > > process_5(const vector<vector<vector<int> > >& image, int number);

// Enlarges the input image in the x and y direction by the scales specified and returns the resulting image
vector<vector<vector<int> > > process_6(const vector<vector<vector<int> > >& image, int x_scale, int y_scale);

// Converts the input image to high contrast and returns the resulting image
vector<vector<vector<int> > > process_7(const vector<vector<vector<int> > >& image);

// Lightens the input image and returns the resulting image
vector<vector<vector<int> > > process_8(const vector<vector<vector<int> > >& image, double scaling_factor);

// Darkens image the input image and returns the resulting image
vector<vector<vector<int> > > process_9(const vector<vector<vector<int> > >& image, double scaling_factor);

// Converts the input image to black, white, red, blue, and green only and returns the resulting image
vector<vector<vector<int> > > process_10(const vector<vector<vector<int> > >& image);

//prepares output file path
string createOutputPath(path originalFilePath, string processName);

int readInImageFile(path inputFile, int userChoice, map<int, ImageProcessor> mapOfProcessors);

int maximum(int a, int b, int c);


#endif
