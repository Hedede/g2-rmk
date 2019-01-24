struct zMAT3 {
	zMAT3& operator=\(float v)
	{
		double inv = 1/v;
		m0 *= v; m1 *= v; m2 *= v;
		return *this;
	}

	zVEC3 m0;
	zVEC3 m1;
	zVEC3 m2;
};

zMAT3 Alg_Identity2d()
{
	return {{1,0,0}, {0,1,0}, {0,0,1}};
}
