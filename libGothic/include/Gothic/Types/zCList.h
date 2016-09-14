#pragma once

template <class T>
struct zCList {

private:
	T* data;
	zCList<T>* next;
};

template <class T>
struct zList {

private:
	int (*Compare)(T const*, T const*);
	size_t count;
	T* last;
	T* root;
};
