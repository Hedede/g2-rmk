enum zGAMEKEY {
	GAMEKEY_UNDEFINED = 0x0,
	GAMEKEY_LEFT = 0x1,
	GAMEKEY_RIGHT = 0x2,
	GAMEKEY_UP = 0x3,
	GAMEKEY_DOWN = 0x4,
	GAMEKEY_ACTION = 0x5,
	GAMEKEY_SLOW = 0x6,
	GAMEKEY_WEAPON = 0x8,
	GAMEKEY_SMOVE = 0xB,
	GAMEKEY_LOOK_FP = 0xD,
	GAMEKEY_END = 0xE,
	GAMEKEY_INVENTORY = 0xF,
	GAMEKEY_LOOK = 0x10,
	GAMEKEY_SNEAK = 0x11,
	GAMEKEY_STRAFELEFT = 0x12,
	GAMEKEY_STRAFERIGHT = 0x13,
	GAMEKEY_SCREEN_STATUS = 0x14,
	GAMEKEY_SCREEN_LOG = 0x15,
	GAMEKEY_SCREEN_MAP = 0x16,
	GAMEKEY_LOCK_TARGET = 0x18,
	GAMEKEY_PARADE = 0x19,
	GAMEKEY_ACTIONLEFT = 0x1A,
	GAMEKEY_ACTIONRIGHT = 0x1B,
	GAMEKEY_LAME_POTION = 0x1C,
	GAMEKEY_LAME_HEAL = 0x1D,
};

struct zSKeyMapping {
	zSKeyMapping() = default;
	zSKeyMapping(uint16_t id) : id(id) {}

	uint16_t id;
	zCArray<uint16_t> keys;
	int unk;
};

class zCInput {
public:
	zCInput();
	virtual void ~zCInput();
	virtual float GetState(ushort)
	{
		return 0.0;
	}
	virtual void SetState(ushort,int) {}
	virtual int GetToggled(ushort)
	{
		return 1;
	}

	virtual void SetDeviceEnabled(zTInputDevice,int) {}
	virtual int GetDeviceEnabled(zTInputDevice)
	{
		return 1;
	}
	virtual int GetDeviceConnected(zTInputDevice)
	{
		return 1;
	}

	virtual int KeyPressed(int)
	{
		return 0;
	}
	virtual int KeyToggled(int)
	{
		return 0;
	}
	virtual int AnyKeyPressed()
	{
		return 0;
	}

	virtual void ResetRepeatKey(int) {}

	virtual uint16_t GetKey(int,int)
	{
		return 0;
	}
	virtual void SetKey(int,int) {}

	virtual char GetChar()
	{
		return 0;
	}

	virtual void ClearKeyBuffer() {}

	virtual int GetNumJoysConnected()
	{
		return 0;
	}

	virtual void SetJoyDigitalEmu(int) {}
	virtual void SetJoyEnabled(int) {}

	virtual float JoyState(int,int)
	{
		return 0.0;
	}
	virtual float JoyState(int)
	{
		return 0.0;
	}

	virtual void GetMousePos(float &,float &,float &) {}

	virtual int GetMouseButtonPressedLeft()
	{
		return 0;
	}
	virtual int GetMouseButtonPressedMid()
	{
		return 0;
	}
	virtual int GetMouseButtonPressedRight()
	{
		return 0;
	}

	virtual void SetMouseSensitivity(float,float) {}
	virtual void GetMouseSensitivity(float &,float &) {}

	virtual int GetMouseIdle()
	{
		return 0;
	}

	virtual void SetMouseFlipXY(int,int) {}
	virtual void GetMouseFlipXY(int &,int &) {}

	virtual void ProcessInputEvents() = 0;

	void BindOption(zSTRING name, uint16_t value, zCArray<uint16_t> array);
	void BindKeys(bool alternate_keys);

private:
	zCArraySort<zSKeyMapping*> mappings;
};


//------------------------------------------------------------------------------
zCInput::zCInput()
{
	mappings.Compare = zCInput::CompareKeyMaps;
	zCInput::InitializeControlValues();
}

void zCInput::BindOption(zSTRING name, uint16_t value, zCArray<uint16_t> keyList)
{
	uint16_t* keys;

	uint16_t* array = &keyList[0];
	size_t arr_size = sizeof(uint16_t) * array.NumInList();

	size_t size = zoptions->ReadRaw(zOPT_SEC_KEYS, name, keys, array, arrsize);
	size /= sizeof(uint16_t);

	keyList.DeleteList();

	for ( size_t i = 0; i < size; ++i ) {
		if (keys[i])
			keyList.InsertEnd(keys[i]);
	}

	int found = mappings.Search(zSKeyMapping(value));
	if (found != -1) {
		auto mapping = mappings[found];
		mappings.RemoveOrderIndex(found);
		if (mapping)
			delete mapping;
	}

	auto mapping = new zSKeyMapping;
	mapping->id = value;
	mapping->keys = keyList;
	mapping->unk = 0;
	mappings.InsertSort(mapping);

	delete[] keys;
}

