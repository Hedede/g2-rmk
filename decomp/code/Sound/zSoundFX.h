class zCSoundFX : public zCResource {
	Z_OBEJCT(zCSoundFX);
public:
	zCSoundFX() = default;
	virtual ~zCSoundFX() = default;

	void LoadResourceData() override;
	void ReleaseResourceData() override;
	void GetResSizeBytes() override;
	void GetCacheConfig(unsigned& timeout, unsigned& maxSize) override;

	virtual int GetNumChannels() { return 0 ;}
	virtual int GetNumChannelFrames(int) { return 0; }
	virtual void GetChannelFrame(int,int) {}
	virtual void SetChannelFrame(int,int) {}
	virtual float GetPlayingTimeMSEC() { return 0.0; }
	virtual float GetPan() { return 0.0; }
	virtual void SetPan(float) {}
	virtual float GetVolume() { return 0.0; }
	virtual void SetVolume(float) {}
	virtual void GetFrequency() { return 0; }
	virtual void SetFrequency(int) {}
	virtual void SetLooping(int) {}
	virtual void IsLooping() { return 0; }
	virtual void SetIsFixed(int) {}
	virtual int GetIsFixed() { return 0; }

private:
	float volume;
	float pan;
	int frequency;

	int unk;

	float timeStart;
};

void zCSoundFX::GetCacheConfig(unsigned& timeout, unsigned& size)
{
	static bool initialized = false;
	static unsigned sndcachetimeout;
	static unsigned sndcachesize;
	if ( !initialized ) {
		initialized = true;

		sndcachetimeout = zCOption::ReadDWord(zoptions, "ENGINE", "zSndCacheOutTimeMSec", 10000);
		static_sndcachesize = zCOption::ReadDWord(zoptions, "ENGINE", "zSndCacheSizeMaxBytes", 20000000);

		struct _MEMORYSTATUS Buffer;
		Buffer.dwLength = 32;
		GlobalMemoryStatus(&Buffer);
		auto mem = (Buffer.dwTotalPhys + 0xFFFFF) >> 20;
		if ( mem > 512 ) {
			sndcachesize *= 3;
			sndcachetimeout *= 3;
		} else if ( mem == 256 ) {
			static_sndcachesize *= 2;
			static_sndcachetimeout *= 2;
		}
	}

	timeout = sndcachetimeout;
	size = sndcachesize;
}
