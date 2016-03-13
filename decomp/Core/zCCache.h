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
class zCCacheAsk : public zCCache<Index, Data> {
public:
	virtual ~zCCacheAsk();
	virtual void PurgeCache();
};
