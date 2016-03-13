class oCBinkPlayer : public zCBinkPlayer {
public:
	virtual ~oCBinkPlayer();
	virtual void OpenVideo(zSTRING);
	virtual void PlayInit(int);
	virtual void PlayFrame();
	virtual void PlayDeinit();
	virtual void PlayHandleEvents();
};
