struct zCMusicControler : zCEffect {
	Z_OBJECT(zCMusicControler);
public:
	zCMusicControler() = default;
	void ~zCMusicControler() override = default
	void OnMessage(zCEventMessage* msg, zCVob* sourceVob) override
	{
		if ( auto evt = dynamic_cast<zCEventMusicControler>(msg) ) {
			if ( evt->subType == 0 ) {
				zCZoneMusic::SetAutochange(0);
				zmusic->PlayThemeByScript(evt->fileName, 0, 0);
			}
			else if ( evt->subType == 1 ) {
				zCZoneMusic::SetAutochange(1);
			}
		}
	}
	void ThisVobRemovedFromWorld(zCWorld* wld) override
	{
		if ( !zCZoneMusic::SetAutochange(1) ) {
			zmusic->Stop();
			zWARNING("X: zCMusicControler stops music playback !");
		}
	}

private:
	// no data members
};
