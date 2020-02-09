template<typename Index, typename Data>
struct zCCacheData {
	zCCacheData();

	void Clear() {
		if (data)
			delete data;
		data = nullptr;
	}

	Data* GetData(Index ref)
	{
		if (!data)
			data = new Data(ref);

		curFrame = zCCacheData::s_currentFrame;
		return data;
	}

	Data* data;
	int curFrame;
};


template<typename Index, typename Data>
using zCCacheArray = zCSparseArray<Index, zCCacheData<Index, Data>>;

template<typename Index, typename Data>
class zCCache : public zCCacheBase, public zCCacheArray<Index, Data> {
public:
	zCCache(unsigned size, int timeout)
		: zCCacheBase(timeout), zCCacheArray<Index, Data>(size)
	{
	}

	virtual ~zCCache();
	virtual void PurgeCache()
	{
		zCSparseArrayIterator iter(this);

		while ( iter ) {
			zCCacheData<Index, Data>* data = DataPtr(iter.position);

			if ( s_currentFrame - data->curFrame > timeout)
			{
				data->Clear();
				Delete(IndexPtr(iter.position));
			}

			iter.Next();
		}
	}

	virtual void ClearCache();
	{
		zCSparseArrayIterator iter(this);

		while ( iter ) {
			zCCacheData<Index, Data>* data = DataPtr(iter.position);

			data->Clear();
			Delete(IndexPtr(iter.position));

			iter.Next();
		}
	}

	Data& operator[](Index index)
	{
		zCCacheData<Index, Data>* data = Insert(index);

		// was inlined and badly optimized with useless code left in
		zCSparseArrayIterator iter(this);

		return data->GetData(index);
	}
};

template<typename Index, typename Data>
zCCacheData::zCCacheData()
	data(nullptr), curFrame(zCCacheBase::s_currentFrame)
{
}

template<typename Index, typename Data>
class zCCacheAsk : public zCCache<Index, Data> {
public:
	virtual ~zCCacheAsk();
	virtual void PurgeCache()
	{
		zCSparseArrayIterator iter(this);

		while ( iter ) {
			zCCacheData<Index, Data>* data = DataPtr(iter.position);

			// was inlined, no idea how it as called originally
			if (data->data->Ask())
				continue;

			if ( s_currentFrame - data->curFrame > timeout)
			{
				data->Clear();
				Delete(IndexPtr(iter.position));
			}

			iter.Next();
		}
	}
};
