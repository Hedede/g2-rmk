class zCMusicTheme {
	zCMusicTheme() = default;
	virtual ~zCMusicTheme() = default;
	virtual bool IsActive() { return false; }

private:
	zSTRING file;
	float vol;
	int loop;
	float reverbMix;
	float reverbTime;
	int transType;
	int transSubType;
	int unk;
	zSTRING name2;
};
