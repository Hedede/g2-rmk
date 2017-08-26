#pragma once

struct zCEventManager {
	static void DoFrameActivity()
	{
		auto& disableEventManagers = Value<int>(0xAB39C8);
		auto& activeEM = Value<zCArray<zCEventManager*>>(0xAB39BC);
		if ( !disableEventManagers ) {
			for (auto& em : activeEM) {
				if (em)
					em->ProcessMessageList();
			}
		}
	}

	void ProcessMessageList()
	{
		// virtual. but there's no overrides
		Thiscall<void(zCEventManager*)> call{0x787000};
		call(this);
	}

	void* vtab;
};
