#pragma once


#include "node.h"

template <typename T>
class linkedList {
public:
	linkedList();//empty list constructor
	void addFirst(T t);//add to beginning of list
	node<T>* getHead();
private:
	node<T> *head;
	int size;
};

template <typename T>
linkedList<T>::linkedList() {
	this->size = 0;
	this->head = new node<T>();
}

template <typename T>
void linkedList<T>::addFirst(T t) {
	node<T>* p = new node<T>(t, head->next);
	head->next = p;
	size++;
}

template <typename T>
node<T>* linkedList<T>::getHead() {
	return this->head;
}