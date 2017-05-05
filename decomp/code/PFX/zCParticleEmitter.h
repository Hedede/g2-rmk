struct zSParticle {
	zSParticle() = default;
};

struct zCParticleEmitter {
	zCParticleEmitter()
	{
		Reset();

		unk16[6] = 0;
		unk17[1] = 0;
		unk17[2] = 0;
		unk14[7] = 0;
		unk14[6] = 0;
		unk14[8] = 0;
		unk14[9] = 0;
	}

	void Reset()
	{
		memset(this, 0, sizeof(zCParticleEmitter));
		ResetStrings();
	}

	void ResetStrings()
	{
		ppsScaleKeys_S.Clear();
		ppsCreateEm_S.Clear();
		shpType_S.Clear();
		shpFOR_S.Clear();
		shpOffsetVec_S.Clear();
		shpDistribType_S.Clear();
		shpDim_S.Clear();
		shpMesh_S.Clear();
		shpScaleKeys_S.Clear();
		dirMode_S.Clear();
		dirFOR_S.Clear();
		dirModeTargetFOR_S.Clear();
		dirModeTargetPos_S.Clear();
		flyGravity_S.Clear();
		visName_S.Clear();
		visOrientation_S.Clear();
		visSizeStart_S.Clear();
		visTexColorStart_S.Clear();
		visTexColorEnd_S.Clear();
		visAlphaFunc_S.Clear();
		trlTexture_S.Clear();
		mrkTexture_S.Clear();
		flockMode.Clear();
		timeStartEnd_S.Clear();
	}

	void UpdateVelocity() {}

public:

	// 1) Emitter: zeitliches  Austoss-Verhalten, particles-per-second
	float    ppsValue;
	zSTRING  ppsScaleKeys_S;
	int      ppsIsLooping;
	int      ppsIsSmooth;
	float    ppsFPS;
	zSTRING  ppsCreateEm_S;
	float    ppsCreateEmDelay;

	// 2) Emitter: raeumliches Austoss-Verhalten
	zSTRING   shpType_S;     // "point, line, box, circle, sphere, mesh"
	zSTRING   shpFOR_S;     // "object,world"
	zSTRING   shpOffsetVec_S;
	zSTRING   shpDistribType_S;    // "RAND, UNIFORM, WALK"
	float     shpDistribWalkSpeed;
	int       shpIsVolume;
	// "", "30", "10 20 30", "30", "30", ""
	// line: nur 1 Dimension !=0 // shape Dimensions
	zSTRING   shpDim_S;
	zSTRING   shpMesh_S;     // "cross.3ds"
	int       shpMeshRender_B;
	zSTRING   shpScaleKeys_S;    // "[1.0] [0.8 0.9 0.2] [1.0]"
	int       shpScaleIsLooping;
	int       shpScaleIsSmooth;
	float     shpScaleFPS;

	// 3) Partikel: Start Richtung/Speed:
	zSTRING   dirMode_S;    // "DIR, TARGET, MESH_POLY"
	zSTRING   dirFOR_S;     // "OBJECT, WORLD"
	zSTRING   dirModeTargetFOR_S;
	zSTRING   dirModeTargetPos_S;   // "30 23 67"
	float     dirAngleHead;
	float     dirAngleHeadVar;
	float     dirAngleElev;
	float     dirAngleElevVar;
	float     velAvg;
	float     velVar;

	// 4) Partikel: Lebensdauer
	float     lspPartAvg;
	float     lspPartVar;

	// 5) Partikel: Flugverhalten (gravity, nicht-linear?, mesh-selfRot?,..)
	// grav: a) nur Y, b) XYZ, c) auf Ziel zu steuern
	// zSTRING  flyMode_S;        // "LINEAR, LIN_SINUS,.."
	// flyMeshSelfRotSpeedMin, flyMeshSelfRotSpeedMax
	zSTRING  flyGravity_S;
	int      flyCollDet_B;

	// 6) Partikel: Visualisierung
	zSTRING  visName_S;     // "NAME_V0_A0.TGA/.3DS" (Variation, Animation)
	zSTRING  visOrientation_S;   // "NONE, VELO"
	int      visTexIsQuadPoly;   // 0=triMesh, 1=quadMesh
	float    visTexAniFPS;
	int      visTexAniIsLooping;   // 0=oneShot, 1=looping
	// color  (nur Tex, lifeSpan-Sync)   
	zSTRING  visTexColorStart_S;
	zSTRING  visTexColorEnd_S;
	// size-ani  (nur Tex, lifeSpan-Sync)
	zSTRING  visSizeStart_S;
	float    visSizeEndScale;
	// alpha  (lifeSpan-Sync)
	zSTRING  visAlphaFunc_S;
	float    visAlphaStart;
	float    visAlphaEnd;

	// 7) misc effects
	
	// trail
	float    trlFadeSpeed;
	zSTRING  trlTexture_S;
	floa     trlWidth;

	// marks
	float    mrkFadeSpeed;
	zSTRING  mrkTexture_S;
	float    mrkSize;
	
	// flocking
	zSTRING  flockMode;
	float    flockStrength;
	
	// local frame of reference override
	int      useEmittersFOR;
	
	zSTRING  timeStartEnd_S;
	
	int      m_bIsAmbientPFX;

	int unk1;

	zSTRING pfxName;

	zCArray<float> arr1;

	zCParticleEmitter* __next;
	int __someEnum;
	int __sphereRadius;
	zVEC3 __sphereCenter;

	int unk14;
	
	// something-something PFXMesh
	zCMesh *__mesh;
	zCObject *__morphMesh;
	zCProgMesh *__progMesh;

	int unk14_3[5];

	zCArray<float> arr2;
	int unk15[6];
	zTBBox3D bbox;
	zVEC3 __outputDir;
	int unk16[5];
	zVEC2 bs;
	zVEC3 vec1;
	zVEC3 vec2;
	int unk17;
	zCTexture *__texture;
	int unk18;
	int __someBool;
	int unk19[6];
};
