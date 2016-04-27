template<typename Index, typename Data>
struct zCCacheData {
	zCCacheData();

	Clear() {
		if (data)
			delete data;
		data = nullptr;
	}

	Data* data;
	int curFrame;
};

template<typename Index, typename Data>
using zCCacheArray = zCSparseArray<Index, zCCacheData<Index, Data>>;

template<typename Index, typename Data>
class zCCache : public zCCacheBase, public zCCacheArray<Index, Data> {
public:
	virtual ~zCCache();
	virtual void PurgeCache();
	virtual void ClearCache();

	Data& operator[](Index const& key)
	{
	
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
	virtual void PurgeCache();
};
