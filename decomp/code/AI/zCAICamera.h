class zCAICamera : public zCAIBase {
	Z_OBJECT(zCAICamera);

public:
	virtual ~zCAICamera();
	virtual void DoAI(zCVob *,int &);

	static void GlobalStartUp() {}

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
	int unki1;
	zSTRING __prevMode;
	char unkc3[12];
	float dialogCamDuration;
	int __dialogCam[6];
	zSTRING unkstr1;
	int __num_checkunderwater;

	zBOOL underwaterFX;

	int unkc5;
	zSTRING unkstr2;
	int unki2;
	zCVob *__helper;
	zCVob *__hlp_array[80];
	zCPathSearch *__pathSearch;
	zCMovementTracker *__tracker;
	zSPathSearchResult *__psreport;
	zSTRING __instanceName;
	float unkf1[2];
	zVEC3 __point;
	char unkc7[8];
};

//------------------------------------------------------------------------------
//_Carsten/zAICamera.cpp

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
			camCon->EditInstance(__instanceName, &scripted);
			v12 = (struct zSTRING *)operator new(0xDCu);
			msg = v12;
			LOBYTE(v47) = 5;
			if ( v12 )
				v13 = zCConsole::zCConsole((zCConsole *)v12);
			else
				v13 = 0;
			LOBYTE(v47) = 0;
			camCon = v13;
			zCConsole::SetPos(v13, 0, 0);
			zCConsole::SetParser(camCon, camParser);
			zCConsole::SetChangedFunc(camCon, CamConsole_ParameterChanged);
			zCConsole::EditInstance(camCon, &_this->__instanceName, &_this->bestRange);
			zCConsole::SetFocus(camCon);
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
