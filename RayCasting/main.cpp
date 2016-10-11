#include <Windows.h>
#include <GL\glew.h>
#include <GL\glut.h>
#include <math.h>
#include <iostream>
#include <string>
#include "vector2d.cpp"
#include "lineFloat.h"
#include "fileIO.h"

#define pi 3.14159265358979323846  

char title[] = "Ray Tracing Demo";

int windowWidth = 1280;
int windowHeight = 768;

int framerate = 60;


lineFloat* lines;
int numLines;

int mX, mY; //mouse positions

//calculates the angle from the vertical axis from source to point
double calculateAngle(vector2d<float> point, vector2d<float> source) {
	float x =point.getX() -source.getX();
	float y = point.getY() - source.getY();
	if (x < 0) {
		if (y >= 0)
			return atan(-y / x) + pi;
		return atan(x / y) + pi / 2;
	}
	else {
		if (y >= 0)
			return atan(x / y) + 3.0 / 2 * pi;
		return atan(-y / x);
	}
}

//uses calculateAngle and sorts from smallest (most negative) angle to largest angle, sorts using bubble sort currently
//WIP: use a more efficient sort like quick sort, heap sort, or merge sort
void sortByAngle(vector2d<float>* points, vector2d<float> source, int size) {
	float * angles = new float[size];
	for (int i = 0; i < size; i++) {
		angles[i] = calculateAngle(points[i], source);
	}
	//perform sorting

	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (int i = 0; i < size - 1; i++)
		{
			if (angles[i] > angles[i+1])
			{
				float temp = angles[i];
				vector2d<float> temp2 = points[i];
				angles[i] = angles[i + 1];
				points[i] = points[i + 1];
				angles[i + 1] = temp;
				points[i + 1] = temp2;
				sorted = false;
			}
		}
	}
	delete angles;
}

vector2d<float>screenVertices[] = { vector2d<float>(0.001,0.001), vector2d<float>(windowWidth,windowHeight), vector2d<float>(0,windowHeight), vector2d<float>(windowWidth,0) };
void initGL() {
	glClearColor(0, 0, 0, 0);
	gluOrtho2D(0, windowWidth, windowHeight, 0);
	glEnable(GL_MULTISAMPLE);
}


void drawCircle(double smoothness, int x, int y, double radius) {
	glBegin(GL_LINE_STRIP); {
		glColor3f(1.0, 0.0, 0.0);
		for (double i = 0; i <= smoothness; i++)
		{
			glVertex2d(x + cos(i / smoothness * 2 * pi)*radius, y + sin(i / smoothness * 2 * pi)*radius);
		}
	}	glEnd();
}

//NEEDS TO BE COMMENTED AT SOME POINT FOR CLARITY
vector2d<float> castRay(vector2d<float> rayP1, vector2d<float> rayP2, lineFloat edges[], int numEdges) {
	lineFloat ray;
	ray = lineFloat(&vector2d<float>(rayP1.getX(), rayP1.getY()), &vector2d<float>(rayP2.getX(), rayP2.getY()));

	double r_px = ray.point1->getX(); double r_py = ray.point1->getY();
	double r_dx = ray.point2->getX() - ray.point1->getX(); double r_dy = ray.point2->getY() - ray.point1->getY();
	double s_px;
	double s_py;
	double s_dx;
	double s_dy;
	double T1 = -1; double T2 = -1;
	int minLine = INT_MAX;
	double minLength = INT_MAX;
	double xPOI = 0;
	double yPOI = 0;
	double xPOI_final = 0;
	double yPOI_final = 0;

	for (int i = 0; i < numEdges; i++) {
		s_px = edges[i].point1->getX();	s_py = edges[i].point1->getY();
		s_dx = edges[i].point2->getX() - edges[i].point1->getX();	s_dy = edges[i].point2->getY() - edges[i].point1->getY();

		if ((s_dx*r_dy - s_dy*r_dx != 0)) {
			if (r_dx == 0) {
				r_dx = 0.001;
			}
			T2 = (r_dx*(s_py - r_py) + r_dy*(r_px - s_px)) / (s_dx*r_dy - s_dy*r_dx);
			T1 = (s_px + s_dx*T2 - r_px) / r_dx;
		}//lines are parallel, therefore we cannot calculate an intersection, doesn't exist, so just go to next iteration of loop
		else 
			continue;

		if (T1 > 0 && (T2 < 1 && T2 > 0)) {
			//calculate the point of intersection
			xPOI = ray.point1->getX() + T1*r_dx;
			yPOI = ray.point1->getY() + T1*r_dy;
			//calculate distance between the two tingz
			double distance = sqrt((xPOI - r_px)*(xPOI - r_px) + (yPOI - r_py)*(yPOI - r_py));
			// if this is a shorter solution than before, we update the values to reflect that
			if (distance < minLength) {
				minLine = i;
				minLength = distance;
				xPOI_final = xPOI;
				yPOI_final = yPOI;
			}
		}
		else
			continue;
	}
	if (minLine == INT_MAX) {
		xPOI_final = r_dx * 1000 + r_px;
		yPOI_final = r_dy * 1000 + r_py;
	}
	return vector2d<float>(xPOI_final, yPOI_final);
}

