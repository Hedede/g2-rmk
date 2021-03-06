#ifndef G2_zCArray
#define G2_zCArray
/* zCArray ist eine sehr einfache und gleichzeitig sehr wichtige Datenstruktur. */
template <class T>
struct zCArray {
	zCArray() = default;
	zCArray(int size)
		: numAlloc(size)
	{
		array = new T[size];
	}

	~zCArray()
	{
		delete[] array;
	}

	zCArray(zCArray<T> const& other)
	{
		int num = other->numInArray;
		AllocAbs(num);

		numInArray = num;
		for (int i = 0; i < num; ++i )
			array[i] = other->array[i];
	}

	zCArray<T>& operator=(zCArray<T> const& other)
	{
		int num = other->numInArray;
		AllocAbs(num);

		numInArray = num;
		for (int i = 0; i < num; ++i )
			array[i] = other->array[i];

		return *this;
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


	T& GetSafe(int index)
	{
		if ( numInArray < 0 )
			zFATAL("D: zTree(zCArray::GetSafe): numInArray  <=0");

		if ( numAlloc <= 0 )
			zFATAL("D: zTree(zCArray::GetSafe): numAlloc  <=0");

		if (index < 0)
			index = 0;

		if (index > numInArray)
			index = numInArray - 1;

		return array[index];
	}

	int Search(T const& val) const
	{
		for (int i = 0; i < numInArray; ++i)
			if (array[i] == val)
				return i;

		return -1;
	}

	bool IsInList(T const& val) const
	{
		return Search(val) != -1;
	}

	int GetNumInList() const
	{
		return numInArray;
	}

	int GetNum() const
	{
		return GetNumInList();
	}

	void Resize(int newSize)
	{
		AllocAbs(newSize);
		for (int i = newSize; i < numInArray; ++i)
			array[i] = {};
		numInArray = newSize;
	}

	void ShrinkToFit()
	{
		if (numAlloc > numInArray) {
			T* new_alloc = new T[numInArray];

			for (int i = 0; i < num; ++i)
				new_alloc[i] = array[i];
			delete[] array;

			array = new_alloc;
		} else {
			DeleteList();
		}
	}

	int InsertEnd(T const& val)
	{
		if (numAlloc < numInArray + 1) {
			if (numAlloc >= 16)
				AllocDelta(numAlloc / 2);
			else
				AllocDelta(numAlloc, 16);
		}
		array[numInArray++] = val;
		return numInArray;
	}

	int InsertAtPos(T const& val, int idx)
	{
		if (numAlloc < numInArray + 1) {
			if (numAlloc >= 16)
				AllocDelta(numAlloc / 2);
			else
				AllocDelta(numAlloc, 16);
		}

		memmove(array[idx + 1], array[idx], sizeof(T) * (numInArray - idx));
		array[idx] = val;
		return numInArray++;
	}

	int InsertFront(T const& val)
	{
		return InsertAtPos(val, 0);
	}

	int Insert(T const& val)
	{
		return InsertEnd(val);
	}

	void RemoveIndex(int index)
	{
		if (numInArray <= 0)
			return;

		--numInArray;
		if (index != numInArray)
			array[index] = array[numInArray];
	}

	void Remove(T const& val)
	{
		int index = Search(val);
		if (index > 0)
			RemoveIndex(index);
	}

	void RemoveOrderIndex(int index)
	{
		if (index >= numInArray)
			return;

		for (int i = index; i < numInArray - 1; ++i)
		{
			array[i] = array[i + 1];
		}

		--this->numInArray;
	}

	void RemoveOrder(T const& val)
	{
		int index = Search(val);
		if (index > 0)
			RemoveOrderIndex(index);
	}


	void DeleteList()
	{
		delete[] this->array;

		array = 0;
		numAlloc = 0;
		numInArray = 0;
	}

	T& operator[](int idx)
	{
		return array[idx];
	}
private:
	//Zeiger auf Speicherbereich
	T* array = nullptr;
	//F??r wieviele Elemente ist gerade Speicher reserviert?
	int numAlloc = 0;
	//Anzahl der Elemente in diesem Speicherbereich
	int numInArray = 0;
};



template <typename T>
// int CalcArraySize(zCArray<T> array) [sic!]
// in original code THEY PASS COPY
int CalcArraySize(zCArray<T> const& array)
{
	return sizeof(T) * array.GetNumInList();
}

// my addition
template <typename C>
int Count(C const& array)
{
	return array.GetNumInList();
}
#endif//G2_zCArray
