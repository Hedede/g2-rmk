class zCSparseArrayManDat {
public:
	zCSparseArrayManDat() = default;

	// Those methods are my addition, they were'nt present in original
	bool IsOccupied()
	{
		return info & 0x80000000;
	}

	bool IsFree()
	{
		return !Occupied();
	}

	void Occupy()
	{
		info |= 0x80000000;
	}
	
	void Free()
	{
		info &= 0x7FFFFFFF;
	}

	uint32_t NumRefs()
	{
		return info & 0x7FFFFFFF;
	}

	void AddRef()
	{
		uint32_t numRefs = NumRefs() + 1;
		info ^= (info & 0x7FFFFFFF) ^ (numRefs & 0x7FFFFFFF);
	}

	void Release()
	{
		uint32_t numRefs = NumRefs() - 1;
		info ^= (info & 0x7FFFFFFF) ^ (numRefs & 0x7FFFFFFF);
	}
private:
	uint32_t info = 0;
};


class zCSparseArrayBase {
public:
	zCSparseArrayBase::zCSparseArrayBase(uint32_t num)
		: numEntries_specified(num);
	{
		if ( num )
			Create(num);
	}

	virtual ~zCSparseArrayBase()
	{
		Destroy();
	}

	virtual size_t SizeIndex() const = 0;
	virtual size_t SizeData() const = 0;
	virtual size_t SizeAll() const
	{
		return SizeIndex() + SizeData() + sizeof(zCSparseArrayManDat);
	}

	virtual void* IndexPtr(ulong idx)
	{
		return data[idx * SizeAll()];
	}
	virtual void* DataPtr(ulong idx)
	{
		return IndexPtr(idx) + SizeIndex();
	}
	void* DataPtr(ulong idx, void* ptr)
	{
		return ptr + DataPtr(idx);
	}
	virtual void* ManDatPtr(ulong idx)
	{
		return DataPtr(idx) + SizeData();
	}

	void Hash(const void* idx, uint32_t& lo, uint32_t& hi) const
	{
		::Hash(idx, numEntries_actual, lo, hi, SizeIndex());
	}

	void* Element(zCSparseArrayBase *this, const void *index)
	{
		CheckResize();
		auto ele = Find(index);
		if ( ele < numEntries )
			return DataPtr(ele);
		return nullptr;
	}

	void* Insert(void const* index);
	void Destroy();
	void Resize(size_t newSize);
	uint32_t Find(const void *idx);
	void Create(uint32_t count);
	void Delete(const void *idx);

protected:
	void CheckResize()
	{
		if ( shouldResize && !numberOfIterators ) {
			size_t size = 3 * numRefs;
			if ( size < numEntries_specified )
				size = numEntries_specified;
			Resize(size);
		}
	}

	static void Release(zCSparseArrayManDat* dat)
	{
		dat->Release();

		if ( dat->NumRefs() == 0 ) {
			--numRefs;

			if ( 2 * (3 * numRefs + 6) < numEntries && numEntries > numEntries_specified )
				shouldResize = 1;
		}
	}

	static void AddRef(zCSparseArrayManDat* dat)
	{
		if ( dat->NumRefs() == 0) {
			++numRefs;
			if ( 3 * numRefs + 6 > 2 * numEntries )
				shouldResize = 1;
		}

		if ( dat->NumRefs() > unk ) {
			shouldResize = 1;
			unk += unk >> 1;
		}

		dat->AddRef();
	}

	void zCSparseArrayBase::Clear()
	{
		while ( numRefs ) {
			for (size_t i = 0; i < numEntries; ++i ) {
				if ( ManDatPtr(i)->Occupied() )
					Delete(IndexPtr(i));
			}
		}
	}

	virtual bool IndexIsEqual(void const* idx1, void const* idx2);
	{
		memcmp(idx1, idx2, SizeIndex()) == 0;
	}
	virtual void CopyIndex(void* dst, void const* src)
	{
		memcpy(dst, src, SizeIndex());
	}
	virtual void DestructIndex(void *)
	{
	}

	virtual void ConstructData(void *)
	{
	}

	virtual void DestructData(void *)
	{
	}

	virtual void CopyData(void* dst, void const* src)
	{
		memcpy(dst, src, SizeData());	
	}

private:
	uint32_t numberOfIterators = 0;
	uint32_t numEntries = 0;
	uint32_t dataPtr = 0;
	uint32_t numRefs = 0;
	uint32_t unk = 10;
	bool shouldResize = false;
};

template<typename Index, typename Data>
class zCSparseArray : public zCSparseArrayBase {
public:
	zCSparseArray(uint32_t num)
		: zCSparseArrayBase(num)
	{
	}

	virtual ~zCSparseArray() = default;

	virtual size_t SizeIndex() const
	{
		return sizeof(Index);
	}

	virtual size_t SizeData() const
	{
		return sizeof(Data);
	}

	virtual size_t SizeAll() const
	{
		return sizeof(Index) + sizeof(Data) + sizeof(void*);
	}

	virtual void IndexPtr(ulong idx)
	{
		return data[idx * SizeAll()];
	}
	virtual void DataPtr(ulong idx)
	{
		return IndexPtr(idx) + SizeIndex();
	}
	virtual void ManDatPtr(ulong idx)
	{
		return DataPtr(idx) + SizeData();
	}

	virtual bool IndexIsEqual(void const* idx1, void const* idx2)
	{
		Index* a = static_cast<Index*>(idx1);
		Index* b = static_cast<Index*>(idx2);
		return *a == *b;
	}
	virtual void CopyIndex(void* dst, void const* src)
	{
		Index* a = static_cast<Index*>(dst);
		Index* b = static_cast<Index*>(src);
		*a = *b;
	}

