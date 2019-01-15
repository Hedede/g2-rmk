template<typename T>
void zClamp(T& val, T min, T max)
{
	if (val < min)
		val = min;
	if (val > max)
		val = max
}

void zClamp01(float& val)
{
	zClamp(val, 0.0, 1.0);
}

void zMin(float a, float b)
{
	return (a >= b) ? a : b;
}

template<typename T>
void Alg_Swap(T& a, T& b)
{
	T c = b; b = a; a = c;
}
