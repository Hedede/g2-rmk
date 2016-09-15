#pragma once
#include <Gothic/Types/zSTRING.h>
#include <Gothic/Types/zArray.h>

struct zCSoundFX;
struct zCParticleEmitter;
struct zCSoundManager {
	zCSoundManager()
	{
		Init();
	}

	void Init()
	{
		Thiscall<void(zCSoundManager*)> _g_Init{0x5EE3F0};
		_g_Init(this);
	}

	struct zCMediumType {
		int index;
		zSTRING name;
	};

	struct zCSoundEventData {
		zCSoundEventData() = default;

		zSTRING name;
		zCSoundFX *sfx         = nullptr;
		zCParticleEmitter *pfx = nullptr;
	};


	struct zCTableEntry {
		zCTableEntry()
		{
			auto CompareSubTableEntry = reinterpret_cast<zCArraySort<zCSoundEventData>::CompareFunc*>(0x5EC5C0);
			events.Compare = CompareSubTableEntry;
		}

		~zCTableEntry()
		{
			Thiscall<void(zCTableEntry*)> dtor{0x632BD0};
			dtor(this);
		}

		zSTRING name;
		int unk1 = 0;
		int unk2 = 0;
		zCArraySort<zCSoundEventData> events;
	};

private:
	zCArraySort<zCMediumType> mediumTypes;
	zCTableEntry collisionSounds[17];
	zCArraySort<zCSoundEventData> damageSounds;
	zCArraySort<zCSoundEventData> slideSounds;
	int unk;
};

static auto& zsndMan = Value<zCSoundManager*>(0x99B0B0);
