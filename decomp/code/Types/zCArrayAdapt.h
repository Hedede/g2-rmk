template <typename T>
struct zCArrayAdapt {
	int SetArray(void* array, int count)
	{
		this->array = array;
		this->count = count;
		return GetSizeInBytes();
	}

	T Get(unsigned idx)
	{
		return array[idx];
	}

	int GetSizeInBytes() const
	{
		return count * sizeof(T);
	}

private:
	T* array;
	int count;
};

template <typename T>
void SetAndCopyArrayAdapt(u8*& bytes, zCArrayAdapt<T>& adapt, zCArray<T>& array)
{
	int count = array.GetNumInList()
	adapt.SetArray(bytes, count)

	memcpy(adapt->array, array->array, sizeof(T) * count);
}