void display() {
	//sort vertices by angle from mouse
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(0.01);
	//render things here
	glColor3f(1.0, 0.0, 0.0);
	//make a square around the window
	glBegin(GL_QUADS); {
		glColor3f(0.0, 0.0, 0.0);
		glVertex2f(0, 0);
		glVertex2f(0, windowHeight);
		glVertex2f(windowWidth, windowHeight);
		glVertex2f(windowWidth, 0);
	}glEnd();


	glColor3f(0.0, 0.0, 1.0);
	glLineWidth(0.2);
	

	//castRay();
	vector2d<float> vertex;

	//
	vector2d<float>* rayPoints = (vector2d<float>*)malloc((6 * numLines + 4) * sizeof(vector2d<float>));

	int rayCount = 0;
	for (int i = 0; i < numLines*6; i++) {
		if (i % 6 == 0) //cast ray directly at the first vertex of the line
			vertex = castRay(vector2d<float>(mX, mY), *lines[i/6].point1, lines, numLines);
		else if ( i % 6 == 1) //cast ray directly at second vertex of line
			vertex = castRay(vector2d<float>(mX, mY), *lines[i/6].point2, lines, numLines);
		else if (i % 6 == 2) {//slightly away from vertex, increase angle from origin slightly, for vertex 1
			double dx = lines[i / 6].point1->getX() - mX; double dy = lines[i / 6].point1->getY() - mY;
			double radius = sqrt((dx*dx) + (dy*dy));
			double theta = calculateAngle(*lines[i / 6].point1, vector2d<float>(mX, mY));
			theta += 0.000001;
			double newX = mX + radius*cos(theta);
			double newY = mY - radius*sin(theta);
			vertex = castRay(vector2d<float>(mX, mY), vector2d<float>(newX, newY), lines, numLines);
		}
		else if (i % 6 == 3) {//away from vertex, positive angle direction, for vertex 2
			double dx = lines[i / 6].point2->getX() - mX; double dy = lines[i / 6].point2->getY() - mY;
			double radius = sqrt((dx*dx) + (dy*dy));
			double theta = calculateAngle(*lines[i / 6].point2, vector2d<float>(mX, mY));
			theta += 0.000001;
			double newX = mX + radius*cos(theta);
			double newY = mY - radius*sin(theta);
			vertex = castRay(vector2d<float>(mX, mY), vector2d<float>(newX, newY), lines, numLines);
		}
		else if (i % 6 == 4) {//negative angle direction vertex1
			double dx = lines[i / 6].point1->getX() - mX; double dy = lines[i / 6].point1->getY() - mY;
			double radius = sqrt((dx*dx) + (dy*dy));
			double theta = calculateAngle(*lines[i / 6].point1, vector2d<float>(mX, mY));
			theta -= 0.000001;
			double newX = mX + radius*cos(theta);
			double newY = mY - radius*sin(theta);
			vertex = castRay(vector2d<float>(mX, mY), vector2d<float>(newX, newY), lines, numLines);
		}
		else if (i % 6 == 5) {//negative angle direction vertex 2
			double dx = lines[i / 6].point2->getX() - mX; double dy = lines[i / 6].point2->getY() - mY;
			double radius = sqrt((dx*dx) + (dy*dy));
			double theta = calculateAngle(*lines[i / 6].point2, vector2d<float>(mX, mY));
			theta -= 0.000001;
			double newX = mX + radius*cos(theta);
			double newY = mY - radius*sin(theta);
			vertex = castRay(vector2d<float>(mX, mY), vector2d<float>(newX, newY), lines, numLines);
		}
		rayPoints[rayCount].setX(vertex.getX());
		rayPoints[rayCount].setY(vertex.getY());
		rayCount++;
	}

	//casts the rays for the corners of the screen
	rayPoints[numLines * 6] = castRay(vector2d<float>(mX, mY), vector2d<float>(0, 0), lines, numLines);
	rayPoints[numLines * 6 + 1] = castRay(vector2d<float>(mX, mY), vector2d<float>(windowWidth, 0), lines, numLines);
	rayPoints[numLines * 6 + 2] = castRay(vector2d<float>(mX, mY), vector2d<float>(0, windowHeight), lines, numLines);
	rayPoints[numLines * 6 + 3] = castRay(vector2d<float>(mX, mY), vector2d<float>(windowWidth, windowHeight), lines, numLines);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(windowWidth, 0);
	glVertex2f(windowWidth, windowHeight);
	glVertex2f(0, windowHeight);
	glEnd();

	
	//draws the visibility polygon on the screen as multiple triangles
	sortByAngle(rayPoints, vector2d<float>(mX, mY), numLines*6+4);

	glColor3f(0.9, 0.8, 0.01);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < numLines*6+5; i++) {
		glVertex2f(mX, mY);
		glVertex2f(rayPoints[i%(numLines*6+4)].getX(), rayPoints[i % (numLines * 6 + 4)].getY());
		glVertex2f(rayPoints[(i + 1)% (numLines * 6 + 4)].getX(), rayPoints[(i + 1) % (numLines * 6 + 4)].getY());
	}
	glEnd();
	/*
	//draws the rays	
	glColor3f(0.5, 0.5, 0.1);
	for (int i = 0; i < numLines*6; i++) {
		glBegin(GL_LINES);
		glVertex2f(rayPoints[i].getX(), rayPoints[i].getY());
		glVertex2f(mX, mY);
		glEnd();
	}
	*/
	//draw the circle where the mouse/player is
	drawCircle(15, mX, mY, 5);
	

	//draw the lines
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(1);
	glBegin(GL_LINES);
	for (int i = 0; i < (numLines); i++) {
		glVertex2f(lines[i].point1->getX(), lines[i].point1->getY());
		glVertex2f(lines[i].point2->getX(), lines[i].point2->getY());
	}
	glEnd();

	free(rayPoints);

	glutSwapBuffers();
}

void Timer(int value) {
	glutPostRedisplay(); //post a paint request to activate display
	glutTimerFunc(1 / framerate, Timer, 0);
}

void mouseUpdate(int x, int y) {
	mX = x;
	mY = y;
}

//free memory for the line variable
void cleanUp() {
	for (int i = 0; i < numLines; i++) {
		free(lines[i].point1);
		free(lines[i].point2);
	}
	free(lines);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow(title);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(mouseUpdate);
	glutTimerFunc(0, Timer, 0);
	initGL();

	//before entering the main loop we read in the line segment file
	lines = readFile("test.txt", &numLines);
	glutMainLoop();
	cleanUp();
	return 0;
}