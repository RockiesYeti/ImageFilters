#include "image.h"
#include <map>
#include <cmath>
#include <list>
#include <iterator>
/*
 * Sets a value to the char array starting at the offset using the size
 * specified by the bytes.
 * This is a helper function for write_image()
 * @param arr    Array to set values for
 * @param offset Starting index offset
 * @param bytes  Number of bytes to set
 * @param value  Value to set
 */
void set_bytes(unsigned char arr[], int offset, int bytes, int value) {
    for (int i = 0; i < bytes; i++) {
        arr[offset+i] = (unsigned char)(value>>(i*8));
    }
}

/** 
 * Write the input image to a BMP file name specified
 * @param filename The BMP file name to save the image to
 * @param image    The input image to save
 * @return True if successful and false otherwise
 */
bool write_image(string filename, const vector<vector<vector<int> > >& image) {
    // Get the image width and height in pixels
    int width_pixels = image[0].size();
    int height_pixels = image.size();

    // Calculate the width in bytes incorporating padding (4 byte alignment)
    int width_bytes = width_pixels * 3;
    int padding_bytes = 0;
    padding_bytes = (4 - width_bytes % 4) % 4;
    width_bytes = width_bytes + padding_bytes;

    // Pixel array size in bytes, including padding
    int array_bytes = width_bytes * height_pixels;

    // Open a file stream for writing to a binary file
    fstream stream;
    stream.open(filename, ios::out | ios::binary);

    // If there was a problem opening the file, return false
    if (!stream.is_open()) {
        return false;
    }

    // Create the BMP and DIB Headers
    const int BMP_HEADER_SIZE = 14;
    const int DIB_HEADER_SIZE = 40;
    unsigned char bmp_header[BMP_HEADER_SIZE] = {0};
    unsigned char dib_header[DIB_HEADER_SIZE] = {0};

    // BMP Header
    set_bytes(bmp_header,  0, 1, 'B');              // ID field
    set_bytes(bmp_header,  1, 1, 'M');              // ID field
    set_bytes(bmp_header,  2, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE+array_bytes); // Size of BMP file
    set_bytes(bmp_header,  6, 2, 0);                // Reserved
    set_bytes(bmp_header,  8, 2, 0);                // Reserved
    set_bytes(bmp_header, 10, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE); // Pixel array offset

    // DIB Header
    set_bytes(dib_header,  0, 4, DIB_HEADER_SIZE);  // DIB header size
    set_bytes(dib_header,  4, 4, width_pixels);     // Width of bitmap in pixels
    set_bytes(dib_header,  8, 4, height_pixels);    // Height of bitmap in pixels
    set_bytes(dib_header, 12, 2, 1);                // Number of color planes
    set_bytes(dib_header, 14, 2, 24);               // Number of bits per pixel
    set_bytes(dib_header, 16, 4, 0);                // Compression method (0=BI_RGB)
    set_bytes(dib_header, 20, 4, array_bytes);      // Size of raw bitmap data (including padding)                     
    set_bytes(dib_header, 24, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 28, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 32, 4, 0);                // Number of colors in palette
    set_bytes(dib_header, 36, 4, 0);                // Number of important colors

    // Write the BMP and DIB Headers to the file
    stream.write((char*)bmp_header, sizeof(bmp_header));
    stream.write((char*)dib_header, sizeof(dib_header));

    // Initialize pixel and padding
    unsigned char pixel[3] = {0};
    unsigned char padding[3] = {0};

    // Pixel Array (Left to right, bottom to top, with padding)
    for (int h = 0; h < height_pixels; h++) {
        for (int w = 0; w < width_pixels; w++) {
            // Write the pixel (Blue, Green, Red)
            pixel[0] = image[h][w][0];
            pixel[1] = image[h][w][1];
            pixel[2] = image[h][w][2];
            stream.write((char*)pixel, 3);
        }
        // Write the padding bytes
        stream.write((char *)padding, padding_bytes);
    }

    // Close the stream and return true
    stream.close();
    return true;
}

/** 
 * Gets an integer from a binary stream.
 * @param stream the stream
 * @param offset the offset at which to read the integer
 * @return the integer starting at the given offset
 */ 
int get_int(fstream& stream, int offset)
{
    stream.seekg(offset);
    int result = 0;
    int base = 1;
    for (int i = 0; i < 4; i++)
    {   
        result = result + stream.get() * base;
        base = base * 256;
    }
    return result;
}


// Adds vignette effect to the input image and returns the resulting image
vector<vector<vector<int> > > process_1(const vector<vector<vector<int> > >& image,int height, int width)
{
    int rows = width;
    int columns = height;
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            double distance = sqrt((pow(w - (width / 2), 2)) + (pow(h - (height / 2), 2)));
            double scaling_factor = (((height - distance) / height)+((width - distance) / width))/2;
            new_image[w][h][0] = image[w][h][0] * scaling_factor;
            new_image[w][h][1] = image[w][h][1] * scaling_factor;
            new_image[w][h][2] = image[w][h][2] * scaling_factor;
            //std::cout << distance << " " << scaling_factor << " " << h << " " << w << endl;
        }
    }    
    return new_image;
}

