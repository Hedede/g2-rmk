/*
 * This looks completely bogus to me
 */
struct zCPrime {
	static zBOOL IsPrime(uint32_t num);
	static uint16_t PrimeNo(uint16_t i);
	static uint32_t NextPrime(uint32_t num);
};

static uint16_t SmallPrime_Current_Index = 0;


int zCPrime::IsPrime(uint32_t num)
{
	if ( num <= 1 )
		return false;

	for ( int i = 0; ; ++i ) {
		auto p = PrimeNo(i);
		if ( p*p > num )
			break;
		if ( num % SmallPrimes[i+1] == 0)
			return false;
	}
	return true;
}

uint16_t zCPrime::PrimeNo(uint16_t i)
{
	if (i < SmallPrime_Current_Index)
		return SmallPrimes[i + 1];

	if ( SmallPrime_Current_Index == 0 ) {
		SmallPrime_Current_Index = 1;
		SmallPrimes[1] = 2;
	}

	if (i < SmallPrime_Current_Index)
		return SmallPrimes[i + 1];

	auto idx = SmallPrime_Current_Index;
	auto p = SmallPrimes[idx] + 1;
	while ( p < 0x10000 ) {
		if (p > 1) {
			for (int n = 0;; ++n) {
				auto pn = zCPrime::PrimeNo(n);
				while (p % pn)
				if ( pn*pn > p ) {
					SmallPrimes[++SmallPrime_Current_Index] = p;
					break;
				}
			}
		}

		++p;

		if ( i < SmallPrime_Current_Index )
			return SmallPrimes[i + 1];
	}
}

uint32_t zCPrime::NextPrime(uint32_t num)
{
	for ( auto n = num; n != -1; ++n ) {
		if (IsPrime(n)) // was inlined
			return n;
	}
	return -1;
}
