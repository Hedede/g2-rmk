class zTBBox2D {
	zVEC2 mins; //zPOINT3
	zVEC2 maxs; //zPOINT3
};

class zTBBox3D {
	zTBBox3D() = default;
	zTBBox3D(zTBBox3D const& other) = default;


	void SetMaximumBox()
	{
		mins[0] = std::numeric_limits<float>::min();
		mins[1] = std::numeric_limits<float>::min();
		mins[2] = std::numeric_limits<float>::min();
		maxs[0] = std::numeric_limits<float>::max();
		maxs[1] = std::numeric_limits<float>::max();
		maxs[2] = std::numeric_limits<float>::max();
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
