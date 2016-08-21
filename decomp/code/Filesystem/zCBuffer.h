class zCBuffer {
public:
	zCBuffer& operator=(zCBuffer const& other)
	{
		if ( this != &other )
			CopyFrom(other);
		return *this;
	}

	void SetMode(zTBufferMode mode)
	{
		this->mode = mode;
	}

	char* GetBuffer()
	{
		return this->buffer;
	}

	int GetPos()
	{
		return pos;
	}

	int GetMaxSize()
	{
		return maxSize;
	}

	int GetRestSize()
	{
		return this->maxSize - this->pos;
	}

	int GetSize()
	{
		return size;
	}

	char* GetCursor()
	{
		return &buffer[pos];
	}

	bool HasError()
	{
		return hasError;
	}
private:
	zTBufferMode mode;
	char* buffer;
	int pos;
	int maxSize;
	int size;
	bool hasError;
};
