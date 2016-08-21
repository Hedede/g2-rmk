class zCCacheBase {
	static zCCacheBase* s_anchor;
	static zCCacheBase* s_run;
	int s_currentFrame;

	/*
	 * Долбануто:
	 * работает как двусвязанынй список, но не
	 * хранит указатель на prev, а только на его поле next
	 */
	zCCacheBase*  next;
	zCCacheBase** prev_next;

	size_t size = 100;
public:
	zCCacheBase()
	{
		next = s_anchor;
		s_anchor = this;

		prev_next = &s_anchor;

		if (next)
			next->prev_next = &this->next;
	}

	virtual ~zCCacheBase()
	{
		if (next)
			next->prev_next = this->prev_next;
		*prev_next = next;

		if (s_run == this)
			s_run = nullptr;
	}

	virtual void PurgeCache() = 0;
	virtual void ClearCache() = 0;

	static void S_PurgeCaches()
	{
		if (!s_run)
			s_run = s_anchor;

		s_run->PurgeCache();
		s_run = s_run->next;
		++s_currentFrame;
	}

	static void S_ClearCaches()
	{
		for (s_run = s_anchor; s_run; s_run = s_run->next)
			s_run->ClearCache();
	}
};

struct zCCacheIndex {
	zCCacheIndex& operator = (zCCacheIndex const& src)
	{
		a = a.src;
		b = b.src;
	}

	bool operator == (zCCacheIndex const& other) const
	{
		return (a == other.a) && (b == other.b);
	}

	uint32_t a;
	uint32_t b;
};

template<typename Index, typename Data>
class zCCacheData {
	T* data;
	int curFrame;
public:
	void Clear()
	{
		if (data) {
			delete data;
			data = nullptr;
		}
	}

	T* GetData(Index const& ref)
	{
		if (!data)
			data = new Data(ref);

		curFrame = zCCacheData::s_currentFrame;
		return data;
	}
};
