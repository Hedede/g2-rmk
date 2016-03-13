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
		int               m_iRainCtr; //Anzahl der bisherigen Regenperioden.
	} rainfx;


	oCBarrier * barrier;
	zBOOL bFadeInOut;
};

/* Ist in Gothic 2 fürs Rendern von Wetter-Blitzen verantwortlich
 * hier heran kommt man über MEM_SkyController.barrier */

class oCBarrier {
	zCMesh*     skySphereMesh;             //

	myPoly*      myPolyList;	           
	myVert*      myVertList;                

	int numMyVerts;                //int        
	int numMyPolys;                //int        

	myThunder*   myThunderList;             //
	int          numMaxThunders;            //
	int          numMyThunders;             //

	int actualIndex;               //int        
	int rootBoltIndex;             //int        

	int startPointList1[10];       //int        
	int numStartPoints1;           //int        
	int startPointList2[10];       //int        
	int numStartPoints2;           //int        
	int startPointList3[10];       //int        
	int numStartPoints3;           //int        
	int startPointList4[10];       //int        
	int numStartPoints4;           //int        

	int topestPoint;               //int        

	zBOOL bFadeInOut;                //zBOOL      
	int fadeState;                 //int		

	zBOOL fadeIn;                    //zBOOL      
	zBOOL fadeOut;                   //zBOOL      
	zBOOL bFlashing;                 //zBOOL	     
	float fFlashTime;                //float	     


	zCSoundFX*                sfx1;                      //
	zTSoundHandle             sfxHandle1;                //
	zCSoundFX*                sfx2;                      //
	zTSoundHandle             sfxHandle2;                //
	zCSoundFX*                sfx3;                      //
	zTSoundHandle             sfxHandle3;                //
	zCSoundFX*                sfx4;                      //
	zTSoundHandle             sfxHandle4;                //
                                  
	zCDecal*                  thunderStartDecal;         //
                                  
	zBOOL 		       activeThunder_Sector[4];     //
                                  
	zVEC2*                    originalTexUVList;         //
};

