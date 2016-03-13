class zCBspBase {
public:
	void CollectVobsInBBox3D(zCArray<zCVob> const& vobs, zTBBox3D const& bbox)
	{
		++bspCollectCtr;
		CollectVobsInBBox3D_I(vobs, bbox);
	}
};
