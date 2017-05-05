class zCModelMeshLib : public zCObject {
	Z_OBJECT(zCModelMeshLib);
public:
	~zCModelMeshLib();

private:
	zCArray nodes;
	zCArray skins;
};
