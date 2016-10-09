struct oCNpcFocus {

	float GetAzi() const { return azimuth; }
	float GetMaxRange() const { return maxRange; }
	float GetRange2() const { return range2; }
	float GetMobThrowRange() const { return mobThrowRange; }
	float GetItemThrowRange() const { return itemThrowRange; }


	int unk[2];
	float range2;
	float azimuth;
	int unk0[4];
	float itemThrowRange;
	int unk1[5];
	float mobThrowRange;
	int unk2[4];
	float maxRange;
};
