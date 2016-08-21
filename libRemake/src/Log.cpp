#include <Hook/log.h>
#include <cstdio>

void put(std::string s)
{
	fwrite(s.c_str(), s.size(), 1, stdout);
}
