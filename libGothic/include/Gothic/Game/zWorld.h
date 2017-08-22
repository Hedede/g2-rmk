#pragma once

struct zCWorld {
	void* vtab;
	char data[0x6254];
};


struct oCWorld : zCWorld {
	char data2[52];
};

CHECK_SIZE(zCWorld, 0x6258);
CHECK_SIZE(oCWorld, 0x628C);
