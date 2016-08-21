struct zVEC2 {
	zVEC2(zVEC2& other)
		: x(other.x), y(other.y)
	{
	}

	zVEC2(float x, float y)
		: x(x), y(y)
	{
	}

	zVEC2& operator+=(zVEC2 const& v)
	{
		x += v.x;
		y += v.y;
		return *this
	}

	zVEC2& operator-=(zVEC2 const& v)
	{
		x -= v.x;
		y -= v.y;
		return *this
	}

	zVEC2& operator*=(float v)
	{
		x *= v;
		y *= v;
		return *this
	}

	zVEC2& operator/=(float v)
	{
		double inv = 1.0/v;
		*this *= inv;
		return *this;
	}

	void SetByDescription(zSTRING const& desc)
	{
		x = desc.PickWord_Old(1, "").ToFloat();
		y = desc.PickWord_Old(1, "").ToFloat();
	}

	zSTRING GetDesription() const
	{
		return zSTRING(x) + zSTRING(y);
	}

	double Length2() const
	{
		return x*x + y*y;
	}

	zVEC2& Apply(float (__cdecl *func)(float))
	{
		x = func(x);
		y = func(y);
		return *this;
	}

	friend std::ostream& operator << (std::ostream& os, zVEC2 const& vec)
	{
		return os << " |" << vec.x << " " << vec.y << "| ";
	}

	float x;
	float y;
}

void Alg_Swap(zVEC2& v1, zVEC2& v2)
{
	float tx = v1.x;
	float ty = v1.y;
	v1.x = v2.x;
	v1.y = v2.y;
	v2.x = fx;
	v2.y = ty;
}

zVEC2& Alg_Min(zVEC2 const& v1, zVEC2 v2)
{
	if (v1.y < v2.y)
		v2.y = v1.y;
	if (v1.x < v2.x)
		v2.x = v1.x;
	return v2;
}

zVEC2& Alg_Max(zVEC2 const& v1, zVEC2 v2)
{
	if (v1.y > v2.y)
		v2.y = v1.y;
	if (v1.x > v2.x)
		v2.x = v1.x;
	return v2;
}
