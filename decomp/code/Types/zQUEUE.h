// This is what appears to be a circular queue
template<typename T>
struct zQUEUE_ITEM {
	zQUEUE_ITEM() = default;

	int free = 1;
	T* data = nullptr;
	zQUEUE_ITEM* next = 0;
	zQUEUE_ITEM* prev = 0;
}

template<typename T>
struct zQUEUE {
	zQUEUE()
	{
		start = new zQUEUE_ITEM<T>(); // _Bert, zqueue.h, 144
		start->prev = start;
		start->next = start;
		front = start;
	}

	bool Free() const
	{
		return start->free;
	}

	size_t Push(T* xData);
	T* Pop();

	size_t numInQueue = 0;
	zQUEUE_ITEM<T>* start;
	zQUEUE_ITEM<T>* front;
}

template<typename T>
size_t zQUEUE<T>::Push(T* xData)
{
	if ( front->next == start ) {
		for (int i = 0; i != 10; ++i) {
			auto item = new zQUEUE_ITEM<T>(); // zqueue.h, 192

			if ( front ) {
				auto next = front->next;
				if ( next )
					next->prev = item;

				item->next = next;
				item->prev = front;

				front->next = item;
			} else {
				item->next = 0;
				item->prev = 0;
			}

			front = item;
		}
	}

	if ( !front->free )
		front = front->next;

	front->free = 0;
	front->data = xData;

	return ++numInQueue;
}

template<typename T>
T* zQUEUE::Pop()
{
	if ( front->free )
		return nullptr;

	T* result = front->data;

	front->free = 1;
	front->data = 0;

	--numInQueue;

	if ( front != start )
		front = front->prev;
}
