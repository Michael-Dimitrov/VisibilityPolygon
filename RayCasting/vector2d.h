#pragma once

template <typename T = double>

class vector2d {
private:
	T x;
	T y;
public:
	vector2d<T>();
	vector2d<T>(T x, T y);
	T getX();
	T getY();
	void setX(T newVar);
	void setY(T newVar);
	vector2d<T> add(vector2d<T> vector);
	vector2d<T> sub(vector2d<T> vector);
	T dot(vector2d<T> vector);
	vector2d<T> scaleSelf(double scale);

};

template<typename T>
vector2d<T>::vector2d() {
	x = 0;
	y = 0;
}

template<typename T>
vector2d<T>::vector2d(T x, T y) {
	this->x = x;
	this->y = y;
}

template<typename T>
T vector2d<T>::getX() {
	return this->x;
}

template<typename T>
T vector2d<T>::getY() {
	return this->y;
}

template<typename T>
void vector2d<T>::setX(T newVar) {
	this->x = newVar;
}

template<typename T>
void vector2d<T>::setY(T newVar) {
	this->y = newVar;
}


//add the two vectors together
template <typename T>
vector2d<T> vector2d<T>::add(vector2d<T> vector) {
	T addX, addY;
	addX = vector.getX() + this->x;
	addY = vector.getY() + this->y;
	return vector2d(addX, addY);
}


//subtract vector from this vector
template <typename T>
vector2d<T> vector2d<T>::sub(vector2d<T> vector) {
	T subX, subY;
	subX = this->x - vector.getX();
	subY = this->y - vector.getY();
	return vector2d(subX, subY);
}

//compute the dot product of 2 2-d vectors
template <typename T>
T vector2d<T>::dot(vector2d<T> vector) {
	T dotX, dotY;
	dotX = vector.getX() * this->x;
	dotY = vector.getY() * this->y;
	return dotX + dotY;
}

template <typename T>
vector2d<T> vector2d<T>::scaleSelf(double scale) {
	return vector2d(this->x*scale, this->y*scale);
}