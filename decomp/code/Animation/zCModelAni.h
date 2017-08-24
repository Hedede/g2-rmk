struct zCModelAniEvent
{
	int type;
	int unk1;
	zSTRING name;
	zSTRING paramz[4];
	int unk2[4];
	zCArray<zCResource*> resources;
	int what[4];
};

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
	int unk1[9];
	zCModelAni *__stateAni;
	int yob[5];
	zCModelAniEvent **__aniEvents;
	float flr;
	int oyb[2];

	IntArray __arr;
	zCModelNode **__nodeList;
	zTMdl_AniSample *__aniSamples;
	int __samplePosRangeMin;
	int __samplePosScaler;

	int16_t numFrames;

	int16_t __numSamples;

	int bitfield;
	uint8_t flags;
};



struct zCModelAniActive {
	void SetDirection(zTMdl_AniDir dir)
	{
		switch (dir) {
		case 2:
			if ( (protoAni->numFrames / 2) > __curFrame )
			{
		case 0:
				__aniDir = -1;
				return;
			}
		case 1:
		default:
			__aniDir = 1;
		}
	}

	void SetProgressPercent(float perc)
	{
		double fnum = protoAni->numFrames - 1;
		if ( __aniDir < 0 )
			__curFrame = fnum * (1.0 - perc);
		else
			__curFrame = fnum * perc;
	}

	zREAL GetProgressPercent()
	{
		double fnum = protoAni->numFrames;
		if ( __aniDir < 0 )
			return 1.0 - __curFrame / fnum;
		else
			return       __curFrame / fnum;
	}

	void SetActFrame(float frame)
	{
		__curFrame = frame;
		double fnum = protoAni->numFrames - 1;
		zClamp(__curFrame, 0.0, fnum);
	}

	zCModelAni *protoAni;
	zCModelAni *nextAni;
	int __aniDir;
	float __curFrame;
	int unk0[1];
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
