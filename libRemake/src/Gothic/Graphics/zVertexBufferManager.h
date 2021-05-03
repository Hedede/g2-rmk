#pragma once
#include <Gothic/Types/zArray.h>

class zCVertexBuffer;
class zCVertexBufferManager {
public:
	zCArray<zCVertexBuffer*> openVBList;
	zCArray<zCVertexBuffer*> closedVBList;
	int changeWorldHintSet = 0;

	void UnlockOpenVertexBuffers()
	{
		thiscall(0x005FCA30, this);
	}
	void StartChangeWorld()
	{
		changeWorldHintSet = 1;
	}
	void EndChangeWorld()
	{
		changeWorldHintSet = 0;
		UnlockOpenVertexBuffers();
	}
};

