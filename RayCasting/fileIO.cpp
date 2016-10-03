// fileIO.cpp
//
#include "fileIO.h"

/*
This function reads a file "filename" and attempts to extract line segment positions from it
The text file is expected to have the following format
N
X1_1,Y1_1,X1_2,Y1_2
X2_1,Y2_1,X2_2,Y2_2
...
XN_1,YN_1,XN_2,YN_2

where N is the number of total line segments described in the file.
X1,Y1 would be the line segment's starting location, and X2,Y2 would be the ending location

it outputs these lines as a pointer to an array of lineFloat
*/
lineFloat* readFile(std::string fileName, int* lineCount) {
	std::ifstream file(fileName); //Open the file
	if (file.is_open())
	{
		std::cout << "File open Successfully." << std::endl;
	}
	else {
		std::cout << "File could not be open." << std::endl;
		return 0;
	}

	//read the number of line segments
	file >> *lineCount;
	lineFloat* lines = new lineFloat[*lineCount];
	char foo;
	//iterate for each line
	for (int i = 0; i < *lineCount + 1; i++) {
		//we need to keep track of the X1, Y1, X2, and Y2 variables
		int x1, y1, x2, y2;
		file >> x1 >> foo >> y1 >> foo >> x2 >> foo >> y2;
		lines[i] = lineFloat(new vector2d<float>(x1, y1), new vector2d<float>(x2, y2));
		//create the first line segment
	}
	file.close();
	return lines;
}