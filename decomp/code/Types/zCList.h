template <class T>
class zCList {
public:
	~zCList()
	{
		DeleteList();
	}

	void DeleteList()
	{
		if ( next ) {
			delete next;
			s_ListPool.Free(next);
			next = 0;
		}
	}

	void DeleteListDatas()
	{
		if ( data ) {
			delete data;
			data = 0;
		}
		if ( next ) {
			next->DeleteListDatas(next);
			DeleteList();
		}
	}
	zCList<T>* Next()
	{
		return next;
	}

	zCList<T> const* Next() const
	{
		return next;
	}

	zCList* InsertFront(T* val)
	{
		zCList *result = s_ListPool.Alloc();
		result->data = val;
		result->next = next;
		next = result;
		return result;
	}

	zCList<T>* Insert(T* val)
	{
		return InsertFront(val);
	}

	void Remove(T const* val)
	{
		if (!next)
			return;

		auto* node = this;
		auto* next = node->next;
		while (next) {
			if (next->data == val) {
				node->next = next->next;
				delete next;

				break;
			}

			node = next;
			next = node->next;
		}

	}

	int zCList<T>::GetNumInList() const
	{
		int i = 0;
		for (auto list : *this)
			++i;

		return i;
	}

	T* Get(size_t index = 0)
	{
		size_t i = 0;
		for (auto list : *this)
			if (i++ == index)
				return *list;
		return 0;
	}

	T const* Get(size_t index = 0) const
	{
		size_t i = 0;
		for (auto list : *this)
			if (i++ == index)
				return *list;
		return 0;
	}

	bool zCList<T>::IsInList(T const* val) const
	{
		for (auto list : *this)
			if (*list == val)
				return true;
		return false;
	}

private:
     T* data;
     zCList<T>* next;
};

// my addition
template<typename T>
struct zCListIterator {
	zCListIterator(zCList<T>* list)
		: list(list)
	{
	}

	T* operator*()
	{
		return list->Get();
	}

	zCList<T>* operator->()
	{
		return list;
	}

	zCList<T>* operator++()
	{
		return list = list->Next();
	}

private:
	friend zCList<T>;
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

template<typename T>
struct zCListConstIterator {
	zCListConstIterator(zCList<T> const* list)
		: list(list)
	{
	}

	T const* operator*()
	{
		return list->Get();
	}

	zCList<T> const* operator->()
	{
		return list;
	}

	zCList<T> const* operator++()
	{
		return list = list->Next();
	}

private:
	friend zCList<T>;
	zCList<T> const* list;
};

template<typename T>
zCListConstIterator begin(zCList<T> const& list)
{
	return list->Next();
}

template<typename T>
zCListConstIterator end(zCList<T> const& list)
{
	return nullptr;
}
