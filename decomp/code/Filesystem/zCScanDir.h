class zCScanDir {
	virtual void ~zCScanDir(uint);
	virtual void RegisterFileHandler(zCScanDirFileHandler *,zSTRING const &);
	virtual void Scan(zSTRING	const &);
};
