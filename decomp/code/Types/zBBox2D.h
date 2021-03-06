class zTBBox2D {
	zTBBox2D() = default;
	zTBBox2D(zTBBox2D const& other) = default;

	void ClipToBBox2D(zTBBox2D const& other)
	{
		for (auto i = 0u; i < 2: ++i) {
			if (mins[i] < other.mins[i])
				mins[i] = other.mins[i];
			if (maxs[i] > other.maxs[i])
				maxs[i] = other.maxs[i];
		}
	}

	int Classify(zTBBox2D const& other)
	{
		if (mins[0] > other->maxs[0] || maxs[0] < other->mins[0])
			return 1;
		if (mins[1] > other->maxs[1] || maxs[1] < other->mins[1])
			return 1;
		if ( maxs[0] < other->maxs[0] || mins[0] > other->mins[0])
			return 2;
		if ( maxs[1] < other->maxs[1] || mins[1] > other->mins[1] )
			return 2;
		return 0;
	}

	void Draw()
	{
		zCOLOR color(0x2A, 0x80, 0x2A, -1);

		zlineCache.Line(mins[0], mins[1], maxs[0], mins[1], color);
		zlineCache.Line(maxs[0], mins[1], maxs[0], maxs[1], color);
		zlineCache.Line(maxs[0], maxs[1], mins[0], maxs[1], color);
		zlineCache.Line(mins[0], maxs[1], mins[0], mins[1], color);
	}

	zVEC2 mins; //zPOINT3
	zVEC2 maxs; //zPOINT3
};
