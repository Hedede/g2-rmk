class zCTouchAnimateSound : public zCTouchAnimate {
	Z_OBJECT(zCTouchAnimateSound);
public:
	virtual ~zCTouchAnimateSound();

	virtual void Archive(zCArchiver& arc);
	{
		zCTouchAnimate::Archive(arc);
		arc.WriteString("sfxTouch", sfxTouch);
	}

	virtual void Unarchive(zCArchiver& arc)
	{
		zCTouchAnimate::Unarchive(arc);
		arc.ReadString("sfxTouch", sfxTouch);
	}

	virtual zSTRING GetSoundName() const
	{
		return sfxTouch;
	}

private:
	zSTRING sfxTouch;
};
