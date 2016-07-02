#include <Windows.h>
#include <GL\glew.h>
#include <GL\glut.h>
#include <math.h>
#include <iostream>
#include <string>
#include "linkedList.h"
#include "vector2d.cpp"
#include "lineFloat.h"

#define pi 3.14159265358979323846  

char title[] = "Ray Tracing Demo";

int windowWidth = 1600;
int windowHeight = 900;

int framerate = 60;

vector2d<float>vertices[] = { vector2d<float>(500,500), vector2d<float>(350,700),
vector2d<float>(150,150), vector2d<float>(1300,250), vector2d<float>(100,400), vector2d<float>(400,400) };

vector2d<float> v1 = vector2d<float>(250, 100);
vector2d<float> v2 = vector2d<float>(200, 300);
vector2d<float> v3 = vector2d<float>(650, 300);
vector2d<float> v4 = vector2d<float>(400, 150);
vector2d<float> v5 = vector2d<float>(600, 400);
vector2d<float> v6 = vector2d<float>(1000, 350);
vector2d<float> v7 = vector2d<float>(450, 650);
vector2d<float> v8 = vector2d<float>(300, 350);
lineFloat lines[] = { lineFloat(&v1, &v2), lineFloat(&v3, &v4),
lineFloat(&v5, &v6), lineFloat(&v7, &v8) };

//stupid fucking bullshit line intersection 

/*
r_px = mX; r_py = mY;
r_dx = mX - lines[j].point1->getX() ;
r_dy = lines[j].point1->getY() - mY;
s_px = lines[j].point1->getX();
s_py = lines[j].point1->getY();
s_dx = lines[j].dX;
s_dy = lines[j].dY;
if ((s_dx*r_dy - s_dy*r_dx != 0) && (r_dx != 0)) {
T2 = (r_dx*(s_py - r_py) + r_dy*(r_px - s_px)) / (s_dx*r_dy - s_dy*r_dx);
T1 = (s_px + s_dx*T2 - r_px) / r_dx;
}
else {
T2 = -1; T1 = -1;
}
*/

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
	//perform meme tier sort (bubble sort)

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




