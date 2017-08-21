class zCCSDebugger {
public:
	virtual void CSDB_BeginSyncBlock(zCCSSyncBlock *);
	virtual void CSDB_BeginAtomBlock(zCCSBlockBase *);
	virtual void CSDB_EndAtomBlock(zCCSBlockBase *);
	virtual void CSDB_Error(zCCSBlockBase *,zSTRING &);
	virtual void CSDB_Warning(zCCSBlockBase *,zSTRING &);
	virtual void CSDB_StartPlaying(zCCutscene *);
	virtual void CSDB_Stop(zCCutscene *);
};
