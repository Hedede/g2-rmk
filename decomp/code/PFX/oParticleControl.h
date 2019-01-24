struct oCParticleControl {
	zCVob* GetPFXVob()
	{
		return pfxVob;
	}

	void SetStartPos(zVEC3& pos)
	{
		startPos = pos;
	}

	zCParticleFX *pfx;
	zCVob *pfxVob;
	int unk1;
	zSTRING pfxName;
	zVEC3 startPos;
	int unk2[2];
	zCConsole *pfxConsole;
};