// Adds claredon effect to the input image and returns the resulting image
vector<vector<vector<int> > > process_2(const vector<vector<vector<int> > >& image, int height, int width)
{
    int rows = width;
    int columns = height;
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    double scaling_factor = 0.3;

    std::cout << "Please enter the scaling factor for the effect: (between 0 and 1)" << endl;
    std::cin >> scaling_factor;
    std::cin.clear();
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
    int newblue=0, newred=0, newgreen=0;
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            
            int b = image[w][h][0];
            int g = image[w][h][1];
            int r = image[w][h][2];
            double average = (b + g + r) / 3;
            if (average > 170) {
                newblue = int(255 - (255 - b) * scaling_factor);
                newgreen = int(255 - (255 - g) * scaling_factor);
                newred = int(255 - (255 - r) * scaling_factor);
            }
            else if (average < 90) {
                newblue = b * scaling_factor;
                newgreen = g * scaling_factor;
                newred = r * scaling_factor;
            }
            else {
                newblue = b ;
                newgreen = g ;
                newred = r ;
            }
            new_image[w][h][0] = newblue;
            new_image[w][h][1] = newgreen;
            new_image[w][h][2] = newred;
        }
    }
    return new_image;
}

// Adds grayscale effect to the input image and returns the resulting image
vector<vector<vector<int> > > process_3(const vector<vector<vector<int> > >& image, int height, int width)
{
    int rows = width;
    int columns = height;
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            int b = image[w][h][0];
            int g = image[w][h][1];
            int r = image[w][h][2];
            double average = (b + g + r) / 3;
            new_image[w][h][0] = average;
            new_image[w][h][1] = average;
            new_image[w][h][2] = average;
        }
    }
    return new_image;
}

// Rotates the input image by 90 degrees clockwise and returns the resulting image
vector<vector<vector<int> > > process_4(const vector<vector<vector<int> > >& image, int height, int width)
{
    int rows = height;
    int columns = width;
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            new_image[h][w][0] = image[w][(height-1)-h][0];
            new_image[h][w][1] = image[w][(height-1)-h][1];
            new_image[h][w][2] = image[w][(height-1)-h][2];
        }
    }
    return new_image;
}

// Rotates image by the specified multiple of 90 degrees clockwise and returns the resulting image
vector<vector<vector<int> > > process_5(const vector<vector<vector<int> > >& image,  int height, int width)
{
    vector<vector<vector<int>>> new_image = image;
    int rotations = 0;
    std::cout << "Please enter the number of rotations:" << endl;
    std::cin >> rotations;
    std::cin.clear();
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < rotations; i++) {
        int new_width = new_image.size();
        int new_height = new_image[0].size();
        new_image = process_4(new_image,new_height, new_width);
    }
    return new_image;
}

// Enlarges the input image in the x and y direction by the scales specified and returns the resulting image
vector<vector<vector<int> > > process_6(const vector<vector<vector<int> > >& image,  int height, int width)
{
    int x_scale = 0, y_scale = 0;
    std::cout << "Please enter the x scale factor:" << endl;
    std::cin >> x_scale ;
    std::cin.clear();
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
    std::cout << "Please enter the y scale factor:" << endl;
    std::cin >> y_scale;
    std::cin.clear();
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
    int rows = width*y_scale;
    int columns = height*x_scale;
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    for (int w = 0; w < rows; w++) {
        for (int h = 0; h < columns; h++) {
            new_image[w][h][0] = image[w/y_scale][h/x_scale][0] ;
            new_image[w][h][1] = image[w/y_scale][h/x_scale][1] ;
            new_image[w][h][2] = image[w/y_scale][h/x_scale][2] ;
        }
    }
    return new_image;
}

// Converts the input image to high contrast and returns the resulting image
vector<vector<vector<int> > > process_7(const vector<vector<vector<int> > >& image, int height, int width)
{
    int rows = width;
    int columns = height;
    int newblue = 0, newred = 0, newgreen = 0;
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            int b = image[w][h][0];
            int g = image[w][h][1];
            int r = image[w][h][2];
            double average = (b + g + r) / 3;
            if (average >= 255 / 2) {
                newred = 255;
                newgreen = 255;
                newblue = 255;
            }
            else {
                newred = 0;
                newgreen = 0;
                newblue = 0;
            }
            new_image[w][h][0] = newblue;
            new_image[w][h][1] = newgreen;
            new_image[w][h][2] = newred;
        }
    }
    return new_image;
}

// Lightens the input image and returns the resulting image
vector<vector<vector<int> > > process_8(const vector<vector<vector<int> > >& image, int height, int width)
{
    int rows = width;
    int columns = height;
    double scaling_factor = 0.3;
    std::cout << "Please enter the scaling factor for the effect: (between 0 and 1)" << endl;
    std::cin >> scaling_factor;
    std::cin.clear();
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            new_image[w][h][0] = int(255 - (255 - image[w][h][0]) * scaling_factor);
            new_image[w][h][1] = int(255 - (255 - image[w][h][1]) * scaling_factor);
            new_image[w][h][2] = int(255 - (255 - image[w][h][2]) * scaling_factor);
        }
    }
    return new_image;
}




