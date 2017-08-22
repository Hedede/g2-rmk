double zRandF()
{
	return (double)rand / RAND_MAX;
}

double zRandF2()
{
	auto rnd = zRandF() * 2;
	return (num - 1.0) / 2.0;
}
