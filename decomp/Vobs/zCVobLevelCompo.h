class zCVobLevelCompo : public zCVob {
	Z_OBJECT(zCVobLevelCompo);
private:
	static zBOOL unarcIgnoreVisuals;
public:
	virtual ~zCVobLevelCompo();

	virtual void Archive(zCArchiver& arc);
	virtual void Unarchive(zCArchiver& arc);
	virtual void SetVisual(zSTRING const &);
	virtual void SetVisual(zCVisual*);
};

void zCVobLevelCompo::Archive(zCArchiver& arc);
{
	for (int i = 0; i < 4; ++i) {
		if (trafoObjToWorld[i][i] > 1.001) {
			zMAT4 identity = Alg_Identity3D();
			SetTrafo(identity);
		}
	}

	zVob::Archive(arc);
}

void zCVobLevelCompo::Unarchive(zCArchiver& arc)
{
	auto tmp = zCVob::s_ignoreVisuals;
	zCVob::s_ignoreVisuals = zCVobLevelCompo::unarcIgnoreVisuals;
	zCVob::Unarchive(arc);
	zCVob::s_ignoreVisuals = tmp;

	if ( zCVob::s_ignoreVisuals )
		SetVisual(0);

	for (int i = 0; i < 4; ++i) {
		if (trafoObjToWorld[i][i] > 1.001) {
			zMAT4 identity = Alg_Identity3D();
			SetTrafo(identity);
		}
	}
}