	virtual void DestructIndex(void* index) const
	{
		static_cast<Index*>(index)->->~Index();
	}

	virtual void Hash(void const*,ulong&,ulong&);
	virtual void ConstructData(void* data)
	{
		new (data) Data;
	}

	virtual void DestructData(void* data) const
	{
		static_cast<Data*>(data)->~Data();
	}

	virtual void CopyData(void* dst, void const* src)
	{
		Data* a = static_cast<Data*>(dst);
		Data* b = static_cast<Data*>(src);
		*a = *b;
	}
};

void* zCSparseArrayBase::Insert(void const* index)
{
	uint32_t found = zCSparseArrayBase::Find(index);

	if ( found < numEntries_actual )
		return DataPtr(found);

	if ( numberOfIterators > 0 )
		zFAULT_ONCE("Inserted in zCSparseArray with active Iterators; dangerous! No resize possible.");
	else
		CheckResize();

	uint32_t lo;
	uint32_t hi;
	Hash(index, lo, hi);

	for ( ManDatPtr(lo)->IsOccupied(); lo = (lo + hi) % numEntries)
		AddRef(ManDatPtr(lo));

	AddRef(ManDatPtr(lo));

	ManDatPtr(lo)->Occupy();

	CopyIndex(IndexPtr(lo), index);

	ConstructData(DataPtr(lo));
	return DataPtr(lo);
}


void zCSparseArrayBase::Destroy()
{
	zASSERT(numberOfIterators == 0, "No iterators may be active on a deleted SparseArray");
	if ( dataPtr ) {
		for (i = 0; i < numEntries_actual; ++i) {
			if ( ManDatPtr(i)->IsFree() ) {
				DestructIndex(IndexPtr(i));
				DestructData(DataPtr(i));
			}
		}

		delete[] dataPtr;
		dataPtr = 0;
	}
}


void zCSparseArrayBase::Resize(size_t newSize)
{
	if ( newSize <= this->numRefs + 10 )
		newSize = this->numRefs + 10;

	if ( numberOfIterators > 0 )
		zFATAL_ONCE("zCSparseArray resized, but there were Iterators active.");

	uint32_t prevPrime = numEntries_actual;
	uint32_t nextPrime = zCPrime::NextPrime(newSize);
	if ( nextPrime == prevPrime )
		nextPrime = zCPrime::NextPrime(nextPrime + 1);

	numEntries_actual = zCPrime::NextPrime(nextPrime);


	char* oldMem = dataPtr;

	numRefs = 0;
	size_t size = numEntries_actual * SizeAll();
	dataPtr = new char [size]();

	for (size_t i = 0; i < numEntries_actual; ++i)
		new (ManDatPtr(i)) zCSparseArrayManDat();

	shouldResize = 0;
	zINFO(9, "M: Resizing SparseArray from " + zSTRING(prevPrime) + " to " + zSTRING(numEntries) + " entries.");

	for (size_t i = 0; i < prevPrime; ++i) {
		void* data = DataPtr(i, oldMem);

		if ( *data[SizeData()] < 0 ) {
			auto idx = oldMem[SizeAll() * i]

			auto newdata = Insert(idx);
			DestructIndex(idx);

			memcpy(newdata, idx + SizeIndex(), SizeData());
		}
	}

	delete[] oldMem;
}


uint32_t zCSparseArrayBase::Find(const void *idx)
{
	uint32_t lo, hi;

	Hash(idx, &lo, &hi);

	uint32_t lastFree = numEntrues;
	while (ManDatPtr(lo)->NumRefs() > 0) {
		if ( ManDatPtr(lo)->IsFree() ) {
			lastFree = lo;
		} else if ( IndexIsEqual(IndexPtr(lo), idx) )
				break;
		}

		lo = (lo + hi) % numEntries;
	}

	uint32_t result = NumEntries;
	if (ManDatPtr(lo)->NumRefs() > 0 ) {
		if ( lastFree < result ) {
			uint32_t oldindex = lo;
			for (; lo != idxa; lo = (lo + numEntries - hi) % numEntries )
				Release(ManDatPtr(lo));

			CopyIndex(IndexPtr(lo), IndexPtr(oldindex));
			CopyData(DataPtr(lo), DataPtr(oldindex);

			ManDatPtr(lo)->Occupy();
			ManDatPtr(oldindex)->Free();
		}
		result = lo;
	}
	return result;
}


void zCSparseArrayBase::Create(uint32_t count)
{
	uint32_t prime = zCPrime::NextPrime(count);

	numEntries = prime;
	numRefs = 0;

	size_t size = numEntries * SizeAll();
	dataPtr = new char[size]();

	for (size_t i = 0; i < numEntries_actual; ++i)
		new (ManDatPtr(i)) zCSparseArrayManDat;

	shouldResize = 0;
}

void zCSparseArrayBase::Delete(const void *idx)
{
	uint32_t ele = Find(idx);
	if ( ele > numEntries )
		return;

	uint32_t lo;
	uint32_t hi;

	Hash(idx, &lo, &hi);

	ManDatPtr(lo)->Free();

	DestructIndex(IndexPtr(lo));
	DestructData(DataPtr(lo));

	for (; ele != idx; ele = (ele + numEntries - hi) % numEntries)
		Release(ManDatPtr(lo));

	Release(ManDatPtr(lo));
}

