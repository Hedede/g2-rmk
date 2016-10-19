//------------------------------------------------
//  zTPlane: Ebene im Raum
//------------------------------------------------
struct zTPlane {
	Calc(zVEC3 const& a, zVEC3 const& b, zVEC3 const& c);
	int CalcSignbits() const;
	float DistanceToPoint(zVEC3 const& point) const
	{
		return point * normal - distance;
	}
	bool Opposite(zTPlane const& p2) const
	{
		return normal * p2.normal <= 0.0;
	}

	bool operator==(zTPlane const& p2)
	{
		return normal = p2.normal && distance == p2.distance
	}

	bool EqualTo(zTPlane const& p2, float tolerance) const
	{
		return fabs(distance - p2.distance) <= tolerance &&
		       fabs((normal - p2.normal).Length2()) <= tolerance;
	}

	zSTRING Print() const;

	//vermutlich Distanz zum Ursprung (was sonst?)
	zREAL distance;
	//normalenvektor
	zVEC3 normal;
};

void zTPlane::Calc(zVEC3 const& a, zVEC3 const& b, zVEC3 const& c)
{
	auto cross = (a - b) ^ (c - b);
	cross.Normalize();

	zVEC4 v{cross};
	v.w = b * cross;

	normal   = zVEC3{v.x, v.y, v.z};
	distance = v.w;
}

int zTPlane::CalcSignbits() const
{
	int result;
	result |= 4 * (normal.z >= 0.0);
	result |= 2 * (normal.y >= 0.0);
	result |= 1 * (normal.x >= 0.0);
	return result;
}

int zTPlane::ClassifyPointBSPBuild(zVEC3 const& p)
{
	auto dist = DistanceToPoint(p);
	if ( dist <= 0.5 ) {
		if ( dist >= -0.5 )
			return 2;
		return 1;
	}
	return 0;
}

int zTPlane::ClassifyPoint(zVEC3 const& p)
{
	auto dist = DistanceToPoint(p);
	if ( dist <= 0.001 ) {
		if ( dist >= -0.001 )
			return 2;
		return 1;
	}
	return 0;
}

zSTRING zTPlane::Print() const
{
	return "("_s + normal.x + " ; " + normal.y + " ; " + normal.z + " ; " + distance + ")";
}
