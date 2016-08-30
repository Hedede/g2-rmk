struct zCSoundManager {
	struct zCTableEntry {
		char data[44];
	};

private:
	zCArraySort<zCMediumType> mediumTypes;
	zCTableEntry table[17];
	zCArraySort<zCSoundEventData> eventData1;
	zCArraySort<zCSoundEventData> eventData2;
	int unk;
};
