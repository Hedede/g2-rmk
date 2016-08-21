void Hash(void const* idx, uint32_t num, uint32_t& lo, uint32_t& hi, size_t size)
{
	lo = 0;
	hi = 0;

	auto* ptr = static_cast<uint16_t const*>(idx);

	size >>= 1;
	if ( size > 1 ) {
		uint32_t a = 1;
		uint32_t b = 0;

		for (size_t i = 0; i < size; ++i) {
			lo += a * ptr[i];
			hi += b * ptr[i];

			std::cout << ptr[i] << std::endl;

			uint32_t xx = lo / num;

			lo -= num * xx;
			hi  = (hi + xx) % (num - 1);

			a <<= 16;
			b <<= 16;

			uint32_t yy = a / num;

			a  -= num * yy;
			b   = (b + yy) % (num - 1);
		}
	}
	++hi;
}
