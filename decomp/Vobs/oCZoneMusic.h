/* address of the pointer that points to the current MusicZone.
   poorly tested, was null most of the time, don't know why. */
const int oCZoneMusic__s_musiczone_Address = 10111524; //0x9A4A24 //oCZoneMusic*

class oCZoneMusic : public zCZoneMusic {
	Z_OBJECT(oCZoneMusic);
public:
	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual void ~oCZoneMusic(uint);
	virtual void ProcessZoneList(zCArraySort<zCZone *> const &,zCArraySort<zCZone *> const &,zCWorld *);
	virtual void GetDefaultZoneClass();
	virtual void GetDebugDescString();
	virtual void OnTrigger(zCVob *);
	virtual void OnUntrigger(zCVob *);
public:
	zBOOL enabled;
	zBOOL local_enabled;
	int priority;
	zBOOL ellipsoid;
	float reverbLevel;
	float volumeLevel;
	zBOOL loop;
	zBOOL dayEntranceDone;
	zBOOL nightEntranceDone;
};

class oCZoneMusicDefault : public oCZoneMusic {
	Z_OBJECT(oCZoneMusicDefault);
public:
	virtual ~oCZoneMusicDefault();
};

void oCZoneMusic::Archive(zCArchiver& arc)
{
	zCZoneMusic::Archive(arc);

	arc.WriteGroupBegin("DynaMusic");

	arc.WriteBool("enabled", enabled);
	arc.WriteInt("priority", priority);
	arc.WriteBool("ellipsoid", ellipsoid);
	arc.WriteFloat("reverbLevel", reverbLevel);
	arc.WriteFloat("volumeLevel", volumeLevel);
	arc.WriteBool("loop", loop);

	arc.WriteGroupEnd("DynaMusic");

	if (arc.InSaveGame()) {
		arc.WriteBool("local_enabled", local_enabled);
		arc.WriteBool("dayEntranceDone", dayEntranceDone);
		arc.WriteBool("nightEntranceDone", nightEntranceDone);
	}
}

void oCZoneMusic::Unarchive(zCArchiver& arc)
{
	zCZoneMusic::Unarchive(arc);

	arc.ReadBool("enabled", enabled);
	arc.ReadInt("priority", priority);
	arc.ReadBool("ellipsoid", ellipsoid);
	arc.ReadFloat("reverbLevel", reverbLevel);
	arc.ReadFloat("volumeLevel", volumeLevel);
	arc.ReadBool("loop", loop);

	if (arc.InSaveGame()) {
		arc.WriteBool("local_enabled", local_enabled);
		arc.WriteBool("dayEntranceDone", dayEntranceDone);
		arc.WriteBool("nightEntranceDone", nightEntranceDone);
	} else {
		local_enabled = enabled;
	}
}
