template <class T>
class zList {
public:
	int zList::IsIn(T *val)
	{
		for (auto ele = root; ele; ele = ele->next) {
			if (ele == val)
				return 1;
		}
		return 0;
	}

private:
	int (*Compare)(const T* ele1,const T* ele2);
	int count;              //Anzahl Elemente
	T* last;               //T*
	T* wurzel;             //T*
};
