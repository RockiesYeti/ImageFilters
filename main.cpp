#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <string>
#include <fstream> 
#include <vector>
#include <iterator>
#include <map>
#include <filesystem>
#include "image.h"

using namespace std;
using namespace std::filesystem;



//
// To compile your code after making changes to any of the files:
// $ make
//
// To run your code after compiling:
// $ ./main
//

int main()
{
	bool exitOption = false;
	int progItr = 0;
	while (!exitOption) {
		std::cout << "Welcome to Image Processor Application" << endl << endl;
		if (progItr > 0) {
			std::cin.clear();
		}
		string filePath;
		string fileOutPath;
		bool fileCheck = false;
		path path2bmp;
		path currentDir = std::filesystem::current_path();
		while (!fileCheck) {
			std::cout << "Please Enter absolute path (or path relative to " << currentDir.string() << ") for bmp file to process or Q to exit: ";
			getline(cin, filePath);
			if (filePath == "Q") { exitOption = true; break; }
			path2bmp += filePath;
			if (std::filesystem::exists(path2bmp) && path2bmp.extension() == ".bmp") {
				fileCheck = true;
			}
			else {
				std::cout << "That is not an existing bmp file." << endl;
			}
		}
		if (filePath == "Q") { break; }
		std::cout << filePath << " is a valid bmp file. " << endl;

		std::map<int, ImageProcessor> mapOfProcessors;
		std::map<int, ImageProcessor>::iterator it;
		ImageProcessor process_1{ "vignette","Adds vignette effect to image (dark corners)" };
		mapOfProcessors[1] = process_1;
		ImageProcessor process_2{ "claredon","Adds claredon type effect to image - darks darker and lights lighter" };
		mapOfProcessors[2] = process_2;
		ImageProcessor process_3{ "grayscale","Grayscale image" };
		mapOfProcessors[3] = process_3;
		ImageProcessor process_4{ "rotate90","Rotates image by 90 degrees clockwise (not counter-clockwise)" };
		mapOfProcessors[4] = process_4;
		ImageProcessor process_5{ "rotate90x","Rotates image by multiples of 90 degrees clockwise" };
		mapOfProcessors[5] = process_5;
		ImageProcessor process_6{ "enlarge","Enlarges the image in the x and y direction" };
		mapOfProcessors[6] = process_6;
		ImageProcessor process_7{ "highcontrast","Convert image to high contrast - black and white only" };
		mapOfProcessors[7] = process_7;
		ImageProcessor process_8{ "lighten","Lightens image" };
		mapOfProcessors[8] = process_8;
		ImageProcessor process_9{ "darken","Darkens image" };
		mapOfProcessors[9] = process_9;
		ImageProcessor process_10{ "bwrgb","Converts image to only black, white, red, blue, and green" };
		mapOfProcessors[10] = process_10;
		std::cout << " Please select from the following image processing options: " << endl;
		for (it = mapOfProcessors.begin(); it != mapOfProcessors.end(); it++) {
			std::cout << "  " << it->first << " - " << it->second.name << " - " << it->second.description << endl;
		}
		std::cout << "  " << "99" << " - " << "Quit" << endl;
		int userChoice = 99;
		std::cin >> userChoice;
		int procStatus = readInImageFile(path2bmp, userChoice, mapOfProcessors);
		if (procStatus == 1) { return 1; }
		if (procStatus == 2) { return 0; }
		progItr++;
	}

    return 0;
}