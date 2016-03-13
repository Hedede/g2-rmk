int BitToNumber(uint64_t bit)
{
	int result = 0;
	if ( bit )
	{
		while ( !(bit & 1) )
		{
			bit >>= 1;
			if ( ++result >= 64 )
				return 0;
		}
	}
	return result;
}

void TestKeys_Carsten()
{
}
