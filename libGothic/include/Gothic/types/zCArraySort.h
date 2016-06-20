#ifndef G2_zCArraySort
#define G2_zCArraySort
template <class T>
int zArraySortDefaultCompare(T const* ele1, T const* ele2)
{
	ptrdiff_t e1 = *(ptrdiff_t *)ele1;
	ptrdiff_t e2 = *(ptrdiff_t *)ele2;

	if (e2 <= e1)
		return e2 < e1;

	return -1;
}

template <class T> 
class zCArraySort {
public:
	zCArraySort()
		: array(nullptr), numAlloc(0), numInArray(0),
		Compare(zArraySortDefaultCompare<T>)
	{
	}

	~zCArraySort()
	{
		delete[] array;
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
		if (size > numAlloc)
			AllocDelta(size - numAlloc);
	}

	T& GetSafe(int index)
	{
		if (numInArray < 0)
			zFATAL("D: zTree(zCArraySort::GetSafe): numInArray<=0");

		if (numAlloc <= 0 )
			zFATAL("D: zTree(zCArraySort::GetSafe): numAlloc<=0");

		if (index < 0)
			index = 0;

		if (index > numInArray)
			index = numInArray - 1;

		return array[index];
	}

	int Search(T const& val, int begin, int end)
	{
		int num = numInArray;
		if ( num <= 0 )
			return -1;

		while ( begin <= end ) {
			int mid = (begin + end) / 2;
			int cmp = Compare(val, array[mid]);

			if (!cmp)
				return mid;

			if (cmp > 0)
				begin = mid + 1;

			if (cmp < 0)
				end = mid - 1;
		}

		return -1;
	}

	int Search(T const& val)
	{
		return Search(val, 0, numInArray);
	}

	bool IsInList(T const& val)
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

	int InsertEnd(T& val)
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

	int InsertSort(T const& val)
	{
		if (numAlloc < numInArray + 1) {
			if (numAlloc >= 16)
				AllocDelta(numAlloc / 2);
			else
				AllocDelta(numAlloc, 16);
		}

		int idx = Search(val);

		if ( idx < numInArray )
		{
			if ( Compare(val, &array[idx]) > 0 )
				++idx;

			memmove(array[idx + 1], array[idx], sizeof(T) * (numInArray - idx));
		}

		array[idx] = val;
		return numInArray++;
	}

	int Insert(T const& val)
	{
		return InsertEnd(val);
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

	void QuickSort()
	{
		//qsort(array, numInArray, sizeof(T), Compare);
	}

private:
	T* array;             //Zeiger auf Speicherbereich
	int numAlloc;          //Für wieviele Elemente ist gerade Speicher reserviert?
	int numInArray;         //Anzahl der Elemente in diesem Speicherbereich
	int (*Compare)(T const& ele1, T const& ele2);
};
#endif//G2_zCArraySort
