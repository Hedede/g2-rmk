class zCVobLevelCompo : public zCVob {
	Z_OBJECT(zCVobLevelCompo);
private:
	static zBOOL unarcIgnoreVisuals;
public:
	zCVobLevelCompo();
	~zCVobLevelCompo() override = default;

	void Archive(zCArchiver& arc) override;
	void Unarchive(zCArchiver& arc) override;
	void SetVisual(zSTRING const& visualFileName) override;
	void SetVisual(zCVisual* newVisual) override;

private:
	zBOOL HasIdentityTrafo() const;
};

zCVobLevelCompo::zCVobLevelCompo()
{
	SetCollDetStat(0);
	SetCollDetDyn(0);
}

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

	// was inlined
	if (!HasIdentityTrafo())
		SetTrafo( Alg_Identity3D() );
}

zBOOL zCVobLevelCompo::HasIdentityTrafo() const
{
	if ( trafoObjToWorld[0][3] > 0.001)
		return 0;

	for (int i = 0; i < 4; ++i)
	{
		if (trafoObjToWorld[i][i] > 1.001)
			return 0;
	}

	return 1;
}

void zCVobLevelCompo::SetVisual(zCVobLevelCompo *this, const struct zSTRING *visualFileName)
{
	auto matUsage = zCMaterial::s_matUsageDefault;
	zCMaterial::s_matUsageDefault = 0;
	zCVob::SetVisual(visualFileName);
	zCVob::SetCollDetStat(0);
	zCVob::SetCollDetDyn(0);
	zCMaterial::s_matUsageDefault = matUsage;
}

void zCVobLevelCompo::SetVisual(zCVobLevelCompo *this, struct zCVisual *newVisual)
{
	auto matUsage = zCMaterial::s_matUsageDefault;
	zCMaterial::s_matUsageDefault = 0;
	zCVob::SetVisual(newVisual);
	zCVob::SetCollDetStat(0);
	zCVob::SetCollDetDyn(0);
	zCMaterial::s_matUsageDefault = matUsage;
}
