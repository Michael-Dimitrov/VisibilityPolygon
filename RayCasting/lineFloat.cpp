#include "vector2d.h"
#include "lineFloat.h"

lineFloat::lineFloat(vector2d<float> *p1, vector2d<float> *p2) {
	point1 = p1;
	point2 = p2;
	vector2d<float>* centre = new vector2d<float>(0,0);
	centre->setX((p1->getX() + p2->getX()) / 2);
	centre->setY((p1->getY() + p2->getY()) / 2);
	centrePoint = centre;
	dX = p2->getX() - p1->getX();
	dY = p2->getY() - p1->getY();
}

lineFloat::lineFloat() {
	point1 = nullptr;
	point2 = nullptr;
}