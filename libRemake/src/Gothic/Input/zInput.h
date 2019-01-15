#pragma once
#include <Hook/Externals.h>
#include <Gothic/Types/zArray.h>

struct zSKeyMapping {
	uint16_t id;
	zCArray<uint16_t> keys;
	int unk;
};

enum zTInputDevice {
	DEVICE_KEYBOARD = 1,
	DEVICE_MOUSE = 2,
	DEVICE_JOYSTICK = 3
};

struct zCInput;
struct zCInput_vt {
	void (__thiscall *dtor)(zCInput *);
	float (__thiscall *GetState)(zCInput *, unsigned short);
	void (__thiscall *SetState)(zCInput *, unsigned short, int);
	int (__thiscall *GetToggled)(zCInput *, unsigned short);
	void (__thiscall *SetDeviceEnabled)(zCInput *, zTInputDevice, int);
	void (__thiscall *GetDeviceEnabled)(zCInput *, zTInputDevice);
	int (__thiscall *GetDeviceConnected)(zCInput *, zTInputDevice);/*
	int (__thiscall *KeyPressed)(zCInput *, int);
	int (__thiscall *KeyToggled)(zCInput *, int);
	void (__thiscall *AnyKeyPressed)(zCInput *);
	void (__thiscall *ResetRepeatKey)(zCInput *, int);
	ushort (__thiscall *GetKey)(zCInput *, int, int);
	void (__thiscall *SetKey)(zCInput *, int, int);
	void (__thiscall *GetChar)(zCInput *);
	void (__thiscall *ClearKeyBuffer)(zCInput *);
	void (__thiscall *GetNumJoysConnected)(zCInput *);
	void (__thiscall *SetJoyDigitalEmu)(zCInput *, int);
	void (__thiscall *SetJoyEnabled)(zCInput *, int);
	void (__thiscall *JoyState1)(zCInput *, int, int);
	void (__thiscall *JoyState2)(zCInput *, int);
	void (__thiscall *GetMousePos)(zCInput *, float *, float *, float *);
	void (__thiscall *GetMouseButtonPressedLeft)(zCInput *);
	void (__thiscall *GetMouseButtonPressedMid)(zCInput *);
	void (__thiscall *GetMouseButtonPressedRight)(zCInput *);
	void (__thiscall *SetMouseSensitivity)(zCInput *, float, float);
	void (__thiscall *GetMouseSensitivity)(zCInput *, float *, float *);
	void (__thiscall *GetMouseIdle)(zCInput *);
	void (__thiscall *SetMouseFlipXY)(zCInput *, int, int);
	void (__thiscall *GetMouseFlipXY)(zCInput *, int *, int *);
	void (__thiscall *ProcessInputEvents)(zCInput *);*/
};


struct zCInput {
	zCInput()
	{
		vtab = reinterpret_cast<void*>(0x830AD4);
		Cdecl<void()>{0x4CC640}();
		mappings.Compare = +[] (zSKeyMapping* const& a, zSKeyMapping* const& b)
		{
			return a->id - b->id;
		};
	}

	bool GetDeviceConnected(zTInputDevice dev)
	{
		return reinterpret_cast<zCInput_vt*>(vtab)->GetDeviceConnected(this,dev);
	}

	void SetDeviceEnabled(zTInputDevice dev, int b)
	{
		reinterpret_cast<zCInput_vt*>(vtab)->SetDeviceEnabled(this,dev,b);
	}


private:
	void* vtab;
	zCArraySort<zSKeyMapping*> mappings;
};

struct zCInput_Win32 {
	zCInput_Win32(void* hwnd)
	{
		Thiscall<void(zCInput_Win32*, void*)>{0x4D4A10}(this, hwnd);
	}
	char data[0x58];
};

extern zCInput*& zinput;
