int zGetHighestBit(int val)
{
	int mask = 0x80000000;
	int result = 31;
	while ( !(val & mask) ) {
		if ( result <= 0 )
			break;

		mask >>= 1;
		--result;
	}
	return result;
}
