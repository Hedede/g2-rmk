class zCArchiverFactory {
	virtual void CreateArchiverRead(zSTRING const &,int);
 virtual void CreateArchiverRead(zFILE *,int);
 virtual void CreateArchiverRead(zCBuffer *,int);
	virtual void CreateArchiverWrite(zSTRING const &,zTArchiveMode,int,int);
 virtual void CreateArchiverWrite(zFILE *,zTArchiveMode,int,int);
 virtual void CreateArchiverWrite(zTArchiveMode,int,int);
 virtual void CreateArchiverWrite(zCBuffer *,zTArchiveMode,int,int);
};