// Darkens image the input image and returns the resulting image
vector<vector<vector<int> > > process_9(const vector<vector<vector<int> > >& image, int height, int width)
{
    int rows = width;
    int columns = height;
    double scaling_factor = 0.3;
    std::cout << "Please enter the scaling factor for the effect: (between 0 and 1)" << endl;
    std::cin >> scaling_factor;
    std::cin.clear();
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            new_image[w][h][0] = image[w][h][0] * scaling_factor;
            new_image[w][h][1] = image[w][h][1] * scaling_factor;
            new_image[w][h][2] = image[w][h][2] * scaling_factor;
        }
    }
    return new_image;
}

// Converts the input image to black, white, red, blue, and green only and returns the resulting image
vector<vector<vector<int> > > process_10(const vector<vector<vector<int> > >& image, int height, int width)
{
    int rows = width;
    int columns = height;
    int newblue = 0, newred = 0, newgreen = 0;
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            
           int b = image[w][h][0];
           int g = image[w][h][1];
           int r = image[w][h][2];
           int sumV = b + g + r;
           int maxV = maximum(b,g,r);

           if (sumV >= 550) {
               newred = 255;
               newgreen = 255;
               newblue = 255;
           }
           else if (sumV <= 150) {
               newred = 0;
               newgreen = 0;
               newblue = 0;
           }
           else if (maxV == r) {
               newred = 255;
               newgreen = 0;
               newblue = 0;
           }
           else if (maxV == b) {
               newred = 0;
               newgreen = 0;
               newblue = 255;
           }
           else {
               newred = 0;
               newgreen = 255;
               newblue = 0;
           }
           new_image[w][h][0] = newblue;
           new_image[w][h][1] = newgreen;
           new_image[w][h][2] = newred;
        }
    }


    return new_image;
}


//Calculates Max of three int's (for RGB)
int maximum(int a, int b, int c){
    int max = (a < b) ? b : a;
    return ((max < c) ? c : max);
}

//Creates a path for the output file
string createOutputPath(path originalFilePath, string processName) {
    path path2out;
    if (originalFilePath.has_parent_path())
    {
        path2out = originalFilePath.parent_path();
        path2out += "\\";
        path2out += originalFilePath.stem();
        path2out += "_" + processName;
        path2out += originalFilePath.extension();
    }
    else {
        path2out = originalFilePath.stem();
        path2out += "_" + processName;
        path2out += originalFilePath.extension();
    }
    return path2out.string();
}

//reads in image file and calls image processors
int readInImageFile(path inputFile, int userChoice, map<int, ImageProcessor> mapOfProcessors) {
    fstream stream;
    stream.open(inputFile, ios::in | ios::out | ios::binary);
    int file_size = get_int(stream, 2);
    int start = get_int(stream, 10);
    int height = get_int(stream, 18);
    int width = get_int(stream, 22);

    int scanline_size = width * 3;
    int padding = 0;
    if (scanline_size % 4 != 0) {
        padding = 4 - scanline_size % 4;
    }

    if (file_size != start + (scanline_size + padding) * height) {
        std::cout << "Not a 24-bit true color image file." << endl;
        return 1;
    }
    int rows = width;
    int columns = height;
    vector<vector<vector<int>>> image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    int pos = start;
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            stream.seekg(pos);
            image[w][h][0] = stream.get();
            image[w][h][1] = stream.get();
            image[w][h][2] = stream.get();
            pos += 3;
        }
    } 

    bool wrImage;
    path fileOutPath;
    string tmpStr;
    std::cin.clear();
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
    vector<vector<vector<int>>> new_image(rows, vector<vector<int> >(columns, vector<int>(3, 0)));
    switch (userChoice) {
    case 1:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_1(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!"<<endl <<endl<< endl;
        break;
    case 2:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_2(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!" << endl << endl << endl;
        break;
    case 3:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_3(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!" << endl << endl << endl;
        break;
    case 4:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_4(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!" << endl << endl << endl;
        break;
    case 5:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_5(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!" << endl << endl << endl;
        break;
    case 6:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_6(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!" << endl << endl << endl;
        break;
    case 7:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_7(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!" << endl << endl << endl;
        break;
    case 8:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_8(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!" << endl << endl << endl;
        break;
    case 9:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_9(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!" << endl << endl << endl;
        break;
    case 10:
        fileOutPath = createOutputPath(inputFile, mapOfProcessors.find(userChoice)->second.name);
        std::cout << "Default output filename: " << fileOutPath << " If you would like to save the file to a different location;" << endl << "Enter a new path now or just hit enter to accept the default: " << endl;
        getline(cin, tmpStr);
        if (tmpStr != "") { fileOutPath = tmpStr; }
        new_image = process_10(image, height, width);
        wrImage = write_image(fileOutPath.string(), new_image);
        std::cout << fileOutPath << "has been created!" << endl << endl << endl;
        break;
    case 99:
        return 2;
    }
    return 0;
}
