class zCEventScreenFX : public zCEventMessage{
	Z_OBJECT(zCEventScreenFX);
public:
	enum zTEventScreenFXSubType {
		BLEND_FADE_IN,
		BLEND_FADE_OUT,
		CINEMA_FADE_IN,
		CINEMA_FADE_OUT,
		FOV_MORPH,
	};

	zCEventScreenFX(zTEventScreenFXSubType sub, float duration, zCOLOR color, float fovDeg);
	virtual ~zCEventScreenFX();

	virtual void Archive(zCArchiver& arc)
	{
		zCEventMessage::Archive(arc);
		arc.WriteFloat("duration", duration);
		arc.WriteColor("color", color);
		arc.WriteFloat("fovDeg", fovDeg);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCEventMessage::Archive(arc);
		arc.WriteFloat("duration", duration);
		arc.WriteColor("color", color);
		arc.WriteFloat("fovDeg", fovDeg);
	}

	virtual void MD_GetNumOfSubTypes();
	virtual zSTRING MD_GetSubTypeString(int type) const
	{
		switch ( type ) {
		case BLEND_FADE_IN:   return "BLEND_FADE_IN";
		case BLEND_FADE_OUT:  return "BLEND_FADE_OUT";
		case CINEMA_FADE_IN:  return "CINEMA_FADE_IN";
		case CINEMA_FADE_OUT: return "CINEMA_FADE_OUT";
		case FOV_MORPH:       return "FOV_MORPH";
		default: return "";
		}
	}

	virtual void MD_GetTimeBehavior();
	virtual void MD_GetMinTime();
	virtual void Pack(zCBuffer &,zCEventManager *);
	virtual void Unpack(zCBuffer &,zCEventManager *);

private:
	zREAL duration;
	zCOLOR color;
	zREAL fovDeg;
};
