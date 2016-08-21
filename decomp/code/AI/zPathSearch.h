struct zCPathSearch {
	static zCPathSearch& GetSearch()
	{
		static zCPathSearch pathSearch;
		return pathSearch;
	}
};
