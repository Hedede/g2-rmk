#ifndef Gothic_zArray_H
#define Gothic_zArray_H
/* zCArray ist eine sehr einfache und gleichzeitig sehr wichtige Datenstruktur. */
template <class T>
struct zCArray {
	zCArray() = default;

	zCArray& operator=(zCArray const& other)
	{
		int num = other.numInArray;
		AllocAbs(num);

		numInArray = num;
		for (int i = 0; i < num; ++i )
			array[i] = other[i];
	}

	void DeleteList()
	{
		delete[] this->array;

		array = 0;
		numAlloc = 0;
		numInArray = 0;
	}
	
	void AllocDelta(int delta)
	{
		if ( delta < 0 )
			return;

		T* new_alloc = new T[delta + this->numAlloc];

		int num = this->numInArray;
		for (int i = 0; i < num; ++i)
			new_alloc[i] = this->array[i];

		delete[] this->array;

		this->array = new_alloc;
		this->numAlloc += delta;
	}

	void AllocAbs(int size)
	{
		if ( numAlloc < size )
			AllocDelta(size - numAlloc);
	}

	T& operator[] (int i) { return array[i]; }
	T const& operator[] (int i) const { return array[i]; }


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
