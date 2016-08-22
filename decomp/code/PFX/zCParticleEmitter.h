struct zSParticle {
	zSParticle() = default;
};

struct zCParticleEmitter {
	zCParticleEmitter()
	{
		Reset();

		unk16[6] = 0;
		unk17[1] = 0;
		unk17[2] = 0;
		unk14[7] = 0;
		unk14[6] = 0;
		unk14[8] = 0;
		unk14[9] = 0;
	}

	void Reset()
	{
		memset(this, 0, sizeof(zCParticleEmitter));
		ResetStrings();
	}

	void ResetStrings()
	{

		str1.Clear();
		str2.Clear();
		str3.Clear();
		str4.Clear();
		str5.Clear();
		str6.Clear();
		str7.Clear();
		visualName.Clear();
		str8.Clear();
		str9.Clear();
		str10.Clear();
		str11.Clear();
		str12.Clear();
		str13.Clear();
		str14.Clear();
		str15.Clear();
		str18.Clear();
		str16.Clear();
		str17.Clear();
		str19.Clear();
		str20.Clear();
		str21.Clear();
		str22.Clear();
		str23.Clear();
	}

	void UpdateVelocity() {}

public:
	int unk;
	zSTRING str1;
	int unk0[3];
	zSTRING str2;
	int unk1;
	zSTRING str3;
	zSTRING str4;
	zSTRING str5;
	zSTRING str6;
	int unk2[2];
	zSTRING str7;
	zSTRING visualName;
	int unk3;
	zSTRING str8;
	int unk4[3];
	zSTRING str9;
	zSTRING str10;
	zSTRING str11;
	zSTRING str12;
	int unk5[8];
	zSTRING str13;
	int unk6;
	zSTRING str14;
	zSTRING str15;
	int unk7[3];
	zSTRING str16;
	zSTRING str17;
	zSTRING str18;
	int unk8;
	zSTRING str19;
	int unk9[3];
	zSTRING str20;
	int unk10[2];
	zSTRING str21;
	int unk11;
	zSTRING str22;
	int unk12[2];
	zSTRING str23;
	char unk13[8];
	zSTRING pfxName;
	zCArray<float> arr1;
	int unk14[15];
	zCArray<float> arr2;
	int unk15[6];
	zTBBox3D bbox;
	int unk16[10];
	zVEC3 vec1;
	zVEC3 vec2;
	int unk17[10];
};
