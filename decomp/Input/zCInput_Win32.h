class zCInput_Win32 : public zCInput {
public:
	virtual void ~zCInput_Win32();
	virtual void GetState(ushort);
	virtual void SetState(ushort,int);
	virtual void GetToggled(ushort);
	virtual void SetDeviceEnabled(zTInputDevice,int);
	virtual void GetDeviceEnabled(zTInputDevice);
	virtual void GetDeviceConnected(zTInputDevice);
	virtual void KeyPressed(int);
	virtual void KeyToggled(int);
	virtual void AnyKeyPressed();
	virtual void ResetRepeatKey(int);
	virtual void GetKey(int,int);
	virtual void SetKey(int,int);
	virtual void GetChar();
	virtual void ClearKeyBuffer();
	virtual void GetMousePos(float &,float &,float &);
	virtual void GetMouseButtonPressedLeft();
	virtual void GetMouseButtonPressedMid();
	virtual void GetMouseButtonPressedRight();
	virtual void SetMouseSensitivity(float,float);
	virtual void GetMouseSensitivity(float &,float &);
	virtual void GetMouseIdle();
	virtual void SetMouseFlipXY(int,int);
	virtual void GetMouseFlipXY(int &,int &);
	virtual void ProcessInputEvents();
};


