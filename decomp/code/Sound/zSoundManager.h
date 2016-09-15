struct zCSoundManager {
	zCSoundManager()
	{
		Init();
	}

	void Init();

	struct zCSoundEventData {
		zCSoundEventData() = default;
		~zCSoundEventData()
		{
			Release(sfx);
			Release(pfx);
		}

		void GetData(zCSoundFX*& sfx, zCParticleEmitter*& pfx)
		{
			GetData("", sfx, pfx);
		}

		void GetData(zSTRING const& prefix, zCSoundFX*& sfx, zCParticleEmitter*& pfx)
		{
			if ( !this->sfx ) {
				auto name = prefix + this->name;
				this->sfx = zsound->LoadSoundFXScript(name);
				if ( this->sfx ) {
					auto parser = zsound->GetSFXParser();
					parser->CreateInstance(name, &zCSoundManager::scriptSoundData);
					//TODO: emitterName probably is member of ^
					this->pfx = zCParticleFX::SearchParticleEmitter(&emitterName);
				}
			}
			sfx = this->sfx;
			pfx = this->pfx;
		}

		zSTRING name;
		zCSoundFX *sfx         = nullptr;
		zCParticleEmitter *pfx = nullptr;
	};

	struct zCTableEntry {
		zCTableEntry()
		{
			events.Compare = CompareSubTableEntry;
		}

		~zCTableEntry()
		{
			events.DeleteList();
			Release(unk1);
		}

		zSTRING name;
		int unk1 = 0;
		int unk2 = 0;
		zCArraySort<zCSoundEventData> events;
	};

	struct zCMediumType {
		int index;
		zSTRING name;
	};


private:
	zCArraySort<zCMediumType> mediumTypes;
	// TODO: rename
	zCTableEntry table[17]; // Collision sounds
	zCArraySort<zCSoundEventData> eventData1; // Damage sounds
	zCArraySort<zCSoundEventData> eventData2; // Slide sounds
	int unk;
};

int CompareSubTableEntry(zCSoundEventData const* a, zCSoundEventData const* b)
{
	return a->name.Compare(b->name);
}

void zCSoundManager::Init()
{
	debugEnabled = 0;

	auto parser = zsound->GetSFXParser();
	if (!parser)
		return;

	mediumTypes.Compare = CompareMediumType;
	eventData1.Compare = CompareSubTableEntry;
	eventData2.Compare = CompareSubTableEntry;

	int count = 0;
	auto num = parser->symtab.GetNumInList();
	for (auto i = 0; i < num; ++i) {
		// See comments in SfxInst.d
		// about DS_ CS_ and SS_
		auto sym = parser->GetSymbol(i);
		auto sname = sym->name;
		auto name = sym->name;
		name.Delete("_", 3);

		if ( sym->GetType() == zPAR_TYPE_STRING ) {
			if (sname.Search(0, "CS_", 1u) != -1 && sname.Length() == 5 ) {
				zCMediumType mt;
				int sum = 0;
				int offs = 0;
				for (auto c : name) {
					int x = c << offs;
					offs += 8;
					sum += x;
				}
				mt.unk = sum;
				mt.name = sym->GetValue(mt.name, 0);

				mediumTypes.InsertEnd(mt);
			}
		} else if ( sym->GetValue() == zPAR_TYPE_INSTANCE) {
			if (sname.Search(0, "CS_", 1u) != -1) {
				auto idx = GetCollSndTableIndex(&name);
				if ( sname.Length() == 6 ) {
					table[idx].name = sname;
				} else if ( sname.Length() == 12 ) {
					name.Delete("_", 3);
					zCSoundEventData evt;
					evt.name = name;
					table[idx].events.InsertEnd(evt);
				}
			} else if ( sname.Search(0, "SS_", 1u) != -1 ) {
				zCSoundEventData evt;
				evt.name = sname;
				eventData2.InsertEnd(evt);
			} else if ( sname.Search(0, "DS_", 1u) == -1 ) {
				zCSoundEventData evt;
				evt.name = sname;
				eventData1.InsertEnd(evt);
			}
			++count;
		}
	}

	for (auto& entry : table)
		entry.events.QSort();

	mediumTypes.QSort();
	eventData1.QSort();
	eventData2.QSort();

	zINFO("D: zCSoundManager: script sounds registered: "_s + count);
}
