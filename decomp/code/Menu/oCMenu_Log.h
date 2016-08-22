class oCMenu_Log : public zCMenu {
public:
	virtual void HandleEvent(int);
	virtual void HandleEnterItem(zCMenuItem *);
	virtual void HandleLeaveItem(zCMenuItem *);
	virtual void HandleActivateItem(zCMenuItem *);
	virtual void Activate();
	virtual ~oCMenu_Log();
	virtual void ScreenInit();
	virtual void ScreenDone();
	virtual void ExecCommand(zSTRING const&);

	void InitForDisplay() {}
};
