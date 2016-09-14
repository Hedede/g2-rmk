struct zCFontMan {
	zCFontMan() = default;
	~zCFontMan()
	{
		for (auto font : fontList)
			Delete(font);
	}

	int GetNumber() const
	{
		return fontList.GetNum();
	}

	zCFont* GetFont(int idx)
	{
		if (idx < GetNumber())
			return fontList[idx];
		return nullptr;
	}

	int Load(zSTRING const& name)
	{
		int idx = SearchFont(name);
		if (idx > 0)
			return idx;

		fontList.InsertEnd(new zCFont(name));
		return GetNumber() - 1;
	}

private:
	int SearchFont(zSTRING const& name)
	{
		zSTRING _name = name;
		_name.Upper();

		int i = 0;
		for (auto font : fontList) {
			if (name == font->GetFontName())
				return i;
			++i;
		}
		return -1;
	}

	zCArray<zCFont*> fontList;
};
