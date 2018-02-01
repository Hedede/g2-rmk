class zCCSDebugger {
public:
	virtual void CSDB_BeginSyncBlock(zCCSSyncBlock *) = 0;
	virtual void CSDB_BeginAtomBlock(zCCSBlockBase *) = 0;
	virtual void CSDB_EndAtomBlock(zCCSBlockBase *) = 0;
	virtual void CSDB_Error(zCCSBlockBase *,zSTRING &) = 0;
	virtual void CSDB_Warning(zCCSBlockBase *,zSTRING &) = 0;
	virtual void CSDB_StartPlaying(zCCutscene *) = 0;
	virtual void CSDB_Stop(zCCutscene *) = 0;
};
