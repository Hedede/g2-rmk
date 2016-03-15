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
