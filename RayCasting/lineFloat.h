#pragma once

#include "vector2d.h"

class lineFloat {
public:
	vector2d<float>* point1 = nullptr;
	vector2d<float>* point2 = nullptr;
	vector2d<float>* centrePoint = nullptr;
	lineFloat(vector2d<float> *p1, vector2d<float> *p2);
	lineFloat();
	float dX;
	float dY;
};