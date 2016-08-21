struct zCCSCamera : zCVob {
	int camTrhFOR;
	int targetTrjFor;
	int loopMode;
	int splLerpMode;
	zBOOL ignoreFORVobRotCam;
	zBOOL ignoreFORVobRotTarget;
	zBOOL adaptToSurroundings;
	zBOOL easeToFirstKey;
	zBOOL easeFromLastKey;
	float totalTime;
	zSTRING autoCamFocusVobName;
	zBOOL autoCamPlayerMovable;
	zBOOL autoCamUntriggerOnLastKey;
	float autoCamUntriggerOnLastKeyDelay;
	int WHAT[5];
	zCArray keys;
	zCArray targets;
	zBOOL paused;
	int WHUT[5];
	zBOOL started;
	zBOOL gotoTimeMode;
	int WHET[8];
	float csTime;
	char WHIT[68];
	float _FLOAT_1;
	int WHOOT;
	float _FLOAT_2;
	int WHAD[4];
	zCPose __pose1;
	zCPose __pose2;
	zCKBSpline _SPLINE_1;
	zCKBSpline _SPLINE_2;
	void *_LERP_1;
	void *_LERP_2;
	zMAT4 _MAT_1;
	zMAT4 _MAT_2;
	char asd[12];
};