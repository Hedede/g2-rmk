class zCZoneReverb : public zCZone {
	Z_OBJECT(zCZoneReverb);
public:
	virtual void Archive(zCArchiver& arc)
	{
		zCZone::Archive(arc);

		arc.WriteGroupBegin("ZoneReverb");

		if ( !arc.InProperties() ) {
			arc.WriteEnum("reverbPreset", "", reverbPreset);
		} else if (zCSoundSystem::GetNumReverbPresets() > 0) {
			int numPresets = zCSoundSystem::GetNumReverbPresets();
			zSTRING enumStr;
			for (int i = 0; i < numPresets; ++i)
				enumStr += zCSoundSystem::GetReverbPresetName(i) + ";";
			arc.WriteEnum("reverbPreset", enumStr, reverbPreset);
		}

		arc.WriteFloat("reverbWeight", reverbWeight);
		arc.WriteFloat("innerRangePerc", innerRangePerc);
		arc.WriteGroupEnd("ZoneReverb");
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCZone::Unarchive(arc);

		if (arc.InProperties() || zCSoundSystem::GetNumReverbPresets() > 0 )
			reverbPreset = arc.ReadEnum("reverbPreset");

		arc.ReadFloat("reverbWeight", reverbWeight);
		arc.ReadFloat("innerRangePerc", innerRangePerc);
		if (innerRangePerc < 0.0)
			innerRangePerc = 0.0;
		else if (innerRangePerc > 1.0)
			innerRangePerc = 1.0;
	}

	virtual ~zCZoneReverb();
	virtual void ProcessZoneList(zCArraySort<zCZone *> const &,zCArraySort<zCZone *> const &,zCWorld *);
	virtual void GetDefaultZoneClass();
	virtual void GetDebugDescString();

private:
	int reverbPreset;
	float reverbWeight;
	float innerRangePerc;
};

class zCZoneReverbDefault : public zCZoneReverb{
	Z_OBJECT(zCZoneReverbDefault);
public:
	virtual ~zCZoneReverbDefault();
	virtual void Unarchive(zCArchiver& arc)
	{
		zCZoneReverb::Unarchive(arc);
		zsound->SetGlobalReverbPreset(reverbPreset, reverbWeight);
	}
	virtual void ThisVobAddedToWorld(zCWorld *);
	virtual void ProcessZoneList(zCArraySort<zCZone *> const &,zCArraySort<zCZone *> const &,zCWorld *);
};
