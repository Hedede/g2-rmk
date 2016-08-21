template <typename T>
class zCArrayAdapt {
public:
	int SetArray(void* array, int count)
	{
		this->array = array;
		this->count = count;
		return sizeof(T) * count;
	}
private:
	char* array;
	int count;
};

template <typename T>
void SetAndCopyArrayAdapt(u8*& bytes, zCArrayAdapt<T>& adapt, zCArray<T>& array)
{
	int count = array.GetNumInList()
	adapt.SetArray(bytes, count)

	memcpy(adapt->array, array->array, sizeof(T) * count);
}
