#ifndef GOTHIC_ZCCSPLAYER_H
#define GOTHIC_ZCCSPLAYER_H

struct zCCSPlayer {
	void Process()
	{
		Thiscall<void(zCCSPlayer*)> call{0x4208C0};
		call(this);
	}
};

#endif//GOTHIC_ZCCSPLAYER_H
