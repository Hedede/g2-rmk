class oCSkyControler_Barrier : public zCSkyController_Outdoor {
public:
	void zCSkyControler_Outdoor::SetTime(float newTime)
	{
		lastRelightTime = lastRelightTime - newTime;
		masterTime = newTime;
		if ( fabs(lastRelightTime) > relightTime ) {
			++relightCtr;
			lastRelightTime = newTime;
		}
	}

private:
	struct zTRainFX {
		zCOutdoorRainFX*  outdoorRainFX;
		zTCamLocationHint camLocationHint;
		zREAL             outdoorRainFXWeight; // 0..1
		zREAL             soundVolume;         // 0..1
		zREAL             timerInsideSectorCantSeeOutside;// msec
		zREAL             timeStartRain;
		zREAL             timeStopRain;
		zBOOL             renderLightning;
		zBOOL             m_bRaining;
		//Anzahl der bisherigen Regenperioden.
		int               m_iRainCtr;
	} rainfx;


	oCBarrier* barrier;
	zBOOL bFadeInOut;
};

struct myThunder {
	myThunder() = default;
	char unk[0x4C];
};

/* Ist in Gothic 2 fürs Rendern von Wetter-Blitzen verantwortlich
 * hier heran kommt man über MEM_SkyController.barrier */
class oCBarrier {
	zCMesh*     skySphereMesh;

	myPoly*      myPolyList;
	myVert*      myVertList;

	int numMyVerts;
	int numMyPolys;

	myThunder*   myThunderList;
	int          numMaxThunders;
	int          numMyThunders;

	int actualIndex;
	int rootBoltIndex;

	int startPointList1[10];
	int numStartPoints1;
	int startPointList2[10];
	int numStartPoints2;
	int startPointList3[10];
	int numStartPoints3;
	int startPointList4[10];
	int numStartPoints4;

	int topestPoint;

	zBOOL bFadeInOut;
	int fadeState;

	zBOOL fadeIn;
	zBOOL fadeOut;
	zBOOL bFlashing;
	float fFlashTime;


	zCSoundFX*                sfx1;
	zTSoundHandle             sfxHandle1;
	zCSoundFX*                sfx2;
	zTSoundHandle             sfxHandle2;
	zCSoundFX*                sfx3;
	zTSoundHandle             sfxHandle3;
	zCSoundFX*                sfx4;
	zTSoundHandle             sfxHandle4;

	zCDecal*               thunderStartDecal;

	zBOOL 		       activeThunder_Sector[4];

	zVEC2*                 originalTexUVList;
};
