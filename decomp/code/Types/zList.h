// _ulf/zList.h
// Intrusive list
template <class T>
struct zList {
	void DeleteList()
	{
		while (auto cur = root) {
			root = root->next;
			delete cur;
		}
	}

	int IsIn(T* val)
	{
		for (auto ele = root; ele; ele = ele->next) {
			if (ele == val)
				return 1;
		}
		return 0;
	}

	void Insert(T* val)
	{
		if (!root) {
			root = val;
			return;
		}

		if (!IsIn(val)) {
			++count;
			last->next = val;
			val->next  = 0;
			last = val;
		}
	}

	void InsertLast(T* val)
	{
		Insert(val);
	}

	void Remove(T* val)
	{
		if ( !root )
			return;


		if ( root == val ) {
			root = root->next;
			if (!root)
				last = 0;

			if ( --count < 0 ) {
				zFAULT("U: LIST: Count is less than 0"); // 216
				return;
			}
		} else {
			T* node = root;
			T* next = root->next;
			while ( next != val ) {
				if (node == next)
					node->next = 0;
				else
					node = next;
				next = node->next;
				if (!next)
					return;
			}

			if ( !val->next )
				last = node;
			node->next = val->next;

			if ( --count < 0 ) {
				zFAULT("U: LIST: Count is less than 0"); // 224
				return;
			}
		}
	}

private:
	int (*Compare)(const T* ele1,const T* ele2);
	int count;    //Anzahl Elemente
	T* last;
	T* wurzel;
};
