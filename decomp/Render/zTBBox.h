class zTBBox2D {
	zVEC2 mins; //zPOINT3
	zVEC2 maxs; //zPOINT3
};

class zTBBox3D {
	zTBBox3D() = default;
	zTBBox3D(zTBBox3D const& other) = default;

	void Init()
	{
		mins[0] = std::numeric_limits<float>::max();
		mins[1] = std::numeric_limits<float>::max();
		mins[2] = std::numeric_limits<float>::max();
		maxs[0] = std::numeric_limits<float>::min();
		maxs[1] = std::numeric_limits<float>::min();
		maxs[2] = std::numeric_limits<float>::min();
	}

	void SetMaximumBox()
	{
		mins[0] = std::numeric_limits<float>::min();
		mins[1] = std::numeric_limits<float>::min();
		mins[2] = std::numeric_limits<float>::min();
		maxs[0] = std::numeric_limits<float>::max();
		maxs[1] = std::numeric_limits<float>::max();
		maxs[2] = std::numeric_limits<float>::max();
	}

	zVEC3 GetCenterFloor()
	{
		return {(mins.x + maxs.x)/2, mins.y, (mins.z + maxs.z)/2};
	}

	float GetVolume()
	{
		auto dim = maxs - mins;
		return dim.x * dim.y * dim.z;
	}

	bool IsIntersecting(zTBBox3D const& other)
	{
		return other.maxs[0] >= mins[0] && other.mins[0] <= maxs[0] &&
		return other.maxs[1] >= mins[1] && other.mins[1] <= maxs[1] &&
		return other.maxs[2] >= mins[2] && other.mins[2] <= maxs[2];
	}

	void SaveBIN(zCFileBIN& bin)
	{
		bin.Write(this, sizeof(this));
	}

	void LoadBIN(zCFileBIN& bin)
	{
		bin.Read(this, sizeof(this));
	}

	zVEC3 mins; //zPOINT3
	zVEC3 maxs; //zPOINT3
};

struct zCOBBox3D {
	zVEC3 center;
	zVEC3 axis[3];
	zVEC3 extent;
	zCList<zCOBBox3D>    childs;
};

void zCOBBox3D::SaveBIN(zCFileBIN& bin)
{
	bin.Write(&center, sizeof(center));
	bin.Write(&axis[0], sizeof(axis));
	bin.Write(&extent, sizeof(extent));

	zCList<zCOBBox3D>* next = childs.next;
	for (uint16_t i = 0; next; ++i )
		next = next->next;

	bin.Write(&i, sizeof(i));

	next = childs.next;
	for (; i > 0; --i) {
		next->Get()->SaveBIN(bin);
		--i;
		next = next->next;
	}
}
