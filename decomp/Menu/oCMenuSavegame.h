class oCMenuSavegame : public zCMenu {
public:
	virtual void HandleEvent(int);
	virtual void Activate();
	virtual ~oCMenuSavegame();
	virtual void ScreenInit();
	virtual void ScreenDone();
	virtual void HandleSlotChange(int);
	virtual void GetSelectedSlot();
};
