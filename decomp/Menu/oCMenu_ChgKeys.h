class oCMenu_ChgKeys : public zCMenu {
public:
	virtual void HandleEvent(int);
	virtual void Leave();
	virtual ~oCMenu_ChgKeys();
	virtual void ScreenInit();
	virtual void ExecCommand(zSTRING const&);
};
