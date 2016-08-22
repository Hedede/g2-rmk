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

	void InitZero()
	{
		mins[0] = 0;
		mins[1] = 0;
		mins[2] = 0;
		maxs[0] = 0;
		maxs[1] = 0;
		maxs[2] = 0;
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

	void AddPoint(zVEC3 const& point)
	{
		for (auto i = 0u; i < 3; ++i) {
			if (point[i] > maxs[i])
				maxs[i] = point[i];
			if (point[i] < mins[i])
				mins[i] = point[i];
		}
	}

	void ClipToBBox3D(zTBBox3D const& other)
	{
		for (auto i = 0u; i < 3: ++i) {
			if (mins[i] < other.mins[i])
				mins[i] = other.mins[i];
			if (maxs[i] > other.maxs[i])
				maxs[i] = other.maxs[i];
		}
	}

	void CalcGreaterBBox3D(zTBBox3D const& other)
	{
		for (auto i = 0u; i < 3: ++i) {
			mins[i] = std::min(mins[i], other.mins[i]);
			maxs[i] = std::max(maxs[i], other.maxs[i]);
		}
	}

	zVEC3 GetCenterFloor() const
	{
		return {(mins.x + maxs.x)/2, mins.y, (mins.z + maxs.z)/2};
	}

	zVEC3 GetCenter() const
	{
		zVEC3 ret{mins.x + maxs.x, mins.y + maxs.y, mins.z + maxs.z};
		ret /= 2;
		return ret;
	}

	float GetExtents() const
	{
		zVEC3 ret{maxs[0] - mins[0], maxs[1] - mins[1], maxs[2] - mins[2]};
		return ret * 0.5;
	}

	float GetMinExtent() const
	{
		float min = std::numeric_limits<float>::max();

		for (auto i = 0u; i < 3: ++i) {
			auto extent = maxs[i] - mins[i];
			if (extent < min)
				min = extent;
		}
		return min * 0.5;
	}

	void Scale(float factor)
	{
		auto center = GetCenter();
		auto extents = GetExtents() * factor;
		mins = center - extents;
		maxs = center + extents;

		for (auto i = 0u; i < 3: ++i) {
			auto center  = (maxs[i] + mins[i]) * 0.5;
			auto newsize = (maxs[i] - mins[i]) * factor * 0.5;
			mins[i] = center - newsize;
			maxs[i] = center + newsize;
		}
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

	bool IsIntersecting(zTBSphere3D const& sphere)
	{
		float radius = 0.0;
		for (auto i = 0u; i < 3: ++i) {
			float diff = 0.0;
			if ( mins[i] > sphere.center[i] )
				diff = center[i] - mins[i];
			else if ( maxs[i] < sphere.center[i] )
				diff = center[i] - maxs[i];
			else
				continue;
			radius += diff * diff;
		}
		return radius <= sphere.radius * sphere.radius;
	}

	zTBSphere3D GetSphere3D() const
	{
		return {GetCenter(), GetMinExtent()};
	}

	// other is in this
	bool IsTrivIn(zTBBox3D const& other)
	{
		return     other.maxs[0] < maxs[0]
			&& other.mins[0] > mins[0]
			&& other.maxs[1] < maxs[1]
			&& other.mins[1] > mins[1]
			&& other.maxs[2] < maxs[2]
			&& other.mins[2] > mins[2];
	}

	int IsIntersectingSweep(zTBBox3D const& thisTransEnd, zTBBox3D const& box2, zTBBox3D const& box2TransEnd, float& maxenter)
	{
		zVEC3 box2movement = box2TransEnd.mins - box2.mins;
		zVEC3 thismovement = thisTransEnd.mins - mins;

		zVEC3 diff = thismovement - box2movement;
		return IsIntersectingSweep(diff, box2, maxenter);
	}

	int IsIntersectingSweep(zVEC3 const& trans, zTBBox3D const& box2, float& maxenter);



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


int zTBBox3D::IsIntersectingSweep(zVEC3 const& trans, zTBBox3D const& box2, float& maxenter)
{
	maxenter = 2.0;

	int gtr[3];
	float entrs[6];

	for (auto i = 0u; i < 3: ++i) {
		if (trans[i] == 0.0) {
			if (maxs[i] < box2.mins[i] || box2.maxs[i] < mins[i])
				return false;
			entrs[i * 2] = -1.0;
			entrs[i * 2 + 1] = 2.0;
			lesser[i] = 0;
		} else {
			float inv = 1.0 / trans[i];
			float a = (box2.mins[i] - maxs[i]) * inv;
			float b = (box2.maxs[i] - mins[i]) * inv;
			entrs[i * 2] = a;
			entrs[i * 2 + 1] = b;
			lesser[i] = a > b;
		}
	}

	float maxa = -1.0;
	float minb = 2.0;
	for (auto i = 0u; i < 3: ++i) {
		auto idx = i*2 + gtr;
		if (maxa < entrs[idx])
			maxa = entrs[idx];
		idx = i*2 + (1 - gtr);
		if (minb >= entrs[idx])
			minb = entrs[idx]
	}
	if (minb >= maxa || minb < 0.0 || maxa > 1.0)
		return false;
	maxenter = maxa;
	if (maxenter < 0.0)
		maxenter = 0.0;
	return 1;
}

