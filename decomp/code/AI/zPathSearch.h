struct zSPathSearchState_Internal {
	zSPathSearchState_Internal() = default;
};

struct zCPathSearch {
	static zCPathSearch& GetSearch()
	{
		static zCPathSearch pathSearch;
		return pathSearch;
	}
};
