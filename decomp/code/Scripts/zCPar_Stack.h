class zCPar_Stack {
public:
	int GetDynSize()
	{
		return stacklast - stack;
	}

	int GetPopPos()
	{
		return stackptr - stack;
	}

	void Clear()
	{
		stacklast = stack;
	}

	int ReachedEnd()
	{
		return stackptr >= stacklast;
	}

	int GetStatSize()
	{
		return stacksize;
	}

	void PushString(zSTRING& str);

private:
	zBYTE* stack;
	zBYTE* stackptr;
	zBYTE* stacklast;
	int stacksize;
};

class zCPar_DataStack {
public:
	zCPar_DataStack();
	~zCPar_DataStack = default;

	int IsEmpty()
	{
		return sptr == 0;
	}
private:
	int stack[2048];
	int sptr;
};
