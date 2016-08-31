struct oCEmitterKey {
	struct {
		// particle effect related key vars
		// vars which influence all particles all time
		zSTRING visName;
		float visSizeScale;
		float scaleDuration;
		float pfx_ppsValue;
		zBOOL pfx_ppsIsSmoothChg;
		zBOOL pfx_ppsIsLoopingChg;
		float pfx_scTime;
		zSTRING pfx_flyGravity;

		// vars which influence particles at creation level only
		zSTRING pfx_shpDim;
		int pfx_shpIsVolumeChg;
		float pfx_shpScaleFPS;
		float pfx_shpDistribWalkSpeed;
		zSTRING pfx_shpOffsetVec;
		zSTRING pfx_shpDistribType;
		zSTRING pfx_dirMode;
		zSTRING pfx_dirFOR;
		zSTRING pfx_dirModeTargetFOR;
		zSTRING pfx_dirModeTargetPos;
		float pfx_velAvg;
		float pfx_lspPartAvg;
		float pfx_visAlphaStart;
		zSTRING lightPresetName;
		float lightRange;
		zSTRING sfxID;
		int sfxIsAmbient;
		zSTRING emCreateFXID;
		float emFlyGravity;
		zSTRING emSelfRotVel;
		zSTRING emTrjMode;
		float emTrjEaseVel;
		int emCheckCollision;
		float emFXLifeSpan;
	} scripted;

	char unk1[72];
	zSTRING instanceName;
	char unk2[24];
};

