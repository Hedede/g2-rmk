class zVEC4 {
public:
	zVEC4() = default;
	zVEC4(float val);

	zVEC4::zVEC4(zVEC4 const& other)
	{
		v[0] = other.v[0];
		v[1] = other.v[1];
		v[2] = other.v[2];
		v[3] = other.v[3];
	}

	zVEC4::zVEC4(int val)
	{
		v[0] = val;
		v[1] = val;
		v[2] = val;
		v[3] = val;
	}

	zVEC4::zVEC4(float x, float y, float z, float w)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = w;
	}

	zVEC4::zVEC4(zVEC3 const& v3, float w)
	{
		v[0] = v3.x;
		v[1] = v3.y;
		v[2] = v3.z;
		v[3] = w;
	}

	float& zVEC4::operator[](int index)
	{
		return v[index];
	}

	float LengthApprox()
	{
		float x = fabs(v[0]);
		float y = fabs(v[1]);
		float z = fabs(v[2]);
		float v = fabs(v[3]);
		if ( x < y ) std::swap(x, y);
		if ( z < w ) std::swap(z, w);
		if ( x < z ) std::swap(x, z);
		if ( y < w ) std::swap(y, w)
		if ( y < z ) std::swap(y, z)
		return x + (w * 16.0 + z * 19.0 + y * 25.0) / 60.0;
	}

	zVEC4& Normalize()
	{
		float inv = 1.0 / Length();
		v[0] *= Length();
		v[1] *= Length();
		v[2] *= Length();
		v[3] *= Length();
		return *this;
	}


	zVEC4* Apply(float (__cdecl *func)(float))
	{
		v[0] = func(v[0]);
		v[1] = func(v[1]);
		v[2] = func(v[2]);
		v[3] = func(v[3]);
		return *this;
	}

private:
	float v[4];
};
