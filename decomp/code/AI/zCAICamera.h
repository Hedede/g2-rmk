class zCAICamera : public zCAIBase {
	Z_OBJECT(zCAICamera);

public:
	static void GlobalStartUp() {}
	static zCAICamera* GetCurrent();
	static zCAICamera* Create();

	virtual ~zCAICamera();
	virtual void DoAI(zCVob *,int &);

	void CleanUp();

	int CreateInstance(zSTRING &instName);

	void ReceiveMsg( zTAICamMsg const& msg );

	zSTRING GetMode() const { return __modeInstance; }
	zSTRING GetPreviousMode() const { return __prevMode; }
	zBOOL IsModeAvailable(const zSTRING& modeInst) const
	{
		return return camParser->GetIndex(modeInst) >= 0;
	}
	zBOOL IsModeActive(const zSTRING& modeInst) const
	{
		return GetMode() == modeInst;
	}

	void SetRotateEnabled(int enabled)
	{
		__tracker->SetRotateEnabled(enabled);
	}

	void SetDialogCamDuration(float dur)
	{
		dialogCamDuration = dur;
	}

private
	int __showControlDots;
	int unkc1;

	struct {
		float bestRange;
		float minRange;
		float maxRange;
		float bestElevation;
		float minElevation;
		float maxElevation;
		float bestAzimuth;
		float minAzimuth;
		float maxAzimuth;
		float bestRotZ;
		float minRotZ;
		float maxRotZ;
		float rotOffsetX;
		float rotOffsetY;
		float rotOffsetZ;
		float targetOffsetX;
		float targetOffsetY;
		float targetOffsetZ;
		float veloTrans;
		float veloRot;
		int translate;
		int rotate;
		int collision;
	} scripted;

	int unki0;

	zCArray<zCVob*> __targetlist;
	zCVob *__csVob;
	zCVob *__target;

	float __ghostAlpha;
	int __focusCount; // unki1
	zSTRING __prevMode;
	char unkc3[12];
	float dialogCamDuration;
	zCCSCamera* __dialogCam[6];
	zSTRING unkstr1;
	int __raysCount;

	zBOOL underwaterFX;

	int unkc5;
	zSTRING unkstr2;
	int unki2;
	zCVob *__helper;
	zCVob *__hlp_array[80];
	zCPathSearch *__pathSearch;
	zCMovementTracker *__tracker;
	zSPathSearchResult *__psreport;
	zSTRING __modeInstance;
	float __minRange;
	float __maxRange;
	zVEC3 __point;
	int __unkBool;
	int hintTargetInPortalRoom;
};

//------------------------------------------------------------------------------
//_Carsten/zAICamera.cpp
zCAICamera* zCAICamera::GetCurrent()
{
	return zCAICamera::current;
}

zCAICamera* zCAICamera::Create()
{
	if ( !zCAICamera::bCreated ) {
		zCAICamera::current  = new zCAICamera();
		zCAICamera::bCreated = 1;
	}
	return GetCurrent();
}


int zCAICamera::CreateInstance(zSTRING &instName)
{
	return camParser->CreateInstance(instName, &scripted);
}

void zCAICamera::ReceiveMsg(const zTAICamMsg& msg)
{
	// I have no idea why it would check for 0x4 (moved backward)
	// maybe it has more then one meaning?
	if ( msg == TARGET_CHANGED || msg == TARGET_MOVED_BACKWARD )
		__tracker->ReceiveMsg(msg);
}

void zCAICamera::CleanUp()
{
	RELEASE(__helper);
}

void zCAICamera::SetTarget(zCVob *t)
{
	zASSERT( t, "C: zAICamera (zCAICamera :: SetTarget) : No target specified" ); // 508
	//TODO
}

char const* camsys_ver = "v0.5";

int CamConsole_EvalFunc(const zSTRING& in, zSTRING& msg)
{
	return zCAICamera::current->Console_EvalFunc(in, msg);
}

static int firstTime = 1;
void zCAICamera::StartUp()
{
	zINFO( "C: *** camera system " + camsys_ver "..." );
	if ( !camParser ) {
		camParser = new zCParser; // 265 (in g1d)
		zoptions->ChangeDir(DIR_SCRIPTS);
	
		zSTRING camscript = "";
		if ( zgameoptions )
			camscript = zgameoptions->ReadString( zOPT_SEC_FILES, "Camera", 0);
		if (!camscript)
			camscript = "System\\Camera";

		zINFO(4, "N: CAM: Loading file " + camscript + ".src or .dat"); // 337

		camParser.Parse( camscript + ".src" );
		camParser.CreatePCode();
		camParser.CheckClassSize( "CCAMSYS", 92 );

		zCAICamera::StartupDialogCam();

		zcon.Register("EDIT CAMERA",          "Camerainstanz editieren ");
		zcon.Register("TOGGLE NOCLIP",        "Toggles collision of camera with world ");
		zcon.Register("TOGGLE CAMDEBUG",      "Toggles camera debug");
		zcon.Register("TOGGLE INERTIATARGET", "Toggles averaging of target position (for camera)");

		zcon.AddEvalFunc( CamConsole_EvalFunc );

		this->__helper = new zCVob; // 281 g1d
		flags3.0x10 = 1;

		firstTime = 0;
	}
}

