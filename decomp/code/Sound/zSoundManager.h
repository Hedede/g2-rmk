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
