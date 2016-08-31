// Intrusive list
template <class T>
struct zList {
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

private:
	int (*Compare)(const T* ele1,const T* ele2);
	int count;              //Anzahl Elemente
	T* last;               //T*
	T* wurzel;             //T*
};