int zCAICamera::Console_EvalFunc(const zSTRING& in, zSTRING& msg)
{
	zSTRING line = in; line.Upper();
	auto word1 = line.PickWord(1, " ", zSTR_SKIP);
	if (word1 == "EDIT") {
		auto word2 = line.PickWord(2, " ", zSTR_SKIP);
		if (word2 == "CAMERA") {
			zcon.Hide();
			if (camCon)
				delete camCon;
			camCon = new zCConsole(); // 542 g1d
			camCon->SetPos(0, 0);
			camCon->SetParser(camParser);
			camCon->SetChangedFunc(CamConsole_ParameterChanged);
			camCon->EditInstance(__modeInstance, &scripted);
			camCon->SetFocus();
			return 1;
		}
		return 0;
	}
	if (word1 == "TOGGLE") {
		if (line.PickWord(2, " ") == "NOCLIP") {
			Toggle(__pathSearch->__collisionEnabled);
			if ( __pathSearch->__collisionEnabled )
				msg = "Camera collision enabled";
			else
				msg = "Camera collision disabled";
			return 1;
		}
		if (line.PickWord(2, " ") == "CAMDEBUG") {
			Toggle(__showControlDots);
			Toggle(showDebug);

			if (__showControlDots) {
				InitHelperVobs();
				if (__target) {
					__target->flags1.0x4 = 1;
					__target->visualAlpha = 0.1;
				}

				msg = "visualizing camera control dots enabled";
			}
			else
			{
				D_ClrWin();
				DeleteHelperVobs();
				if (__target) {
					__target->flags1.0x4 = 0;
					__target->visualAlpha = 1.0;
				}

				msg = "visualizing camera control dots disabled";
			}
			return 1;
		}
		if (line.PickWord(2, " ") == "INERTIATARGET") {
			if (__tracker->__targetInertia == 1) {
				__tracker->__targetInertia = 10;
				__tracker->SetPlayerPos(__tracker->playerPos);
				msg = "target trägheit an";
			} else {
				__tracker->__targetInertia = 1;
				__tracker->SetPlayerPos(__tracker->playerPos);
				msg = "target trägheit aus";
			}
			return 1;
		}
		return 0;
	}
	return 0;
}

void zCAICamera::ShowDebug()
{
	__helper->SetTrafo( __tracker->__mat0 );

	__hlp_array[0]->SetPositionWorld(__tracker->__vec2);
	__hlp_array[1]->SetPositionWorld(__tracker->__vec3);
	__hlp_array[2]->SetPositionWorld(__tracker->__vec1);

	auto lastWp = __tracker->GetLastValidWayPoint( 0 );

	__hlp_array[3]->SetPositionWorld( lastWp );

	auto playerPos     = "PlayerPos: " + __tracker->playerPos.GetString() + "\n"; // was inlined
	auto targetVelo    = "target velo     : " + __tracker->__targetVelo + "\n";
	auto rangeToPlayer = "Range To Player : " + __tracker->GetRange() + "\n";

	auto msg = __tracker->__msg;

	zSTRING movement;
	if (msg & TARGET_STAND) {
		movement = "TARGET_STAND";
	} else {
		if ( msg & TARGET_MOVED_LEFT) {
			movement = "TARGET_MOVED_LEFT";
		} else if ( msg & TARGET_MOVED_RIGHT ) {
			movement = "TARGET_MOVED_RIGHT";
		}
		if ( msg & TARGET_MOVED_FORWARD ) {
			movement += " | " + "TARGET_MOVED_FORWARD";
		} else if ( msg & TARGET_MOVED_BACKWARD ) {
			movement += " | " + "TARGET_MOVED_BACKWARD";
		}

		if ( msg & TARGET_MOVED_UP ) {
			movement += " | " + "TARGET_MOVED_UP";
		} else if ( msg & TARGET_MOVED_DOWN ) {
			movement += " | " + "TARGET_MOVED_DOWN";
		}
	}
	movement += "\n";

	zSTRING rotation;
	if ( msg & target_rot_none ) {
		rotation = "TARGET_ROT_NONE";
	} else if ( msg & TARGET_ROTATED_LEFT ) {
		rotation = "TARGET_ROTATED_LEFT";
	} else if ( msg & "TARGET_ROTATED_RIGHT" ) {
		rotation = "TARGET_ROTATED_RIGHT";
	}
	rotation += "\n";

	auto azi  =  " Azimuth   : " + __tracker->GetAzimuth();
	auto elev =  " Elevation : " + __tracker->GetElevation();

	zSTRING range;
	if ( GetMaxRange() * 100.0 < __tracker->GetRange() ) {
		range = "Player out of maxRange\n";
	} else if ( GetMaxRange() * 100.0 > __tracker->GetRange() ) {
		range = "Player out of minRange\n";
	} else {
		range = "Player in Range\n";
	}

	zSTRING camPos;
	auto pos = __csVob->GetPositionWorld();
	if ( __pathSearch->IsPointValid( pos, 4, failReasonDummy) ) {
		camPos = "CamPos valid\n";
	} else {
		camPos = "CamPos not valid\n";
	}

	zSTRING npcs = "NPC/focus count:\n" + __focusCount;
	zSTRING mode = "ModeName:" + GetMode();
	zSTRING rays = "Rays casted:" + __raysCount;

	screen->Print(0, 3000, playerPos);
	screen->Print(0, 3300, targetVelo);
	screen->Print(0, 4200, rangeToPlayer);
	screen->Print(0, 4500, movement);
	screen->Print(0, 4800, rotation);
	screen->Print(0, 5100, "");
	screen->Print(0, 5400, azi);
	screen->Print(0, 5700, elev);
	screen->Print(0, 6000, range);
	screen->Print(0, 6300, camPos);
	screen->Print(0, 6600, npcs);
	screen->Print(0, 6900, mode);
	screen->Print(0, 2700, rays);
}
