template <class T> 
class zCListSort {
public:
	zCListSort<T>* InsertSort(T const& elem);
private:
	int (*Compare)(T *ele1,T *ele2);
	T* data;
	zCListSort<T>* next;
};

zCListSort<T>::~zCListSort<T>()
{
	if ( next ) {
		delete next;
		s_ListSortPool.Free(next);
	}
}

int zCListSort<T>::GetNumInList()
{
	int i = 0;
	for (zCList<T>* node = next; node; ++i)
		node = node->next;

	return i;
}

T& zCListSort<T>::Get(size_t index)
{
	size_t i = 0;
	for (auto* node = next; node; node = node->next)
		if (i++ == index)
			return node->data;
	return 0;
}

bool zCListSort<T>::IsInList(T* val)
{
	for (auto* node = next; node; node = node->next)
		if (node->data == val)
			return true;
	return false;
}

zCListSort<T>* zCListSort<T>::InsertSort(T *ele)
{
	auto* alloc = s_ListSortPool.Alloc();

	alloc->data = ele;

	auto* node = this;
	auto* next = node->next;
	while (next)
	{
		if (Compare(ele, next->data) < 0)
			break;

		node = next;
		next = node->next;
	}

	alloc->next = next;
	node->next = alloc;
	
	return alloc;
}

zCListSort* zCListSort<T>::InsertFront(T* val)
{
	zCList *result = s_ListSortPool.Alloc();
	result->data = val;
	result->next = next;
	next = result;
	return result;
}

zCListSort* zCListSort<T>::Insert(T* val)
{
	return InsertFront(val);
}

void zCListSort<T>::Remove(T* val)
{
	if (!next)
		return;


	auto* node = this;
	auto* next = node->next;
	while (next)
	{
		if (node->data == val)
			break;

		node = next;
		next = node->next;
	}

	node->next = next->next;

	delete next;
}
