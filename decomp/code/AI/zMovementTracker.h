struct zCPose {
	~zCPose() = default;
};

struct zCMovementTracker {

private:
	void CheckKeys() {}

private:
	zTAICamMsg __msg;
	float unk0[4];
	zVEC3 __sampledPos;
	float unk00[3];
	zVEC3 playerPos;
	zVEC3 playerPos2;
	char unk1[12];
	zVEC3 __testPos;
	zVEC3 __camPos2;
	int unk2[16];
	zCPose __pose1;
	zCPose __pose2;
	zCPose __pose3;
	int unk3[102];
	zMAT4 __mat;
	int unko4[32];
	zMAT4 __mat2;
	zMAT4 __mat3;
	zMAT4 __mat4;
	zMAT4 __mat5;
	zMAT4 __mat6;
	zMAT4 __mat7;
	zMAT4 __mat8;
	int unk4[3];
	int rotateEnabled;
	int unk5[3];
	oCNpc *player;
	zCPathSearch *pathSearch;
	zCAICamera *aiCam;
	int unkoooo;
};

