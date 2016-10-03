#pragma once

template <typename T>
class node {
public:
	T data;
	node* next;
	node(T t, node* next);
	node();
};

template <typename T>
node<T>::node(T t, node* next) {
	this->data = t;
	this->next = next;
}

template <typename T>
node<T>::node() {

}