class zCVobSoundDaytime : public zCVobSound {
	Z_OBJECT(zCVobSoundDaytime);
public:
	virtual ~zCVobSoundDaytime();

	virtual void Archive(zCArchiver& arc)
	{
		zCVobSound::Archive(arc);
		arc.WriteGroupBegin("SoundDaytime");
		arc.WriteFloat("sndStartTime", sndStartTime);
		arc.WriteFloat("sndEndTime", sndEndTime);
		arc.WriteString("sndName2", sndName2);
		arc.WriteGroupEnd("SoundDaytime");
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCVobSound::Unarchive(vtbl, a2);
		Release(sndFx2);
		sndHandle2 = 0;

		arc.ReadFloat("sndStartTime", sndStartTime);
		if ( sndStartTime < 0.0 )
			sndStartTime = 0.0;
		else if (sndStartTime > 24.0)
			sndStartTime = 24.0

		arc.ReadFloat("sndEndTime", sndEndTime);
		if ( sndEndTime < 0.0 )
			sndEndTime = 0.0;
		else if (sndEndTime > 24.0)
			sndEndTime = 24.0

		arc.ReadString("sndName2", sndName2);
	}

	virtual zCClassDef* GetZoneMotherClass() const
	{
		return &zCVobSound::classDef;
	}

	virtual void GetDebugDescString();
	virtual void DoSoundUpdate(float);

private:
	float sndStartTime;
	float sndEndTime;
	zSTRING sndName2;
	zCSoundFX *sndFx2;
	int sndHandle2;
};
