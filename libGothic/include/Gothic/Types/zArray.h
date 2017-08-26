#ifndef Gothic_zArray_H
#define Gothic_zArray_H
/* zCArray ist eine sehr einfache und gleichzeitig sehr wichtige Datenstruktur. */
template <class T>
struct zCArray {
	zCArray() = default;

	//Zeiger auf Speicherbereich
	T* array = nullptr;
	//Für wieviele Elemente ist gerade Speicher reserviert?
	int numAlloc = 0;
	//Anzahl der Elemente in diesem Speicherbereich
	int numInArray = 0;
};

template <class T>
struct zCArraySort : zCArray<T> {
	zCArraySort() = default;

	using CompareFunc = int(T const&,T const&);
	CompareFunc *Compare;
};

template <class T>
T* begin(zCArray<T>& array) { return array.array; }
template <class T>
T* end(zCArray<T>& array) { return array.array + array.numInArray; }
#endif//Gothic_zArray_H
