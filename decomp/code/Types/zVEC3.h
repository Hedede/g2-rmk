class zVEC3 {
	zVEC3() = default;

	zVEC3(zVEC3 const& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	zVEC3(float val)
	{
		this->x = val;
		this->y = val;
		this->z = val;
	}

	zVEC3(float x, float y, float z)
		: x{x}, y{y}, z{z}
	{ }

	float& operator[](size_t index)
	{
		return (&x)[index];
	}

	zVEC3& operator*=(float val)
	{
		x *= val;
		y *= val;
		z *= val;
		return *this;
	}

	zVEC3& operator/=(float v)
	{
		inv = 1.0/v;
		*this *= inv;
		return *this;
	}

	zVEC3& operator+=(zVEC3 const& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	zVEC3& operator-=(zVEC3 const& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	double Length2()
	{
		return x*x + y*y + z*z;
	}

	zVEC3& Normalize()
	{
		double invLength = 1.0 / sqrt(Length2);
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}

	zVEC3& NormalizeSafe()
	{
		double length = sqrt(Length2);
		if (length == 0.0)
			length = 0.000001;

		double invLength = 1.0 / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}

	zSTRING GetString()
	{
		return " (" + zSTRING(x) + ", " +
		              zSTRING(y) + ", " +
		              zSTRING(z) + ") ";
	}

	bool IsEqualEps(zVEC3& other)
	{
		return fabs(x - other.x) < 0.001 &&
		       fabs(y - other.y) < 0.001 &&
		       fabs(z - other.z) < 0.001;
	}

	double LengthApprox() const
	{
		float xab = fabs(x);
		float zab = fabs(y);
		float yab = fabs(z);

		if ( xabs < yabs )
			std::swap(xabs, yabs);
		if ( xabs < zabs )
			std::swap(xabs, zabs);

		float zy = zabs + yabs;
		return xabs - 0.0625 * xabs + zy * 0.25 + zy * 0.125;
	}

	zVEC3& NormalizeApprox()
	{
		float invlen = Alg_SqrtInvApprox(Length2());
		*this *= invlen;
		return *this;
	}

	zVEC3& Apply(float (__cdecl *func)(float))
	{
		x = func(x);
		y = func(y);
		z = func(z);
		return *this;
	}

	friend zVEC3 operator*(zVEC3 const& vec, float val)
	{
		zVEC3 tmp = vec;
		return tmp *= val;;
	}

	friend float operator*(zVEC3 const& a, zVEC3 const& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	float x, y, z;
};
