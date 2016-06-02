class oCAICamera : public zCAIBase {
	Z_OBJECT(oCAICamera);
public:
	virtual ~oCAICamera() = default;
	void DoAI(zCVob* vob, int& vobDeleted) override;
	bool HasAIDetectedCollision() override
	{
		return false;
	}

private:
	zCTimer ai_timer; // not sure about name
};

void oCAICamera::DoAI(zCVob* vob, int& vobDeleted)
{
	vobDeleted = 0;
	float time = ztimer.frameTimeFloat * 0.001;
	if ( time == 0.0 )
		time = 0.02;

	float angle = 0.0;
	float angleLocal = 0.0;
	float z = 0.0;
	float y = 0.0;
	float x = 0.0;


	float tmp = 350.0 * time;  // movement
	float tmp2 = 150.0 * time; // rotation

	if ( zinput->KeyPressed(KEY_LSHIFT) || zinput->KeyPressed(KEY_RSHIFT) ) {
		tmp  *= 4;
		tmp2 *= 2;
	}

	if ( zinput->KeyPressed(KEY_UPARROW) ) {
		z = tmp;
	} else if ( zinput->KeyPressed(KEY_DOWNARROW) ) {
		z = -tmp;
	}

	if ( zinput->KeyPressed(KEY_LEFTARROW) ) {
		if ( zinput->KeyPressed(KEY_LMENU) )
			x = -tmp;
		else
			angle = -tmp2;
	}

	if ( zinput->KeyPressed(KEY_RIGHTARROW) ) {
		if ( zinput->KeyPressed(KEY_LMENU) )
			x = tmp;
		else
			angle = tmp2;
	}

	if ( zinput->KeyPressed(KEY_A) ) {
		y = tmp;
	} else if ( zinput->KeyPressed(KEY_Z) ) {
		y = -tmp;
	}

	if ( zinput->KeyPressed(KEY_PRIOR) ) {
		angleLocal = -tmp2;
	} else if ( zinput->KeyPressed(KEY_NEXT) ) {
		angleLocal = tmp2;
	}

	auto axis = zVEC3{0,1.0,0};
	vob->RotateWorld(axis, angle);

	axis = zVEC3{1.0,0,0};
	vob->RotateLocal(axis, angleLocal);
	vob->MoveLocal(x, y, z);
}
