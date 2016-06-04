class zCModelAni : zCObject {
	Z_OBJECT(zCModelAni);
public:
	virtual ~zCModelAni();

	int GetAniID()
	{
		return aniId;
	}

	zSTRING const& GetAniName()
	{
		return aniName;
	}

	int GetAniType()
	{
		return bitfield << 26 >> 26;
	}

private:
	zSTRING aniName;
	zSTRING __unk;
	int aniId;
	int unk0[7];
	int layer; // maybe?
	int yob[23];
	zTMdl_AniSample **eee;
	int uu;
	int UU;
	int **heh;
	int bitfield;
	unsigned __int8 flags;
};

struct zCModelAniActive {
	zCModelAni *ani;
	int unk0[4];
	float flt1;
	float flt2;
	int unk1[5];
	zVEC3 vec3_0;
	zVEC3 vec3_1;
	zCQuat quat0;
	zCQuat quat1;
	zCQuat quat2;
	int unk3[4];
};