void castRay() {
	lineFloat ray;
	for (int k = 0; k < 24; k++) {
		if (k % 6 == 0) {
			ray = lineFloat(new vector2d<float>(mX, mY), new vector2d<float>(lines[k / 6].point1->getX(), lines[k / 6].point1->getY()));
		}
		else if (k % 6 == 1) {
			ray = lineFloat(new vector2d<float>(mX, mY), new vector2d<float>(lines[k / 6].point2->getX(), lines[k / 6].point2->getY()));
		}//make ray but with degree + 1
		else if (k % 6 == 2) {
			float origX = lines[k / 6].point1->getX();
			float origY = lines[k / 6].point1->getY();
			float theta = atan(origY / origX);
			float radius = sqrt(origX*origX + origY*origY);
			theta+= 0.00001* pi/180;
			float newX = radius*cos(theta);
			float newY = radius*sin(theta);
			ray = lineFloat(new vector2d<float>(mX, mY), new vector2d<float>(newX, newY));
		}//for point 2 with degree + 1
		else if (k % 6 == 3) {
			float origX = lines[k / 6].point2->getX();
			float origY = lines[k / 6].point2->getY();
			float theta = atan(origY / origX);
			float radius = sqrt(origX*origX + origY*origY);
			theta += 0.00001* pi / 180;
			float newX = radius*cos(theta);
			float newY = radius*sin(theta);
			ray = lineFloat(new vector2d<float>(mX, mY), new vector2d<float>(newX, newY));
		}//point 1 degree - 1
		else if (k % 6 == 4) {
			float origX = lines[k / 6].point1->getX();
			float origY = lines[k / 6].point1->getY();
			float theta = atan(origY / origX);
			float radius = sqrt(origX*origX + origY*origY);
			theta -= 0.00001*pi / 180;
			float newX = radius*cos(theta);
			float newY = radius*sin(theta);
			ray = lineFloat(new vector2d<float>(mX, mY), new vector2d<float>(newX, newY));
		}//point 2 degree -1
		else if (k % 6 == 5) {
			float origX = lines[k / 6].point2->getX();
			float origY = lines[k / 6].point2->getY();
			float theta = atan(origY / origX);
			float radius = sqrt(origX*origX + origY*origY);
			theta -= 0.00001* pi / 180;
			float newX = radius*cos(theta);
			float newY = radius*sin(theta);
			ray = lineFloat(new vector2d<float>(mX, mY), new vector2d<float>(newX, newY));
		}
		//calculate intersection for each edge on the map
		double r_px = ray.point1->getX();
		double r_py = ray.point1->getY();
		double r_dx = ray.point2->getX() - ray.point1->getX();
		double r_dy = ray.point2->getY() - ray.point1->getY();
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
		//for every line segment on map
		for (int i = 0; i < 4; i++)
		{
			
			//set the initial value we need to help solve our equation
			s_px = lines[i].point1->getX();
			s_py = lines[i].point1->getY();
			s_dx = lines[i].point2->getX() - lines[i].point1->getX();
			s_dy = lines[i].point2->getY() - lines[i].point1->getY();
			if ((s_dx*r_dy - s_dy*r_dx != 0)) {
				if (r_dx == 0) {
					r_dx = 0.001;
				}
				T2 = (r_dx*(s_py - r_py) + r_dy*(r_px - s_px)) / (s_dx*r_dy - s_dy*r_dx);
				T1 = (s_px + s_dx*T2 - r_px) / r_dx;
			}//lines are parallel, therefore we cannot calculate an intersection, doesn't exist, so just go to next iteration of loop
			else {
				continue;
			}
			

			

			//we intersect the segment, and not outside of its boundaries
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
		//if there were no intersections
		if (minLine == INT_MAX) {
			xPOI_final = r_dx * 1000 + r_px;
			yPOI_final = r_dy * 1000 + r_py;
		}
		glBegin(GL_LINES);
		glVertex2f(ray.point1->getX(), ray.point1->getY());
		glVertex2f(xPOI_final, yPOI_final);
		glEnd();
	}
	//cast rays for the waste screen corners
	for (int k = 0; k < 4; k++) {
		ray = lineFloat( new vector2d<float>(mX, mY), new vector2d<float>(screenVertices[k]));
		double r_px = ray.point1->getX();
		double r_py = ray.point1->getY();
		double r_dx = ray.point2->getX() - ray.point1->getX();
		double r_dy = ray.point2->getY() - ray.point1->getY();
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
		//for every line segment on map
		for (int i = 0; i < 4; i++)
		{
			//set the initial value we need to help solve our equation
			s_px = lines[i].point1->getX();
			s_py = lines[i].point1->getY();
			s_dx = lines[i].point2->getX() - lines[i].point1->getX();
			s_dy = lines[i].point2->getY() - lines[i].point1->getY();

			if ((s_dx*r_dy - s_dy*r_dx != 0) && (r_dx != 0)) {
				T2 = (r_dx*(s_py - r_py) + r_dy*(r_px - s_px)) / (s_dx*r_dy - s_dy*r_dx);
				T1 = (s_px + s_dx*T2 - r_px) / r_dx;
			}//lines are parallel, therefore we cannot calculate an intersection, doesn't exist, so just go to next iteration of loop
			else {
				continue;
			}
			//we intersect the segment, and not outside of its boundaries
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
		//if there were no intersections
		if (minLine == INT_MAX) {
			xPOI_final = r_dx * 1000 + r_px;
			yPOI_final = r_dy * 1000 + r_py;
		}
		glBegin(GL_LINES);
		glVertex2f(ray.point1->getX(), ray.point1->getY());
		glVertex2f(xPOI_final, yPOI_final);
		glEnd();
	}
}


//NEEDS TO BE COMMENTED AT SOME POINT FOR CLARITY
vector2d<float> castRay(vector2d<float> rayP1, vector2d<float> rayP2, lineFloat edges[], int numEdges) {
	lineFloat ray;
	ray = lineFloat(new vector2d<float>(rayP1.getX(), rayP1.getY()), new vector2d<float>(rayP2.getX(), rayP2.getY()));

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
		glColor3f(1.0, 1.0, 1.0);
		glVertex2f(0, 0);
		glVertex2f(0, windowHeight);
		glVertex2f(windowWidth, windowHeight);
		glVertex2f(windowWidth, 0);
	}glEnd();


	glColor3f(0.0, 0.0, 1.0);
	glLineWidth(0.2);
	
	//castRay();
	vector2d<float> vertex;
	vector2d<float> rayPoints[28];
	int rayCount = 0;
	for (int i = 0; i < 4*2*3; i++) {
		if (i % 6 == 0)
			vertex = castRay(vector2d<float>(mX, mY), *lines[i/6].point1, lines, 4);
		else if ( i % 6 == 1)
			vertex = castRay(vector2d<float>(mX, mY), *lines[i/6].point2, lines, 4);
		else if (i % 6 == 2) {
			double dx = lines[i / 6].point1->getX() - mX; double dy = lines[i / 6].point1->getY() - mY;
			double radius = sqrt((dx*dx) + (dy*dy));
			double theta = calculateAngle(*lines[i / 6].point1, vector2d<float>(mX, mY));
			theta += 0.5*pi / 180;
			double newX = mX + radius*cos(theta);
			double newY = mY - radius*sin(theta);
			vertex = castRay(vector2d<float>(mX, mY), vector2d<float>(newX, newY), lines, 4);
		}
		else if (i % 6 == 3) {
			double dx = lines[i / 6].point2->getX() - mX; double dy = lines[i / 6].point2->getY() - mY;
			double radius = sqrt((dx*dx) + (dy*dy));
			double theta = calculateAngle(*lines[i / 6].point2, vector2d<float>(mX, mY));
			theta += 0.5*pi / 180;
			double newX = mX + radius*cos(theta);
			double newY = mY - radius*sin(theta);
			vertex = castRay(vector2d<float>(mX, mY), vector2d<float>(newX, newY), lines, 4);
		}
		else if (i % 6 == 4) {
			double dx = lines[i / 6].point1->getX() - mX; double dy = lines[i / 6].point1->getY() - mY;
			double radius = sqrt((dx*dx) + (dy*dy));
			double theta = calculateAngle(*lines[i / 6].point1, vector2d<float>(mX, mY));
			theta -= 0.5*pi / 180;
			double newX = mX + radius*cos(theta);
			double newY = mY - radius*sin(theta);
			vertex = castRay(vector2d<float>(mX, mY), vector2d<float>(newX, newY), lines, 4);
		}
		else if (i % 6 == 5) {
			double dx = lines[i / 6].point2->getX() - mX; double dy = lines[i / 6].point2->getY() - mY;
			double radius = sqrt((dx*dx) + (dy*dy));
			double theta = calculateAngle(*lines[i / 6].point2, vector2d<float>(mX, mY));
			theta -= 0.5*pi / 180;
			double newX = mX + radius*cos(theta);
			double newY = mY - radius*sin(theta);
			vertex = castRay(vector2d<float>(mX, mY), vector2d<float>(newX, newY), lines, 4);
		}
		rayPoints[rayCount].setX(vertex.getX());
		rayPoints[rayCount].setY(vertex.getY());
		rayCount++;
	}

	rayPoints[24] = castRay(vector2d<float>(mX, mY), vector2d<float>(0, 0), lines, 4);
	rayPoints[25] = castRay(vector2d<float>(mX, mY), vector2d<float>(1600, 0), lines, 4);
	rayPoints[26] = castRay(vector2d<float>(mX, mY), vector2d<float>(0, 900), lines, 4);
	rayPoints[27] = castRay(vector2d<float>(mX, mY), vector2d<float>(1600, 900), lines, 4);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(1600, 0);
	glVertex2f(1600, 900);
	glVertex2f(0, 900);
	glEnd();

	
	//draws the vision polygon on the screen as multiple triangles
	sortByAngle(rayPoints, vector2d<float>(mX, mY), 28);
	glColor3f(0.9, 0.8, 0.01);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 28; i++) {
		glVertex2f(mX, mY);
		glVertex2f(rayPoints[i%28].getX(), rayPoints[i%28].getY());
		glVertex2f(rayPoints[(i + 1)%28].getX(), rayPoints[(i + 1) % 28].getY());
	}
	glEnd();
	
	//draws the rays
	glColor3f(0.5, 0.5, 0.1);
	for (int i = 0; i < 28; i++) {
		glBegin(GL_LINES);
		glVertex2f(rayPoints[i].getX(), rayPoints[i].getY());
		glVertex2f(mX, mY);
		glEnd();
	}

	//draw the circle where the mouse is
	drawCircle(15, mX, mY, 5);
	//draw every single line
	//when drawing the lines use the lines[] array
	glColor3f(0.5, 0.5, 0.5);
	glLineWidth(10);
	glBegin(GL_LINES);
	for (int i = 0; i < (sizeof(lines) / sizeof(lines[0])); i++) {
		glVertex2f(lines[i].point1->getX(), lines[i].point1->getY());
		glVertex2f(lines[i].point2->getX(), lines[i].point2->getY());
	}
	glEnd();

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

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow(title);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(mouseUpdate);
	glutTimerFunc(0, Timer, 0);
	initGL();
	glutMainLoop();
	return 0;
}