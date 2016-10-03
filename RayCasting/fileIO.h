#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "lineFloat.h"
#include "vector2d.h"

lineFloat* readFile(std::string fileName, int* lineCount);