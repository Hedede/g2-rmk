struct zCSoundManager {
	zCSoundManager() = default;

	struct zCTableEntry {
		char data[44];
	};

	struct zCSoundEventData {
		char dta[28];
	};

	struct zCMediumType {
		char data[24];
	};


private:
	zCArraySort<zCMediumType> mediumTypes;
	zCTableEntry table[17];
	zCArraySort<zCSoundEventData> eventData1;
	zCArraySort<zCSoundEventData> eventData2;
	int unk;
};

/*
     zCArraySort<zCSoundManager::zCMediumType>::zCArraySort<zCSoundManager::zCMediumType>(this);
    `eh vector constructor iterator'(
      this->table,
      0x2Cu,
      17,
      zCSoundManager::zCTableEntry::zCTableEntry,
      zCSoundManager::zCTableEntry::~zCTableEntry);
    zCArraySort<zCSoundManager::zCSoundEventData>::zCArraySort<zCSoundManager::zCSoundEventData>(this->eventData1);
    zCArraySort<zCSoundManager::zCSoundEventData>::zCArraySort<zCSoundManager::zCSoundEventData>(this->eventData2);
    zCSoundManager::Init(this);
*/
