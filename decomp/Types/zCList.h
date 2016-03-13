template <class T>
class zCList {
public:
	~zCList()
	{
		DeleteList();
	}

	T& Get(size_t index = 0);

	zCList<T>* Next()
	{
		return next;
	}

	zCList<T>* Insert(T* val)
	{
		return InsertFront(val);
	}
private:
     T* data;
     zCList<T>* next;
};

int zCList<T>::GetNumInList()
{
	int i = 0;
	for (zCList<T>* node = next; node; ++i)
		node = node->next;

	return i;
}

T& zCList<T>::Get(size_t index)
{
	size_t i = 0;
	for (auto* node = next; node; node = node->next)
		if (i++ == index)
			return node->data;
	return 0;
}

bool zCList<T>::IsInList(T* val)
{
	for (auto* node = next; node; node = node->next)
		if (node->data == val)
			return true;
	return false;
}

zCList* zCList<T>::InsertFront(T* val)
{
	zCList *result = s_ListPool.Alloc();
	result->data = val;
	result->next = next;
	next = result;
	return result;
}


void zCList<T>::Remove(T* val)
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

void zCList<T>::DeleteList()
{
	if ( next ) {
		delete next;
		s_ListPool.Free(next);
		next = 0;
	}
}

void zCList<T>::DeleteListDatas()
{
	if ( data )
	{
		delete data;
		data = 0;
	}
	if ( next )
	{
		next->DeleteListDatas(next);
		DeleteList();
	}
}

// my addition
template<typename T>
struct zCListIterator {
	zCListIterator(zCList<T>* list)
		: list(list)
	{
	}

	zCList<T>* operator->()
	{
		return list;
	}

	zCList<T>* operator++()
	{
		list = list->Next();
	}

	zCList<T>* list;
};

template<typename T>
zCListIterator begin(zCList<T>& list)
{
	return list->Next();
}

template<typename T>
zCListIterator end(zCList<T>& list)
{
	return nullptr;
}