void zCInput::BindKeys(bool alternate_keys)
{
	// GAMEKEY_UP
	zCArray<uint16_t> keyList;

	keyList.InsertEnd(KEY_UPARROW);
	if ( alternate_keys )
		keyList.InsertEnd(KEY_W);
	else
		keyList.InsertEnd(KEY_HOME);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_UP, keyList); // keyUp

	BindOption(ZOPT_GAMEKEY_UP, GAMEKEY_UP, keyList);



	// GAMEKEY_DOWN
	keyList.DeleteList();
	keyList.InsertEnd(KEY_DOWNARROW);
	if ( alternate_keys )
		keyList.InsertEnd(KEY_S);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_DOWN, keyList);

	BindOption(ZOPT_GAMEKEY_DOWN, GAMEKEY_DOWN, keyList);


	// GAMEKET_TURN_LEFT
	keyList.DeleteList();
	if ( alternate_keys ) {
		keyList.InsertEnd(KEY_LEFTARROW);
		keyList.InsertEnd(KEY_Q);
	}
	else
	{
		keyList.InsertEnd(KEY_INSERT);
	}

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_LEFT, keyList);

	BindOption(ZOPT_GAMEKEY_LEFT, GAMEKEY_LEFT, keyList);


	// GAMEKEY_TURN_RIGHT
	keyList.DeleteList();
	if ( alternate_keys )
	{
		keyList.InsertEnd(KEY_RIGHTARROW);
		keyList.InsertEnd(KEY_E);
	}
	else
	{
		keyList.InsertEnd(KEY_PRIOR);
	}

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_RIGHT, keyList);

	BindOption(ZOPT_GAMEKEY_RIGHT, GAMEKEY_RIGHT, keyList);


	// GAMEKEY_STRAFE_LEFT
	keyList.DeleteList();
	if ( alternate_keys )
	{
		keyList.InsertEnd(KEY_DELETE);
		keyList.InsertEnd(KEY_A);
	}
	else
	{
		keyList.InsertEnd(KEY_LEFTARROW);
	}

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_STRAFELEFT, keyList);

	BindOption(ZOPT_GAMEKEY_STRAFELEFT, GAMEKEY_STRAFELEFT, keyList);


	// GAMEKEY_STRAFE_RIGHT
	keyList.DeleteList();
	if ( alternate_keys )
	{
		keyList.InsertEnd(KEY_NEXT);
		keyList.InsertEnd(KEY_D);
	}
	else
	{
		keyList.InsertEnd(KEY_RIGHTARROW);
	}

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_STRAFERIGHT, keyList);

	BindOption(ZOPT_GAMEKEY_STRAFERIGHT, GAMEKEY_STRAFERIGHT, keyList);


	// GAMEKEY_ACTION
	keyList.DeleteList();
	keyList.InsertEnd(KEY_LCONTROL);
	keyList.InsertEnd(MOUSE_BUTTONLEFT);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_ACTION, keyList);

	BindOption(ZOPT_GAMEKEY_ACTION, GAMEKEY_ACTION, keyList);


	// GAMEKEY_SLOW
	keyList.DeleteList();
	keyList.InsertEnd(KEY_LSHIFT);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_SLOW, keyList);

	BindOption(ZOPT_GAMEKEY_SLOW, GAMEKEY_SLOW, keyList);


	// GAMEKEY_SMOVE
	keyList.DeleteList();
	keyList.InsertEnd(KEY_LMENU);
	keyList.InsertEnd(KEY_RMENU);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_SMOVE, keyList);

	BindOption(ZOPT_GAMEKEY_SMOVE, GAMEKEY_SMOVE, keyList);


	// GAMEKEY_WEAPON
	keyList.DeleteList();
	keyList.InsertEnd(KEY_SPACE);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_WEAPON, keyList);

	zSTRING::zSTRING(&bindname, &ZOPT_GAMEKEY_WEAPON);
	BindOption(ZOPT_GAMEKEY_WEAPON, GAMEKEY_WEAPON, keyList);


	// GAMEKEY_SNEAK
	keyList.DeleteList();
	if ( alternate_keys )
		keyList.InsertEnd(KEY_X);
	else
		keyList.InsertEnd(KEY_A);

	keyList.InsertEnd(KEY_MULTIPLY);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_SNEAK, keyList);

	BindOption(ZOPT_GAMEKEY_SNEAK, GAMEKEY_SNEAK, keyList);


	// GAMEKEY_LOOK
	keyList.DeleteList();
	keyList.InsertEnd(KEY_NUMPAD0);
	keyList.InsertEnd(KEY_R);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_LOOK, keyList);

	BindOption(GAMEKEY_LOOK, ZOPT_GAMEKEY_LOOK, keyList);


	// GAMEKEY_LOOK_FP
	keyList.DeleteList();
	keyList.InsertEnd(KEY_DECIMAL);
	keyList.InsertEnd(KEY_F);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_LOOK_FP, keyList);

	BindOption(ZOPT_GAMEKEY_LOOK_FP, GAMEKEY_LOOK_FP, keyList);

	// GAMEKEY_END
	keyList.DeleteList();
	keyList.InsertEnd(KEY_ESCAPE);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_END, keyList);

	BindOption(ZOPT_GAMEKEY_END, GAMEKEY_END, keyList);

	// GAMEKEY_INVENTORY
	keyList.DeleteList();
	keyList.InsertEnd(KEY_TAB);
	keyList.InsertEnd(KEY_BACK);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_INVENTORY, keyList);

	BindOption(ZOPT_GAMEKEY_INVENTORY, GAMEKEY_INVENTORY, keyList);

	// GAMEKEY_SCREEN_STATUS
	keyList.DeleteList();
	if ( !alternate_keys )
		keyList.InsertEnd(KEY_S);
	keyList.InsertEnd(KEY_B);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_SCREEN_STATUS, keyList);

	BindOption(ZOPT_GAMEKEY_SCREEN_STATUS, GAMEKEY_SCREEN_STATUS, keyList);

	// GAMEKEY_SCREEN_LOG
	keyList.DeleteList();
	if ( !alternate_keys )
		keyList.InsertEnd(KEY_L);
	keyList.InsertEnd(KEY_N);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_SCREEN_LOG, keyList);

	BindOption(ZOPT_GAMEKEY_SCREEN_LOG, GAMEKEY_SCREEN_LOG, keyList);


	// GAMEKEY_SCREEN_MAP
	keyList.DeleteList();
	keyList.InsertEnd(KEY_M);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_SCREEN_MAP, keyList);

	BindOption(ZOPT_GAMEKEY_SCREEN_MAP, GAMEKEY_SCREEN_MAP, keyList);

	// GAMEKEY_ACTIONLEFT
	keyList.DeleteList();
	keyList.InsertEnd(KEY_DELETE);
	keyList.InsertEnd(KEY_PERIOD);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_ACTIONLEFT, keyList);

	BindOption(ZOPT_GAMEKEY_ACTIONLEFT, GAMEKEY_ACTIONLEFT, keyList);


	// GAMEKEY_ACTIONRIGHT
	keyList.DeleteList();
	keyList.InsertEnd(KEY_NEXT);
	keyList.InsertEnd(KEY_LBRACKET);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_ACTIONRIGHT, keyList);

	BindOption(ZOPT_GAMEKEY_ACTIONRIGHT, GAMEKEY_ACTIONRIGHT, keyList);


	// GAMEKEY_PARADE
	keyList.DeleteList();

	keyList.InsertEnd(MOUSE_BUTTONRIGHT);
	keyList.InsertEnd(KEY_END);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_PARADE, keyList);

	BindOption(ZOPT_GAMEKEY_PARADE, GAMEKEY_PARADE, keyList);


	// GAMEKEY_LOCK_TARGET
	keyList.DeleteList();
	keyList.InsertEnd(KEY_RETURN);
	keyList.InsertEnd(KEY_Y);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_LOCK_TARGET, keyList);

	BindOption(ZOPT_GAMEKEY_LOCK_TARGET, GAMEKEY_LOCK_TARGET, keyList);


	// GAMEKEY_LAME_POTION
	keyList.DeleteList();
	keyList.InsertEnd(KEY_P);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_LAME_POTION, keyList);

	BindOption(ZOPT_GAMEKEY_LAME_POTION, GAMEKEY_LAME_POTION, keyList);


	// GAMEKEY_LAME_HEAL
	keyList.DeleteList();
	keyList.InsertEnd(KEY_H);

	_GetDefaultBinding(alternate_keys, ZOPT_GAMEKEY_LAME_HEAL, keyList);

	BindOption(ZOPT_GAMEKEY_LAME_HEAL, GAMEKEY_LAME_HEAL, keyList);

	keyList.DeleteList();
}
