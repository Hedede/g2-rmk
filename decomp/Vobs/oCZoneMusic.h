class oCZoneMusic : public zCZoneMusic {
	Z_OBJECT(oCZoneMusic);

/* from Ikarus pack:
 * pointer that points to the current MusicZone.
   poorly tested, was null most of the time, don't know why. */
	static oCZoneMusic* s_musiczone;
public:
	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;

	virtual ~oCZoneMusic() = default;

	virtual void ProcessZoneList(zCArraySort<zCZone *> const &,zCArraySort<zCZone *> const &,zCWorld *);
	virtual void GetDefaultZoneClass();
	virtual void GetDebugDescString();
	virtual void OnTrigger(zCVob *);
	virtual void OnUntrigger(zCVob *);

protected:
	static int IsDaytime()
	{
		return ogame->GetWorldTimer()->IsDay();
	}

private:
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
	virtual ~oCZoneMusicDefault() = default;
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
