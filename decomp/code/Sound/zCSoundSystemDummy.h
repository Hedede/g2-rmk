class zCSoundSystemDummy : zCSoundSystem {
	virtual ~zCSoundSystemDummy();
	zCSoundFX* LoadSoundFX(zSTRING const &) override
	{
		return new zCSoundFXDummy;
	}
	void PlaySound(zCSoundFX *,int,int,float,float) override
	{
		return nullptr;
	}
	void PlaySound(zCSoundFX *,int) override
	{
		return nullptr;
	}
	void PlaySound3D(zSTRING const &,zCVob *,int,zCSoundSystem::zTSound3DParams	*) override
	{
		return nullptr;
	}
	void PlaySound3D(zCSoundFX *,zCVob *,int,zCSoundSystem::zTSound3DParams *) override
	{
		return nullptr;
	}
	void GetSound3DProps(int const &,zCSoundSystem::zTSound3DParams &) override
	{
		return nullptr;
	}
	void UpdateSound3D(int const &,zCSoundSystem::zTSound3DParams*) override
	{
		return nullptr;
	}
	void DoSoundUpdate() override {}
	void SetGlobalOcclusion(float) override {}
};
